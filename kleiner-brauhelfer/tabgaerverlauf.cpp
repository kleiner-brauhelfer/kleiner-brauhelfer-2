#include "tabgaerverlauf.h"
#include "ui_tabgaerverlauf.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
#include "model/datetimedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/restextraktdelegate.h"
#include "model/textdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgrohstoffeabziehen.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

TabGaerverlauf::TabGaerverlauf(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabGaerverlauf)
{
    ui->setupUi(this);

    gSettings->beginGroup("TabGaerverlauf");

    TableView *table = ui->tableWidget_Schnellgaerverlauf;
    table->setModel(bh->sud()->modelSchnellgaerverlauf());
    table->appendCol({ModelSchnellgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSchnellgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateSchnellgaerung").toByteArray());
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabGaerverlauf::table_selectionChanged);
    connect(ui->widget_DiaSchnellgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf::diagram_selectionChanged);

    table = ui->tableWidget_Hauptgaerverlauf;
    table->setModel(bh->sud()->modelHauptgaerverlauf());
    table->appendCol({ModelHauptgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelHauptgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(true, table)});
    table->appendCol({ModelHauptgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelHauptgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelHauptgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateHauptgaerung").toByteArray());
    connect(table->selectionModel(),  &QItemSelectionModel::selectionChanged, this,  &TabGaerverlauf::table_selectionChanged);
    connect(ui->widget_DiaHauptgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf::diagram_selectionChanged);

    table = ui->tableWidget_Nachgaerverlauf;
    table->setModel(bh->sud()->modelNachgaerverlauf());
    table->appendCol({ModelNachgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColDruck, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, 10.0, 0.1, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, -20.0, 100.0, 0.1, false, table)});
    table->appendCol({ModelNachgaerverlauf::ColCO2, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelNachgaerverlauf::ColBemerkung, true, true, -1, new TextDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateNachgaerung").toByteArray());
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabGaerverlauf::table_selectionChanged);
    connect(ui->widget_DiaNachgaerverlauf, &Chart3::selectionChanged, this, &TabGaerverlauf::diagram_selectionChanged);

    mDefaultSplitterStateSchnellgaerung = ui->splitterSchnellgaerung->saveState();
    ui->splitterSchnellgaerung->restoreState(gSettings->value("splitterStateSchnellgaerung").toByteArray());

    mDefaultSplitterStateHauptgaerung = ui->splitterHauptgaerung->saveState();
    ui->splitterHauptgaerung->restoreState(gSettings->value("splitterStateHauptgaerung").toByteArray());

    mDefaultSplitterStateNachgaerung = ui->splitterNachgaerung->saveState();
    ui->splitterNachgaerung->restoreState(gSettings->value("splitterStateNachgaerung").toByteArray());

    gSettings->endGroup();

    connect(bh, &Brauhelfer::discarded, this, &TabGaerverlauf::sudLoaded);
    connect(bh->sud(), &SudObject::loadedChanged, this, &TabGaerverlauf::sudLoaded);
    connect(bh->sud(), &SudObject::modified, this, &TabGaerverlauf::updateValues);
    connect(bh->sud(), &SudObject::dataChanged, this, &TabGaerverlauf::sudDataChanged);
    connect(bh->sud()->modelSchnellgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelSchnellgaerverlauf(), &ProxyModel::rowsInserted, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelSchnellgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelSchnellgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf::dataChangedSchnellgaerverlauf);
    connect(bh->sud()->modelHauptgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelHauptgaerverlauf(), &ProxyModel::rowsInserted, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelHauptgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelHauptgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf::dataChangedHauptgaerverlauf);
    connect(bh->sud()->modelNachgaerverlauf(), &ProxyModel::layoutChanged, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelNachgaerverlauf(), &ProxyModel::rowsInserted,this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelNachgaerverlauf(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf::updateDiagramm);
    connect(bh->sud()->modelNachgaerverlauf(), &ProxyModel::dataChanged, this, &TabGaerverlauf::dataChangedNachgaerverlauf);
    connect(bh->sud()->modelHefegaben(), &ProxyModel::layoutChanged, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelHefegaben(), &ProxyModel::rowsInserted, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelHefegaben(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelHefegaben(), &ProxyModel::dataChanged, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelWeitereZutatenGaben(), &ProxyModel::layoutChanged, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelWeitereZutatenGaben(), &ProxyModel::rowsInserted, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelWeitereZutatenGaben(), &ProxyModel::rowsRemoved, this, &TabGaerverlauf::updateWeitereZutaten);
    connect(bh->sud()->modelWeitereZutatenGaben(), &ProxyModel::dataChanged, this, &TabGaerverlauf::updateWeitereZutaten);
}

TabGaerverlauf::~TabGaerverlauf()
{
    delete ui;
}

void TabGaerverlauf::onTabActivated()
{
    updateDiagramm();
}

void TabGaerverlauf::saveSettings()
{
    gSettings->beginGroup("TabGaerverlauf");
    gSettings->setValue("tableStateSchnellgaerung", ui->tableWidget_Schnellgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("tableStateHauptgaerung", ui->tableWidget_Hauptgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("tableStateNachgaerung", ui->tableWidget_Nachgaerverlauf->horizontalHeader()->saveState());
    gSettings->setValue("splitterStateSchnellgaerung", ui->splitterSchnellgaerung->saveState());
    gSettings->setValue("splitterStateHauptgaerung", ui->splitterHauptgaerung->saveState());
    gSettings->setValue("splitterStateNachgaerung", ui->splitterNachgaerung->saveState());
    gSettings->endGroup();
}

void TabGaerverlauf::restoreView()
{
    ui->tableWidget_Schnellgaerverlauf->restoreDefaultState();
    ui->tableWidget_Hauptgaerverlauf->restoreDefaultState();
    ui->tableWidget_Nachgaerverlauf->restoreDefaultState();
    ui->splitterSchnellgaerung->restoreState(mDefaultSplitterStateSchnellgaerung);
    ui->splitterHauptgaerung->restoreState(mDefaultSplitterStateHauptgaerung);
    ui->splitterNachgaerung->restoreState(mDefaultSplitterStateNachgaerung);
}

void TabGaerverlauf::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleSchnellgaerprobe))
    {
        if (gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe))
        {
            if (ui->toolBox_Gaerverlauf->count() == 2)
            {
                ui->toolBox_Gaerverlauf->insertItem(0, ui->pageSchnellgaerung, tr("Schnellgärprobe"));
                ui->pageSchnellgaerung->setVisible(true);
            }
        }
        else
        {
            if (ui->toolBox_Gaerverlauf->count() == 3)
            {
                ui->toolBox_Gaerverlauf->removeItem(0);
                ui->pageSchnellgaerung->setVisible(false);
            }
        }
    }
    if (bh->sud()->isLoaded())
    {
        updateValues();
    }
}

void TabGaerverlauf::keyPressEvent(QKeyEvent* event)
{
    TabAbstract::keyPressEvent(event);
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
    else if (ui->tableWidget_Nachgaerverlauf->hasFocus())
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

void TabGaerverlauf::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TabGaerverlauf::dropEvent(QDropEvent *event)
{
    for (const QUrl& url : event->mimeData()->urls())
    {
        QFile file(url.toLocalFile());
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            const QWidget* currentWidget = ui->toolBox_Gaerverlauf->currentWidget();
            if (currentWidget == ui->pageSchnellgaerung)
                pasteFromClipboardSchnellgaerverlauf(file.readAll());
            else if (currentWidget == ui->pageHauptgaerung)
                pasteFromClipboardHauptgaerverlauf(file.readAll());
            else if (currentWidget == ui->pageNachgaerung)
                pasteFromClipboardNachgaerverlauf(file.readAll());
        }
    }
}

void TabGaerverlauf::sudLoaded()
{
    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    switch (status)
    {
    case Brauhelfer::SudStatus::Abgefuellt:
        ui->toolBox_Gaerverlauf->setCurrentWidget(ui->pageNachgaerung);
        break;
    default:
        ui->toolBox_Gaerverlauf->setCurrentWidget(ui->pageHauptgaerung);
        break;
    }
}

void TabGaerverlauf::sudDataChanged(const QModelIndex& index)
{
    if (index.column() == ModelSud::ColStatus)
    {
        updateWeitereZutaten();
    }
}

void TabGaerverlauf::checkEnabled()
{
    bool enabled;
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    enabled = status == Brauhelfer::SudStatus::Gebraut || gSettings->ForceEnabled;
    ui->wdgEditSchnellgaerung->setVisible(enabled);
    ui->tableWidget_Schnellgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = status == Brauhelfer::SudStatus::Gebraut  || gSettings->ForceEnabled;
    ui->wdgEditHauptgaerung1->setVisible(enabled);
    ui->tableWidget_Hauptgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = status == Brauhelfer::SudStatus::Abgefuellt  || gSettings->ForceEnabled;
    ui->wdgEditNachgaerung->setVisible(enabled);
    ui->tableWidget_Nachgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
}

void TabGaerverlauf::updateValues()
{
    checkEnabled();
}

void TabGaerverlauf::dataChangedSchnellgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelSchnellgaerverlauf::ColZeitstempel)
        bh->sud()->modelSchnellgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf::dataChangedHauptgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelHauptgaerverlauf::ColZeitstempel)
        bh->sud()->modelHauptgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf::dataChangedNachgaerverlauf(const QModelIndex& index)
{
    if (index.column() == ModelNachgaerverlauf::ColZeitstempel)
        bh->sud()->modelNachgaerverlauf()->invalidate();
    updateDiagramm();
}

void TabGaerverlauf::updateDiagramm()
{
    Chart3 *diag;
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
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
    diag->setData1Limit(bh->sud()->getSREErwartet());
    diag->rescale();
    diag->replot();

    model = bh->sud()->modelHauptgaerverlauf();
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
    if (bh->sud()->getSchnellgaerprobeAktiv())
        diag->setData1Limit(bh->sud()->getGruenschlauchzeitpunkt());
    else
        diag->setData1Limit(bh->sud()->getSREErwartet());
    diag->rescale();
    diag->replot();

    model = bh->sud()->modelNachgaerverlauf();
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
    diag = ui->widget_DiaNachgaerverlauf;
    diag->clear();
    diag->setData1(x, y1, tr("CO₂"), QStringLiteral("g/L"), 1);
    diag->setData2(x, y2, tr("Druck"), QStringLiteral("bar"), 2);
    diag->setData3(x, y3, tr("Temperatur"), QStringLiteral("°C"), 1);
    diag->setData1Limit(bh->sud()->getCO2());
    diag->rescale();
    diag->replot();
}

void TabGaerverlauf::table_selectionChanged(const QItemSelection &selected)
{
    Chart3 *diag;
    const QWidget* currentWidget = ui->toolBox_Gaerverlauf->currentWidget();
    if (currentWidget == ui->pageSchnellgaerung)
        diag = ui->widget_DiaSchnellgaerverlauf;
    else if (currentWidget == ui->pageHauptgaerung)
        diag = ui->widget_DiaHauptgaerverlauf;
    else if (currentWidget == ui->pageNachgaerung)
        diag = ui->widget_DiaNachgaerverlauf;
    else
        return;
    int index = -1;
    if (selected.indexes().count() > 0)
        index = selected.indexes()[0].row();
    diag->select(index);
}

void TabGaerverlauf::diagram_selectionChanged(int index)
{
    QTableView *table;
    const QWidget* currentWidget = ui->toolBox_Gaerverlauf->currentWidget();
    if (currentWidget == ui->pageSchnellgaerung)
        table = ui->tableWidget_Schnellgaerverlauf;
    else if (currentWidget == ui->pageHauptgaerung)
        table = ui->tableWidget_Hauptgaerverlauf;
    else if (currentWidget == ui->pageNachgaerung)
        table = ui->tableWidget_Nachgaerverlauf;
    else
        return;
    table->selectRow(index);
}

void TabGaerverlauf::on_btnAddSchnellgaerMessung_clicked()
{
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    double re = model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColRestextrakt).toDouble();
    double temp = model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColTemp).toDouble();
    DlgRestextrakt dlg(re, bh->sud()->getSWIst(), temp, QDateTime::currentDateTime(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelSchnellgaerverlauf::ColSudID, bh->sud()->id()},
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

void TabGaerverlauf::on_btnDelSchnellgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Schnellgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
        bh->sud()->modelSchnellgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportSchnellgaerMessung_clicked()
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

void TabGaerverlauf::on_btnAddHauptgaerMessung_clicked()
{
    ProxyModel *model = bh->sud()->modelHauptgaerverlauf();
    double re = model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColRestextrakt).toDouble();
    double temp = model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColTemp).toDouble();
    DlgRestextrakt dlg(re, bh->sud()->getSWIst(), temp, QDateTime::currentDateTime(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QMap<int, QVariant> values({{ModelHauptgaerverlauf::ColSudID, bh->sud()->id()},
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

void TabGaerverlauf::updateWeitereZutaten()
{
    ui->comboBox_GaerungEwzAuswahl->clear();
    ui->comboBox_GaerungEwzAuswahlEntnahme->clear();
    for (int i = 0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); ++i)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(bh->sud()->modelWeitereZutatenGaben()->index(i, ModelWeitereZutatenGaben::ColZeitpunkt).data().toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
        {
            QString name = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColName).toString();
            int id = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColID).toInt();
            Brauhelfer::ZusatzTyp zusatztyp = static_cast<Brauhelfer::ZusatzTyp>(bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColTyp).toInt());
            Brauhelfer::RohstoffTyp typ = zusatztyp == Brauhelfer::ZusatzTyp::Hopfen ? Brauhelfer::RohstoffTyp::Hopfen : Brauhelfer::RohstoffTyp::Zusatz;
            Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColZugabestatus).toInt());
            bool entnahme = !bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColEntnahmeindex).toBool();
            int menge = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toInt();
            if (status == Brauhelfer::ZusatzStatus::NichtZugegeben)
                ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<Brauhelfer::RohstoffTyp,int>(typ, id)));
            else if (status == Brauhelfer::ZusatzStatus::Zugegeben && entnahme)
                ui->comboBox_GaerungEwzAuswahlEntnahme->addItem(name, id);
        }
    }
    for (int i = 0; i < bh->sud()->modelHefegaben()->rowCount(); ++i)
    {
        if (!bh->sud()->modelHefegaben()->data(i, ModelHefegaben::ColZugegeben).toBool())
        {

            QString name = bh->sud()->modelHefegaben()->data(i, ModelHefegaben::ColName).toString();
            int id = bh->sud()->modelHefegaben()->data(i, ModelHefegaben::ColID).toInt();
            int menge = bh->sud()->modelHefegaben()->data(i, ModelHefegaben::ColMenge).toInt();
            ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<Brauhelfer::RohstoffTyp,int>(Brauhelfer::RohstoffTyp::Hefe, id)));
        }
    }
    ui->widget_EwzZugeben->setVisible(ui->comboBox_GaerungEwzAuswahl->count() > 0);
    ui->widget_EwzEntnehmen->setVisible(ui->comboBox_GaerungEwzAuswahlEntnahme->count() > 0);
}

