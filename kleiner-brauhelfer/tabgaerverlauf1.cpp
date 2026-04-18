#include "tabgaerverlauf1.h"
#include "ui_tabgaerverlauf1.h"
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
#include "model/restextraktdelegate.h"
#include "model/textdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "widgets/widgetdecorator.h"

extern Settings* gSettings;

TabGaerverlauf1::TabGaerverlauf1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabGaerverlauf1),
    mSud(nullptr)
{
    ui->setupUi(this);
}

TabGaerverlauf1::~TabGaerverlauf1()
{
    delete ui;
}

void TabGaerverlauf1::setup(SudObject *sud)
{
    mSud = sud;

    gSettings->beginGroup(staticMetaObject.className());

    TableView *table = ui->tableWidget_Schnellgaerverlauf;
    table->setModel(mSud->modelSchnellgaerverlauf());
    table->appendCol({ModelSchnellgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColTage, true, true, 100, new SpinBoxDelegate(table)});
    table->appendCol({ModelSchnellgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateSchnellgaerung").toByteArray());
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabGaerverlauf1::table_selectionChangedSchnellgaerverlauf);
    connect(ui->widget_DiaSchnellgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf1::diagram_selectionChangedSchnellgaerverlauf);

    table = ui->tableWidget_Hauptgaerverlauf;
    table->setModel(mSud->modelHauptgaerverlauf());
    table->appendCol({ModelHauptgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelHauptgaerverlauf::ColTage, true, true, 100, new SpinBoxDelegate(table)});
    table->appendCol({ModelHauptgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(true, table)});
    table->appendCol({ModelHauptgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelHauptgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateHauptgaerung").toByteArray());
    connect(table->selectionModel(),  &QItemSelectionModel::selectionChanged, this,  &TabGaerverlauf1::table_selectionChangedHauptgaerverlauf);
    connect(ui->widget_DiaHauptgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf1::diagram_selectionChangedHauptgaerverlauf);

    mDefaultSplitterState = ui->splitterSchnellgaerung->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    mDefaultSplitterStateSchnellgaerung = ui->splitterSchnellgaerung->saveState();
    ui->splitterSchnellgaerung->restoreState(gSettings->value("splitterStateSchnellgaerung").toByteArray());

    mDefaultSplitterStateHauptgaerung = ui->splitterHauptgaerung->saveState();
    ui->splitterHauptgaerung->restoreState(gSettings->value("splitterStateHauptgaerung").toByteArray());

    gSettings->endGroup();

    connect(mSud->bh(), &Brauhelfer::discarded, this, &TabGaerverlauf1::sudLoaded);
    connect(mSud, &SudObject::loadedChanged, this, &TabGaerverlauf1::sudLoaded);
    connect(mSud, &SudObject::modified, this, &TabGaerverlauf1::updateValues);
    connect(mSud, &SudObject::dataChanged, this, &TabGaerverlauf1::sudDataChanged);
    connect(mSud->modelSchnellgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelSchnellgaerverlauf(), &ProxyModel::rowsInserted, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelSchnellgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelSchnellgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf1::dataChangedSchnellgaerverlauf);
    connect(mSud->modelHauptgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelHauptgaerverlauf(), &ProxyModel::rowsInserted, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelHauptgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf1::updateDiagramm);
    connect(mSud->modelHauptgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf1::dataChangedHauptgaerverlauf);
    connect(mSud->modelHefegaben(), &ProxyModel::layoutChanged, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelHefegaben(), &ProxyModel::rowsInserted, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelHefegaben(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelHefegaben(), &ProxyModel::dataChanged, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::layoutChanged, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::rowsInserted, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf1::updateWeitereZutaten);
    connect(mSud->modelWeitereZutatenGaben(), &ProxyModel::dataChanged, this, &TabGaerverlauf1::updateWeitereZutaten);

    WidgetDecorator::addDecortor(findChildren<QWidget*>());
}

void TabGaerverlauf1::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateDiagramm();
}

void TabGaerverlauf1::changeEvent(QEvent* event)
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

void TabGaerverlauf1::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableStateSchnellgaerung", ui->tableWidget_Schnellgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("tableStateHauptgaerung", ui->tableWidget_Hauptgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterStateSchnellgaerung", ui->splitterSchnellgaerung->saveState());
    gSettings->setValue("splitterStateHauptgaerung", ui->splitterHauptgaerung->saveState());
    gSettings->endGroup();
}

void TabGaerverlauf1::restoreView()
{
    ui->tableWidget_Schnellgaerverlauf->restoreDefaultState();
    ui->tableWidget_Hauptgaerverlauf->restoreDefaultState();
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterSchnellgaerung->restoreState(mDefaultSplitterStateSchnellgaerung);
    ui->splitterHauptgaerung->restoreState(mDefaultSplitterStateHauptgaerung);
}

void TabGaerverlauf1::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleSchnellgaerprobe))
    {
        ui->groupBoxSchnellgaerprobe->setVisible(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    }
    if (mSud->isLoaded())
    {
        updateValues();
    }
}

