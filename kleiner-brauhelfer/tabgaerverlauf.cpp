#include "tabgaerverlauf.h"
#include "ui_tabgaerverlauf.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"
#include "model/datetimedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/restextraktdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "widgets/wdgweiterezutatgabe.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabGaerverlauf::TabGaerverlauf(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabGaerverlauf)
{
    ui->setupUi(this);

    ui->widget_DiaSchnellgaerverlauf->BezeichnungL1 = tr("Restextrakt [°P]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL1 = tr("°P");
    ui->widget_DiaSchnellgaerverlauf->L1Precision = 1;
    ui->widget_DiaSchnellgaerverlauf->BezeichnungL2 = tr("Alkohol [%]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL2 = tr("%");
    ui->widget_DiaSchnellgaerverlauf->L2Precision = 1;
    ui->widget_DiaSchnellgaerverlauf->BezeichnungL3 = tr("Temperatur [°C]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL3 = tr("°C");
    ui->widget_DiaSchnellgaerverlauf->L3Precision = 1;

    ui->widget_DiaHauptgaerverlauf->BezeichnungL1 = tr("Restextrakt [°P]");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL1 = tr("°P");
    ui->widget_DiaHauptgaerverlauf->L1Precision = 1;
    ui->widget_DiaHauptgaerverlauf->BezeichnungL2 = tr("Alkohol [%]");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL2 = tr("%");
    ui->widget_DiaHauptgaerverlauf->L2Precision = 1;
    ui->widget_DiaHauptgaerverlauf->BezeichnungL3 = tr("Temperatur °C");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL3 = tr("°C");
    ui->widget_DiaHauptgaerverlauf->L3Precision = 1;

    ui->widget_DiaNachgaerverlauf->BezeichnungL1 = tr("CO₂-Gehalt [g/l]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL1 = tr("g/l");
    ui->widget_DiaNachgaerverlauf->L1Precision = 1;
    ui->widget_DiaNachgaerverlauf->BezeichnungL2 = tr("Druck [bar]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL2 = tr("bar");
    ui->widget_DiaNachgaerverlauf->L2Precision = 2;
    ui->widget_DiaNachgaerverlauf->BezeichnungL3 = tr("Temperatur [°C]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL3 = tr("°C");
    ui->widget_DiaNachgaerverlauf->L3Precision = 1;

    gSettings->beginGroup("TabGaerverlauf");

    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    model->setHeaderData(ModelSchnellgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColRestextrakt, Qt::Horizontal, tr("SRE [°P]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColTemp, Qt::Horizontal, tr("Temp. [°C]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColAlc, Qt::Horizontal, tr("Alk. [%]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG [%]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG [%]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    TableView *table = ui->tableWidget_Schnellgaerverlauf;
    table->setModel(model);
    table->cols.append({ModelSchnellgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(false, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelSchnellgaerverlauf::ColBemerkung, true, true, -1, nullptr});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateSchnellgaerung").toByteArray());

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaSchnellgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    model = bh->sud()->modelHauptgaerverlauf();
    model->setHeaderData(ModelHauptgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelHauptgaerverlauf::ColRestextrakt, Qt::Horizontal, tr("SRE [°P]"));
    model->setHeaderData(ModelHauptgaerverlauf::ColTemp, Qt::Horizontal, tr("Temp. [°C]"));
    model->setHeaderData(ModelHauptgaerverlauf::ColAlc, Qt::Horizontal, tr("Alk. [%]"));
    model->setHeaderData(ModelHauptgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG [%]"));
    model->setHeaderData(ModelHauptgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG [%]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    table = ui->tableWidget_Hauptgaerverlauf;
    table->setModel(model);
    table->cols.append({ModelHauptgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->cols.append({ModelHauptgaerverlauf::ColRestextrakt, true, false, 100, new RestextraktDelegate(true, table)});
    table->cols.append({ModelHauptgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table)});
    table->cols.append({ModelHauptgaerverlauf::ColAlc, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelHauptgaerverlauf::ColsEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelHauptgaerverlauf::ColtEVG, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelHauptgaerverlauf::ColBemerkung, true, true, -1, nullptr});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateHauptgaerung").toByteArray());

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaHauptgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    model = bh->sud()->modelNachgaerverlauf();
    model->setHeaderData(ModelNachgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelNachgaerverlauf::ColDruck, Qt::Horizontal, tr("Druck [bar]"));
    model->setHeaderData(ModelNachgaerverlauf::ColTemp, Qt::Horizontal, tr("Temp. [°C]"));
    model->setHeaderData(ModelNachgaerverlauf::ColCO2, Qt::Horizontal, tr("CO2 [g/l]"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    table = ui->tableWidget_Nachgaerverlauf;

    table->setModel(model);
    table->cols.append({ModelNachgaerverlauf::ColZeitstempel, true, false, 150, new DateTimeDelegate(false, false, table)});
    table->cols.append({ModelNachgaerverlauf::ColDruck, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, 10.0, 0.1, false, table)});
    table->cols.append({ModelNachgaerverlauf::ColTemp, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table)});
    table->cols.append({ModelNachgaerverlauf::ColCO2, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelNachgaerverlauf::ColBemerkung, true, true, -1, nullptr});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateNachgaerung").toByteArray());

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaNachgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    mDefaultSplitterStateSchnellgaerung = ui->splitterSchnellgaerung->saveState();
    ui->splitterSchnellgaerung->restoreState(gSettings->value("splitterStateSchnellgaerung").toByteArray());

    mDefaultSplitterStateHauptgaerung = ui->splitterHauptgaerung->saveState();
    ui->splitterHauptgaerung->restoreState(gSettings->value("splitterStateHauptgaerung").toByteArray());

    mDefaultSplitterStateNachgaerung = ui->splitterNachgaerung->saveState();
    ui->splitterNachgaerung->restoreState(gSettings->value("splitterStateNachgaerung").toByteArray());

    gSettings->endGroup();

    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(layoutChanged()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(layoutChanged()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateWeitereZutaten()));

    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
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

void TabGaerverlauf::restoreView(bool full)
{
    ui->tableWidget_Schnellgaerverlauf->restoreDefaultState();
    ui->tableWidget_Hauptgaerverlauf->restoreDefaultState();
    ui->tableWidget_Nachgaerverlauf->restoreDefaultState();
    if (full)
    {
        ui->splitterSchnellgaerung->restoreState(mDefaultSplitterStateSchnellgaerung);
        ui->splitterHauptgaerung->restoreState(mDefaultSplitterStateHauptgaerung);
        ui->splitterNachgaerung->restoreState(mDefaultSplitterStateNachgaerung);
    }
}

void TabGaerverlauf::keyPressEvent(QKeyEvent* event)
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
            switch (ui->toolBox_Gaerverlauf->currentIndex())
            {
            case 0:
                pasteFromClipboardSchnellgaerverlauf(file.readAll());
                break;
            case 1:
                pasteFromClipboardHauptgaerverlauf(file.readAll());
                break;
            case 2:
                pasteFromClipboardNachgaerverlauf(file.readAll());
                break;
            }
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
        ui->toolBox_Gaerverlauf->setCurrentIndex(2);
        break;
    default:
        ui->toolBox_Gaerverlauf->setCurrentIndex(1);
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

void TabGaerverlauf::updateDiagramm()
{
    WdgDiagramView *diag = ui->widget_DiaSchnellgaerverlauf;
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelSchnellgaerverlauf::ColZeitstempel).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L3Datum.append(dt);
        diag->L1Daten.append(model->index(row, ModelSchnellgaerverlauf::ColRestextrakt).data().toDouble());
        diag->L2Daten.append(model->index(row, ModelSchnellgaerverlauf::ColAlc).data().toDouble());
        diag->L3Daten.append(model->index(row, ModelSchnellgaerverlauf::ColTemp).data().toDouble());
    }
    diag->setWertLinie1(bh->sud()->getSRE());
    diag->repaint();

    diag = ui->widget_DiaHauptgaerverlauf;
    model = bh->sud()->modelHauptgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelHauptgaerverlauf::ColZeitstempel).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L3Datum.append(dt);
        diag->L1Daten.append(model->index(row, ModelHauptgaerverlauf::ColRestextrakt).data().toDouble());
        diag->L2Daten.append(model->index(row, ModelHauptgaerverlauf::ColAlc).data().toDouble());
        diag->L3Daten.append(model->index(row, ModelHauptgaerverlauf::ColTemp).data().toDouble());
    }
    if (bh->sud()->getSchnellgaerprobeAktiv())
        diag->setWertLinie1(bh->sud()->getGruenschlauchzeitpunkt());
    else
        diag->setWertLinie1(bh->sud()->getSRE());
    diag->repaint();

    diag = ui->widget_DiaNachgaerverlauf;
    model = bh->sud()->modelNachgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, ModelNachgaerverlauf::ColZeitstempel).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L3Datum.append(dt);
        diag->L1Daten.append(model->index(row, ModelNachgaerverlauf::ColCO2).data().toDouble());
        diag->L2Daten.append(model->index(row, ModelNachgaerverlauf::ColDruck).data().toDouble());
        diag->L3Daten.append(model->index(row, ModelNachgaerverlauf::ColTemp).data().toDouble());
    }
    diag->setWertLinie1(bh->sud()->getCO2());
    diag->repaint();
}

void TabGaerverlauf::table_selectionChanged(const QItemSelection &selected)
{
    WdgDiagramView *diag;
    switch (ui->toolBox_Gaerverlauf->currentIndex())
    {
    case 0:
        diag = ui->widget_DiaSchnellgaerverlauf;
        break;
    case 1:
        diag = ui->widget_DiaHauptgaerverlauf;
        break;
    case 2:
        diag = ui->widget_DiaNachgaerverlauf;
        break;
    default:
        return;
    }
    int id = -1;
    if (selected.indexes().count() > 0)
        id = selected.indexes()[0].row();
    diag->MarkierePunkt(id);
}

void TabGaerverlauf::diagram_selectionChanged(int id)
{
    QTableView *table;
    switch (ui->toolBox_Gaerverlauf->currentIndex())
    {
    case 0:
        table = ui->tableWidget_Schnellgaerverlauf;
        break;
    case 1:
        table = ui->tableWidget_Hauptgaerverlauf;
        break;
    case 2:
        table = ui->tableWidget_Nachgaerverlauf;
        break;
    default:
        return;
    }
    table->selectRow(id);
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
    for (const QModelIndex& index : indices)
        bh->sud()->modelSchnellgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportSchnellgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    "",
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
            DlgRohstoffeAbziehen dlg(data.first,
                                     bh->sud()->modelHefegaben()->data(row, ModelHefegaben::ColName).toString(),
                                     bh->sud()->modelHefegaben()->data(row, ModelHefegaben::ColMenge).toDouble(),
                                     this);
            dlg.exec();
        }
    }
    else
    {
        int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue(ModelWeitereZutatenGaben::ColID, data.second);
        if (row >= 0)
        {
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabeDatum, QDate::currentDate());
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));
            DlgRohstoffeAbziehen dlg(data.first,
                                     bh->sud()->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::ColName).toString(),
                                     bh->sud()->modelWeitereZutatenGaben()->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                                     this);
            dlg.exec();
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
    for (const QModelIndex& index : indices)
        bh->sud()->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportHauptgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    "",
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
    for (const QModelIndex& index : indices)
        bh->sud()->modelNachgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnImportNachgaerMessung_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("CSV Datei auswählen"),
                                                    "",
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
    QDateTime dt = QDateTime::fromString(string, Qt::SystemLocaleShortDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::DefaultLocaleShortDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::TextDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::ISODate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, Qt::SystemLocaleLongDate);
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "d.M.yy h:m:s");
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "d.M.yy h:m");
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "d.M.yyyy h:m:s");
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "d.M.yyyy h:m");
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "M/d/yyyy h:m:s");
    if (!dt.isValid())
        dt = QDateTime::fromString(string, "M/d/yyyy h:m");
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
    QStringList rows = str.split("\n", Qt::SkipEmptyParts);
  #else
    QStringList rows = str.split("\n", QString::SkipEmptyParts);
  #endif
    if (rows.size() == 0)
        rows.append(str);
    bool error = false;
    bool firstRow = true;
    bool wasBlocked = bh->modelSchnellgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() == 1)
            cols = row.split(";");
        if (cols.size() == 1)
            cols = row.split(",");
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
    QStringList rows = str.split("\n", Qt::SkipEmptyParts);
  #else
    QStringList rows = str.split("\n", QString::SkipEmptyParts);
  #endif
    if (rows.size() == 0)
        rows.append(str);
    bool error = false;
    bool firstRow = true;
    bool wasBlocked = bh->modelHauptgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() == 1)
            cols = row.split(";");
        if (cols.size() == 1)
            cols = row.split(",");
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
    QStringList rows = str.split("\n", Qt::SkipEmptyParts);
  #else
    QStringList rows = str.split("\n", QString::SkipEmptyParts);
  #endif
    if (rows.size() == 0)
        rows.append(str);
    bool error = false;
    bool firstRow = true;
    bool wasBlocked = bh->modelNachgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() == 1)
            cols = row.split(";");
        if (cols.size() == 1)
            cols = row.split(",");
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