void TabGaerverlauf::on_btnGaerungEwzZugeben_clicked()
{
    QPair<Brauhelfer::RohstoffTyp,int> data = ui->comboBox_GaerungEwzAuswahl->currentData().value<QPair<Brauhelfer::RohstoffTyp,int>>();
    if (data.first == Brauhelfer::RohstoffTyp::Hefe)
    {
        int row = bh->sud()->modelHefegaben()->getRowWithValue(ModelHefe::ColID, data.second);
        if (row >= 0)
        {
            bh->sud()->modelHefegaben()->setData(row, ModelHefegaben::ColZugabeDatum, QDate::currentDate());
            bh->sud()->modelHefegaben()->setData(row, ModelHefegaben::ColZugegeben, true);
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(true, data.first,
                                         bh->sud()->modelHefegaben()->data(row, ModelHefegaben::ColName).toString(),
                                         bh->sud()->modelHefegaben()->data(row, ModelHefegaben::ColMenge).toDouble(),
                                         this);
                dlg.exec();
            }
        }
    }
    else
    {
        int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue(ModelWeitereZutatenGaben::ColID, data.second);
        if (row >= 0)
        {
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabeDatum, QDate::currentDate());
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(true, data.first,
                                         bh->sud()->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::ColName).toString(),
                                         bh->sud()->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                                         this);
                dlg.exec();
            }
        }
    }
}