void TabGaerverlauf1::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (ui->tableWidget_Schnellgaerverlauf->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnDelSchnellgaerMessung_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnAddSchnellgaerMessung_clicked();
            break;
        default:
            if (event->matches(QKeySequence::Paste))
                if (ui->tableWidget_Schnellgaerverlauf->editTriggers() != QAbstractItemView::EditTrigger::NoEditTriggers)
                    pasteFromClipboardSchnellgaerverlauf(QApplication::clipboard()->text());
            break;
        }
    }
    else if (ui->tableWidget_Hauptgaerverlauf->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnDelHauptgaerMessung_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnAddHauptgaerMessung_clicked();
            break;
        default:
            if (event->matches(QKeySequence::Paste))
                if (ui->tableWidget_Hauptgaerverlauf->editTriggers() != QAbstractItemView::EditTrigger::NoEditTriggers)
                    pasteFromClipboardHauptgaerverlauf(QApplication::clipboard()->text());
            break;
        }
    }
}

void TabGaerverlauf1::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TabGaerverlauf1::dropEvent(QDropEvent *event)
{
    for (const QUrl& url : event->mimeData()->urls())
    {
        QFile file(url.toLocalFile());
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            pasteFromClipboardHauptgaerverlauf(file.readAll());
        }
    }
}

void TabGaerverlauf1::sudLoaded()
{
    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
}

void TabGaerverlauf1::sudDataChanged(const QModelIndex& index)
{
    if (index.column() == ModelSud::ColStatus)
    {
        updateWeitereZutaten();
    }
}

void TabGaerverlauf1::checkEnabled()
{
    bool enabled;
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(mSud->getStatus());
    enabled = status == Brauhelfer::SudStatus::Gebraut || gSettings->ForceEnabled;
    ui->wdgEditSchnellgaerung->setVisible(enabled);
    ui->tableWidget_Schnellgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = status == Brauhelfer::SudStatus::Gebraut  || gSettings->ForceEnabled;
    ui->wdgEditHauptgaerung1->setVisible(enabled);
    ui->tableWidget_Hauptgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
}

void TabGaerverlauf1::updateValues()
{
    checkEnabled();
}

