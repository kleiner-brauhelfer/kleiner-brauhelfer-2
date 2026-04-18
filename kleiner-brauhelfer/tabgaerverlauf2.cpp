#include "tabgaerverlauf2.h"
#include "ui_tabgaerverlauf2.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
#include "model/datetimedelegate.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/textdelegate.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "widgets/widgetdecorator.h"

extern Settings* gSettings;

TabGaerverlauf2::TabGaerverlauf2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabGaerverlauf2),
    mSud(nullptr)
{
    ui->setupUi(this);
}

TabGaerverlauf2::~TabGaerverlauf2()
{
    delete ui;
}

void TabGaerverlauf2::setup(SudObject *sud)
{
    mSud = sud;

    gSettings->beginGroup(staticMetaObject.className());

    TableView *table = ui->tableWidget_Nachgaerverlauf;
    table->setModel(mSud->modelNachgaerverlauf());
    table->appendCol({ModelNachgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColTage, true, true, 100, new SpinBoxDelegate(table)});
    table->appendCol({ModelNachgaerverlauf::ColDruck, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, 10.0, 0.1, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColCO2, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelNachgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateNachgaerung").toByteArray());
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabGaerverlauf2::table_selectionChanged);
    connect(ui->widget_DiaNachgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf2::diagram_selectionChanged);

    mDefaultSplitterStateNachgaerung = ui->splitterNachgaerung->saveState();
    ui->splitterNachgaerung->restoreState(gSettings->value("splitterStateNachgaerung").toByteArray());

    gSettings->endGroup();

    connect(mSud->bh(), &Brauhelfer::discarded, this, &TabGaerverlauf2::sudLoaded);
    connect(mSud, &SudObject::loadedChanged, this, &TabGaerverlauf2::sudLoaded);
    connect(mSud, &SudObject::modified, this, &TabGaerverlauf2::updateValues);
    connect(mSud->modelNachgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf2::updateDiagramm);
    connect(mSud->modelNachgaerverlauf(), &ProxyModel::rowsInserted,this, &TabGaerverlauf2::updateDiagramm);
    connect(mSud->modelNachgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf2::updateDiagramm);
    connect(mSud->modelNachgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf2::dataChangedNachgaerverlauf);

    WidgetDecorator::addDecortor(findChildren<QWidget*>());
}

void TabGaerverlauf2::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateDiagramm();
}

void TabGaerverlauf2::changeEvent(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
    QWidget::changeEvent(event);
}

void TabGaerverlauf2::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableStateNachgaerung", ui->tableWidget_Nachgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("splitterStateNachgaerung", ui->splitterNachgaerung->saveState());
    gSettings->endGroup();
}

void TabGaerverlauf2::restoreView()
{
    ui->tableWidget_Nachgaerverlauf->restoreDefaultState();
    ui->splitterNachgaerung->restoreState(mDefaultSplitterStateNachgaerung);
}

void TabGaerverlauf2::modulesChanged(Settings::Modules modules)
{
    if (mSud->isLoaded())
    {
        updateValues();
    }
}

void TabGaerverlauf2::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (ui->tableWidget_Nachgaerverlauf->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnDelNachgaerMessung_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnAddNachgaerMessung_clicked();
            break;
        default:
            if (event->matches(QKeySequence::Paste))
                if (ui->tableWidget_Nachgaerverlauf->editTriggers() != QAbstractItemView::EditTrigger::NoEditTriggers)
                    pasteFromClipboardNachgaerverlauf(QApplication::clipboard()->text());
            break;
        }
    }
}

void TabGaerverlauf2::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TabGaerverlauf2::dropEvent(QDropEvent *event)
{
    for (const QUrl& url : event->mimeData()->urls())
    {
        QFile file(url.toLocalFile());
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            pasteFromClipboardNachgaerverlauf(file.readAll());
        }
    }
}

void TabGaerverlauf2::sudLoaded()
{
    updateDiagramm();
    updateValues();
}

void TabGaerverlauf2::checkEnabled()
{
    bool enabled;
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    enabled = status == Brauhelfer::SudStatus::Abgefuellt  || gSettings->ForceEnabled;
    ui->wdgEditNachgaerung->setVisible(enabled);
    ui->tableWidget_Nachgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
}

void TabGaerverlauf2::updateValues()
{
    checkEnabled();
}

void TabGaerverlauf2::dataChangedNachgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelNachgaerverlauf::ColZeitstempel)
        mSud->modelNachgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf2::updateDiagramm()
{
    Chart3 *diag;
    ProxyModel *model = mSud->modelNachgaerverlauf();
    QVector<double> x = QVector<double>(model->rowCount());
    QVector<double> y1 = QVector<double>(model->rowCount());
    QVector<double> y2 = QVector<double>(model->rowCount());
    QVector<double> y3 = QVector<double>(model->rowCount());
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelNachgaerverlauf::ColZeitstempel).data().toDateTime();
        x[row] = dt.toSecsSinceEpoch();
        y1[row] = model->index(row, ModelNachgaerverlauf::ColCO2).data().toDouble();
        y2[row] = model->index(row, ModelNachgaerverlauf::ColDruck).data().toDouble();
        y3[row] = model->index(row, ModelNachgaerverlauf::ColTemp).data().toDouble();
    }
    diag = ui->widget_DiaNachgaerverlauf;
    diag->clear();
    diag->setData1(x, y1, tr("CO₂"), QStringLiteral("g/L"), 1);
    diag->setData2(x, y2, tr("Druck"), QStringLiteral("bar"), 2);
    diag->setData3(x, y3, tr("Temperatur"), QStringLiteral("°C"), 1);
    diag->setData1Limit(mSud->getCO2());
    diag->rescale();
    diag->replot();
}