void TabGaerverlauf::on_btnGaerungEwzEntnehmen_clicked()
{
    int id = ui->comboBox_GaerungEwzAuswahlEntnahme->currentData().toInt();
    int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue(ModelWeitereZutatenGaben::ColID, id);
    if (row >= 0)
    {
        bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColEntnahmeDatum, QDate::currentDate());
        bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Entnommen));
    }
}

void TabGaerverlauf::on_btnDelHauptgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Hauptgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
        bh->sud()->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportHauptgaerMessung_clicked()
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

void TabGaerverlauf::on_btnAddNachgaerMessung_clicked()
{
    QMap<int, QVariant> values({{ModelNachgaerverlauf::ColSudID, bh->sud()->id()}});
    int row = bh->sud()->modelNachgaerverlauf()->append(values);
    if (row >= 0)
    {
        ui->tableWidget_Nachgaerverlauf->setCurrentIndex(bh->sud()->modelNachgaerverlauf()->index(row, ModelNachgaerverlauf::ColDruck));
        ui->tableWidget_Nachgaerverlauf->scrollTo(ui->tableWidget_Nachgaerverlauf->currentIndex());
        ui->tableWidget_Nachgaerverlauf->edit(ui->tableWidget_Nachgaerverlauf->currentIndex());
    }
}