void TabGaerverlauf1::dataChangedSchnellgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelSchnellgaerverlauf::ColZeitstempel)
        mSud->modelSchnellgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf1::dataChangedHauptgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelHauptgaerverlauf::ColZeitstempel)
        mSud->modelHauptgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf1::updateDiagramm()
{
    Chart3 *diag;
    ProxyModel *model = mSud->modelSchnellgaerverlauf();
    QVector<double> x(model->rowCount());
    QVector<double> y1(model->rowCount());
    QVector<double> y2(model->rowCount());
    QVector<double> y3(model->rowCount());
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelSchnellgaerverlauf::ColZeitstempel).data().toDateTime();
        x[row] = dt.toSecsSinceEpoch();
        y1[row] = model->index(row, ModelSchnellgaerverlauf::ColRestextrakt).data().toDouble();
        y2[row] = model->index(row, ModelSchnellgaerverlauf::ColAlc).data().toDouble();
        y3[row] = model->index(row, ModelSchnellgaerverlauf::ColTemp).data().toDouble();
    }
    diag = ui->widget_DiaSchnellgaerverlauf;
    diag->clear();
    diag->setData1(x, y1, tr("Restextrakt"), QStringLiteral("°P"), 1);
    diag->setData2(x, y2, tr("Alkohol"), QStringLiteral("%"), 1);
    diag->setData3(x, y3, tr("Temperatur"), QStringLiteral("°C"), 1);
    diag->setData1Limit(mSud->getSREErwartet());
    diag->rescale();
    diag->replot();

    model = mSud->modelHauptgaerverlauf();
    x = QVector<double>(model->rowCount());
    y1 = QVector<double>(model->rowCount());
    y2 = QVector<double>(model->rowCount());
    y3 = QVector<double>(model->rowCount());
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelHauptgaerverlauf::ColZeitstempel).data().toDateTime();
        x[row] = dt.toSecsSinceEpoch();
        y1[row] = model->index(row, ModelHauptgaerverlauf::ColRestextrakt).data().toDouble();
        y2[row] = model->index(row, ModelHauptgaerverlauf::ColAlc).data().toDouble();
        y3[row] = model->index(row, ModelHauptgaerverlauf::ColTemp).data().toDouble();
    }
    diag = ui->widget_DiaHauptgaerverlauf;
    diag->clear();
    diag->setData1(x, y1, tr("Restextrakt"), QStringLiteral("°P"), 1);
    diag->setData2(x, y2, tr("Alkohol"), QStringLiteral("%"), 1);
    diag->setData3(x, y3, tr("Temperatur"), QStringLiteral("°C"), 1);
    if (mSud->getSchnellgaerprobeAktiv())
        diag->setData1Limit(mSud->getGruenschlauchzeitpunkt());
    else
        diag->setData1Limit(mSud->getSREErwartet());
    diag->rescale();
    diag->replot();

    model = mSud->modelNachgaerverlauf();
    x = QVector<double>(model->rowCount());
    y1 = QVector<double>(model->rowCount());
    y2 = QVector<double>(model->rowCount());
    y3 = QVector<double>(model->rowCount());
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelNachgaerverlauf::ColZeitstempel).data().toDateTime();
        x[row] = dt.toSecsSinceEpoch();
        y1[row] = model->index(row, ModelNachgaerverlauf::ColCO2).data().toDouble();
        y2[row] = model->index(row, ModelNachgaerverlauf::ColDruck).data().toDouble();
        y3[row] = model->index(row, ModelNachgaerverlauf::ColTemp).data().toDouble();
    }
}

void TabGaerverlauf1::table_selectionChangedSchnellgaerverlauf(const QItemSelection &selected)
{
    int index = -1;
    if (selected.indexes().count() > 0)
        index = selected.indexes()[0].row();
    ui->widget_DiaSchnellgaerverlauf->select(index);
}

void TabGaerverlauf1::table_selectionChangedHauptgaerverlauf(const QItemSelection &selected)
{
    int index = -1;
    if (selected.indexes().count() > 0)
        index = selected.indexes()[0].row();
    ui->widget_DiaHauptgaerverlauf->select(index);
}

void TabGaerverlauf1::diagram_selectionChangedSchnellgaerverlauf(int index)
{
    ui->tableWidget_Schnellgaerverlauf->selectRow(index);
}

void TabGaerverlauf1::diagram_selectionChangedHauptgaerverlauf(int index)
{
    ui->tableWidget_Hauptgaerverlauf->selectRow(index);
}

void TabGaerverlauf1::on_btnAddSchnellgaerMessung_clicked()
{
    ProxyModel *model = mSud->modelSchnellgaerverlauf();
    double re = model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColRestextrakt).toDouble();
    double temp = model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColTemp).toDouble();
    DlgRestextrakt dlg(re, mSud->getSWIst(), temp, QDateTime::currentDateTime(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelSchnellgaerverlauf::ColSudID, mSud->id()},
                                    {ModelSchnellgaerverlauf::ColZeitstempel, dlg.datum()},
                                    {ModelSchnellgaerverlauf::ColRestextrakt, dlg.value()},
                                    {ModelSchnellgaerverlauf::ColTemp, dlg.temperatur()}});
        int row = model->append(values);
        if (row >= 0)
        {
            ui->tableWidget_Schnellgaerverlauf->setCurrentIndex(model->index(row, ModelSchnellgaerverlauf::ColRestextrakt));
            ui->tableWidget_Schnellgaerverlauf->scrollTo(ui->tableWidget_Schnellgaerverlauf->currentIndex());
        }
    }
}

void TabGaerverlauf1::on_btnDelSchnellgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Schnellgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : std::as_const(indices))
        mSud->modelSchnellgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf1::on_btnImportSchnellgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    QStringLiteral(""),
                                                    tr("CSV Datei (*.csv);;Alle Dateien (*.*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            pasteFromClipboardSchnellgaerverlauf(file.readAll());
        }
    }
}