void TabGaerverlauf2::table_selectionChanged(const QItemSelection &selected)
{
    int index = -1;
    if (selected.indexes().count() > 0)
        index = selected.indexes()[0].row();
    ui->widget_DiaNachgaerverlauf->select(index);
}

void TabGaerverlauf2::diagram_selectionChanged(int index)
{
    ui->tableWidget_Nachgaerverlauf->selectRow(index);
}

void TabGaerverlauf2::on_btnAddNachgaerMessung_clicked()
{
    QMap<int, QVariant> values({{ModelNachgaerverlauf::ColSudID, mSud->id()}});
    int row = mSud->modelNachgaerverlauf()->append(values);
    if (row >= 0)
    {
        ui->tableWidget_Nachgaerverlauf->setCurrentIndex(mSud->modelNachgaerverlauf()->index(row, ModelNachgaerverlauf::ColDruck));
        ui->tableWidget_Nachgaerverlauf->scrollTo(ui->tableWidget_Nachgaerverlauf->currentIndex());
        ui->tableWidget_Nachgaerverlauf->edit(ui->tableWidget_Nachgaerverlauf->currentIndex());
    }
}

void TabGaerverlauf2::on_btnDelNachgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Nachgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : std::as_const(indices))
        mSud->modelNachgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf2::on_btnImportNachgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    QStringLiteral(""),
                                                    tr("CSV Datei (*.csv);;Alle Dateien (*.*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            pasteFromClipboardNachgaerverlauf(file.readAll());
        }
    }
}

QDateTime TabGaerverlauf2::toDateTime(QString string) const
{
    string.remove('\"').remove('\'');
  #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QDateTime dt = QDateTime::fromString(string, Qt::TextDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::ISODate);
  #else
    QDateTime dt = QDateTime::fromString(string, Qt::SystemLocaleShortDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::DefaultLocaleShortDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::TextDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::ISODate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::SystemLocaleLongDate);
  #endif
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("d.M.yy h:m:s"));
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("d.M.yy h:m"));
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("d.M.yyyy h:m:s"));
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("d.M.yyyy h:m"));
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("M/d/yyyy h:m:s"));
    if (!dt.isValid())
        dt = QDateTime::fromString(string, QStringLiteral("M/d/yyyy h:m"));
    if (dt.isValid())
    {
        if (dt.date().year() < 2000)
            dt = dt.addYears(100);
    }
    return dt;
}

double TabGaerverlauf2::toDouble(QString string, bool *ok) const
{
    bool _ok = false;
    string.remove('\"').remove('\'');
    double val = string.toDouble(&_ok);
    if (!_ok)
        val = QLocale().toDouble(string, &_ok);
    if (ok)
        *ok = _ok;
    return val;
}

void TabGaerverlauf2::pasteFromClipboardNachgaerverlauf(const QString& str)
{
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QStringList rows = str.split(QStringLiteral("\n"), Qt::SkipEmptyParts);
  #else
    QStringList rows = str.split("\n", QString::SkipEmptyParts);
  #endif
    if (rows.size() == 0)
        rows.append(str);
    bool error = false;
    bool firstRow = true;
    bool wasBlocked = mSud->bh()->modelNachgaerverlauf()->blockSignals(true);
    for (const QString& row : std::as_const(rows))
    {
        QStringList cols = row.split(QStringLiteral("\t"));
        if (cols.size() == 1)
            cols = row.split(QStringLiteral(";"));
        if (cols.size() == 1)
            cols = row.split(QStringLiteral(","));
        if (cols.size() > 1)
        {
            QDateTime dt = toDateTime(cols[0]);
            if (dt.isValid())
            {
                QMap<int, QVariant> values = {{ModelNachgaerverlauf::ColSudID, mSud->id()},
                                              {ModelNachgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double druck = toDouble(cols[1], &ok);
                    if (ok)
                    {
                        values[ModelNachgaerverlauf::ColDruck] = druck;
                        if (cols.size() > 2)
                        {
                            double temp = toDouble(cols[2], &ok);
                            if (ok)
                            {
                                values[ModelNachgaerverlauf::ColTemp] = temp;
                                values[ModelNachgaerverlauf::ColCO2] = BierCalc::co2(druck, temp);
                            }
                        }
                    }
                }
                if (cols.size() > 3)
                    values[ModelNachgaerverlauf::ColBemerkung] = cols[3];
                if (cols.size() > 4)
                    values[ModelNachgaerverlauf::ColBemerkung] = cols[4];
                mSud->bh()->modelNachgaerverlauf()->appendDirect(values);
            }
            else
            {
                error = !firstRow;
            }
        }
        else
        {
            error = !firstRow;
        }
        firstRow = false;
    }
    mSud->bh()->modelNachgaerverlauf()->blockSignals(wasBlocked);
    mSud->bh()->modelNachgaerverlauf()->emitModified();
    mSud->modelNachgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Druck1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Druck2;Temperatur2[;Bemerkung2]"));
    }
}