void TabGaerverlauf::on_btnDelNachgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Nachgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
        bh->sud()->modelNachgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportNachgaerMessung_clicked()
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

QDateTime TabGaerverlauf::toDateTime(QString string) const
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

double TabGaerverlauf::toDouble(QString string, bool *ok) const
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

void TabGaerverlauf::pasteFromClipboardSchnellgaerverlauf(const QString &str)
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
    bool wasBlocked = bh->modelSchnellgaerverlauf()->blockSignals(true);
    for (const QString& row : qAsConst(rows))
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
                QMap<int, QVariant> values = {{ModelSchnellgaerverlauf::ColSudID, bh->sud()->id()},
                                              {ModelSchnellgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = toDouble(cols[1], &ok);
                    if (ok)
                    {
                        values[ModelSchnellgaerverlauf::ColRestextrakt] = sre;
                        values[ModelSchnellgaerverlauf::ColAlc] = BierCalc::alkohol(bh->sud()->getSWIst(), sre);
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
                bh->modelSchnellgaerverlauf()->appendDirect(values);
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
    bh->modelSchnellgaerverlauf()->blockSignals(wasBlocked);
    bh->modelSchnellgaerverlauf()->emitModified();
    bh->sud()->modelSchnellgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Restextrakt1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Restextrakt2;Temperatur2[;Bemerkung2]"));
    }
}

void TabGaerverlauf::pasteFromClipboardHauptgaerverlauf(const QString& str)
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
    bool wasBlocked = bh->modelHauptgaerverlauf()->blockSignals(true);
    for (const QString& row : qAsConst(rows))
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
                QMap<int, QVariant> values = {{ModelHauptgaerverlauf::ColSudID, bh->sud()->id()},
                                              {ModelHauptgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = toDouble(cols[1], &ok);
                    if (ok)
                    {
                        values[ModelHauptgaerverlauf::ColRestextrakt] = sre;
                        values[ModelHauptgaerverlauf::ColAlc] = BierCalc::alkohol(bh->sud()->getSWIst(), sre);
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
                bh->modelHauptgaerverlauf()->appendDirect(values);
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
    bh->modelHauptgaerverlauf()->blockSignals(wasBlocked);
    bh->modelHauptgaerverlauf()->emitModified();
    bh->sud()->modelHauptgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Restextrakt1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Restextrakt2;Temperatur2[;Bemerkung2]"));
    }
}

void TabGaerverlauf::pasteFromClipboardNachgaerverlauf(const QString& str)
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
    bool wasBlocked = bh->modelNachgaerverlauf()->blockSignals(true);
    for (const QString& row : qAsConst(rows))
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
                QMap<int, QVariant> values = {{ModelNachgaerverlauf::ColSudID, bh->sud()->id()},
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
                bh->modelNachgaerverlauf()->appendDirect(values);
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
    bh->modelNachgaerverlauf()->blockSignals(wasBlocked);
    bh->modelNachgaerverlauf()->emitModified();
    bh->sud()->modelNachgaerverlauf()->invalidate();
    if (error)
    {
        QMessageBox::warning(this, tr("Gärverlauf"),
                             tr("Nicht alle Einträge konnten importiert werden.\n"
                                "Unterstütztes Format:\n"
                                "Datum1;Druck1;Temperatur1[;Bemerkung1]\n"
                                "Datum2;Druck2;Temperatur2[;Bemerkung2]"));
    }
}