void TabGaerverlauf1::on_btnAddHauptgaerMessung_clicked()
{
    ProxyModel *model = mSud->modelHauptgaerverlauf();
    double re = model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColRestextrakt).toDouble();
    double temp = model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColTemp).toDouble();
    DlgRestextrakt dlg(re, mSud->getSWIst(), temp, QDateTime::currentDateTime(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelHauptgaerverlauf::ColSudID, mSud->id()},
                                    {ModelHauptgaerverlauf::ColZeitstempel, dlg.datum()},
                                    {ModelHauptgaerverlauf::ColRestextrakt, dlg.value()},
                                    {ModelHauptgaerverlauf::ColTemp, dlg.temperatur()}});
        int row = model->append(values);
        if (row >= 0)
        {
            ui->tableWidget_Hauptgaerverlauf->setCurrentIndex(model->index(row, ModelHauptgaerverlauf::ColRestextrakt));
            ui->tableWidget_Hauptgaerverlauf->scrollTo(ui->tableWidget_Hauptgaerverlauf->currentIndex());
        }
    }
}

void TabGaerverlauf1::updateWeitereZutaten()
{
    ui->comboBox_GaerungEwzAuswahl->clear();
    ui->comboBox_GaerungEwzAuswahlEntnahme->clear();
    for (int i = 0; i < mSud->modelWeitereZutatenGaben()->rowCount(); ++i)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(mSud->modelWeitereZutatenGaben()->index(i, ModelWeitereZutatenGaben::ColZeitpunkt).data().toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
        {
            QString name = mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColName).toString();
            int id = mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColID).toInt();
            Brauhelfer::ZusatzTyp zusatztyp = static_cast<Brauhelfer::ZusatzTyp>(mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColTyp).toInt());
            Brauhelfer::RohstoffTyp typ = zusatztyp == Brauhelfer::ZusatzTyp::Hopfen ? Brauhelfer::RohstoffTyp::Hopfen : Brauhelfer::RohstoffTyp::Zusatz;
            Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColZugabestatus).toInt());
            bool entnahme = !mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColEntnahmeindex).toBool();
            int menge = mSud->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toInt();
            if (status == Brauhelfer::ZusatzStatus::NichtZugegeben)
                ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<Brauhelfer::RohstoffTyp,int>(typ, id)));
            else if (status == Brauhelfer::ZusatzStatus::Zugegeben && entnahme)
                ui->comboBox_GaerungEwzAuswahlEntnahme->addItem(name, id);
        }
    }
    for (int i = 0; i < mSud->modelHefegaben()->rowCount(); ++i)
    {
        if (!mSud->modelHefegaben()->data(i, ModelHefegaben::ColZugegeben).toBool())
        {

            QString name = mSud->modelHefegaben()->data(i, ModelHefegaben::ColName).toString();
            int id = mSud->modelHefegaben()->data(i, ModelHefegaben::ColID).toInt();
            int menge = mSud->modelHefegaben()->data(i, ModelHefegaben::ColMenge).toInt();
            ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<Brauhelfer::RohstoffTyp,int>(Brauhelfer::RohstoffTyp::Hefe, id)));
        }
    }
    ui->widget_EwzZugeben->setVisible(ui->comboBox_GaerungEwzAuswahl->count() > 0);
    ui->widget_EwzEntnehmen->setVisible(ui->comboBox_GaerungEwzAuswahlEntnahme->count() > 0);
}

void TabGaerverlauf1::on_btnGaerungEwzZugeben_clicked()
{
    QPair<Brauhelfer::RohstoffTyp,int> data = ui->comboBox_GaerungEwzAuswahl->currentData().value<QPair<Brauhelfer::RohstoffTyp,int>>();
    if (data.first == Brauhelfer::RohstoffTyp::Hefe)
    {
        int row = mSud->modelHefegaben()->getRowWithValue(ModelHefe::ColID, data.second);
        if (row >= 0)
        {
            mSud->modelHefegaben()->setData(row, ModelHefegaben::ColZugabeDatum, QDate::currentDate());
            mSud->modelHefegaben()->setData(row, ModelHefegaben::ColZugegeben, true);
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(mSud, true, data.first,
                                         mSud->modelHefegaben()->data(row, ModelHefegaben::ColName).toString(),
                                         mSud->modelHefegaben()->data(row, ModelHefegaben::ColMenge).toDouble(),
                                         this);
                dlg.exec();
            }
        }
    }
    else
    {
        int row = mSud->modelWeitereZutatenGaben()->getRowWithValue(ModelWeitereZutatenGaben::ColID, data.second);
        if (row >= 0)
        {
            mSud->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabeDatum, QDate::currentDate());
            mSud->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(mSud, true, data.first,
                                         mSud->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::ColName).toString(),
                                         mSud->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                                         this);
                dlg.exec();
            }
        }
    }
}

void TabGaerverlauf1::on_btnGaerungEwzEntnehmen_clicked()
{
    int id = ui->comboBox_GaerungEwzAuswahlEntnahme->currentData().toInt();
    int row = mSud->modelWeitereZutatenGaben()->getRowWithValue(ModelWeitereZutatenGaben::ColID, id);
    if (row >= 0)
    {
        mSud->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColEntnahmeDatum, QDate::currentDate());
        mSud->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Entnommen));
    }
}

void TabGaerverlauf1::on_btnDelHauptgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Hauptgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : std::as_const(indices))
        mSud->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf1::on_btnImportHauptgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    QStringLiteral(""),
                                                    tr("CSV Datei (*.csv);;Alle Dateien (*.*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            pasteFromClipboardHauptgaerverlauf(file.readAll());
        }
    }
}

QDateTime TabGaerverlauf1::toDateTime(QString string) const
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

double TabGaerverlauf1::toDouble(QString string, bool *ok) const
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

void TabGaerverlauf1::pasteFromClipboardSchnellgaerverlauf(const QString &str)
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
    bool wasBlocked = mSud->bh()->modelSchnellgaerverlauf()->blockSignals(true);
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
                QMap<int, QVariant> values = {{ModelSchnellgaerverlauf::ColSudID, mSud->id()},
                                              {ModelSchnellgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = toDouble(cols[1], &ok);
                    if (ok)
                    {
                        values[ModelSchnellgaerverlauf::ColRestextrakt] = sre;
                        values[ModelSchnellgaerverlauf::ColAlc] = BierCalc::alkohol(mSud->getSWIst(), sre);
                    }
                }
                if (cols.size() > 2)
                {
                    bool ok = false;
                    double temp = toDouble(cols[2], &ok);
                    if (ok)
                    {
                        values[ModelSchnellgaerverlauf::ColTemp] = temp;
                    }
                }
                if (cols.size() > 3)
                    values[ModelSchnellgaerverlauf::ColBemerkung] = cols[3];
                if (cols.size() > 6)
                    values[ModelSchnellgaerverlauf::ColBemerkung] = cols[6];
                mSud->bh()->modelSchnellgaerverlauf()->appendDirect(values);
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
    mSud->bh()->modelSchnellgaerverlauf()->blockSignals(wasBlocked);
    mSud->bh()->modelSchnellgaerverlauf()->emitModified();
    mSud->modelSchnellgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Restextrakt1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Restextrakt2;Temperatur2[;Bemerkung2]"));
    }
}

void TabGaerverlauf1::pasteFromClipboardHauptgaerverlauf(const QString& str)
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
    bool wasBlocked = mSud->bh()->modelHauptgaerverlauf()->blockSignals(true);
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
                QMap<int, QVariant> values = {{ModelHauptgaerverlauf::ColSudID, mSud->id()},
                                              {ModelHauptgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = toDouble(cols[1], &ok);
                    if (ok)
                    {
                        values[ModelHauptgaerverlauf::ColRestextrakt] = sre;
                        values[ModelHauptgaerverlauf::ColAlc] = BierCalc::alkohol(mSud->getSWIst(), sre);
                    }
                }
                if (cols.size() > 2)
                {
                    bool ok = false;
                    double temp = toDouble(cols[2], &ok);
                    if (ok)
                    {
                        values[ModelHauptgaerverlauf::ColTemp] = temp;
                    }
                }
                if (cols.size() > 3)
                    values[ModelHauptgaerverlauf::ColBemerkung] = cols[3];
                if (cols.size() > 6)
                    values[ModelHauptgaerverlauf::ColBemerkung] = cols[6];
                mSud->bh()->modelHauptgaerverlauf()->appendDirect(values);
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
    mSud->bh()->modelHauptgaerverlauf()->blockSignals(wasBlocked);
    mSud->bh()->modelHauptgaerverlauf()->emitModified();
    mSud->modelHauptgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Restextrakt1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Restextrakt2;Temperatur2[;Bemerkung2]"));
    }
}
