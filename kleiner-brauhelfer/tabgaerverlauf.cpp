#include "tabgaerverlauf.h"
#include "ui_tabgaerverlauf.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QClipboard>
#include "brauhelfer.h"
#include "settings.h"
#include "model/datetimedelegate.h"
#include "model/doublespinboxdelegate.h"
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
    ui->widget_DiaNachgaerverlauf->L2Precision = 1;
    ui->widget_DiaNachgaerverlauf->BezeichnungL3 = tr("Temperatur [°C]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL3 = tr("°C");
    ui->widget_DiaNachgaerverlauf->L3Precision = 1;

    int col;
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    QTableView *table = ui->tableWidget_Schnellgaerverlauf;
    QHeaderView *header = table->horizontalHeader();
    table->setModel(model);
    for (int i = 0; i < model->columnCount(); ++i)
        table->setColumnHidden(i, true);

    col = ModelSchnellgaerverlauf::ColZeitstempel;
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelSchnellgaerverlauf::ColRestextrakt;
    model->setHeaderData(col, Qt::Horizontal, tr("SRE [°P]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelSchnellgaerverlauf::ColTemp;
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelSchnellgaerverlauf::ColAlc;
    model->setHeaderData(col, Qt::Horizontal, tr("Alk. [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelSchnellgaerverlauf::ColsEVG;
    model->setHeaderData(col, Qt::Horizontal, tr("sEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelSchnellgaerverlauf::ColtEVG;
    model->setHeaderData(col, Qt::Horizontal, tr("tEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelSchnellgaerverlauf::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->setStretchLastSection(true);
    header->moveSection(header->visualIndex(col), 6);

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaSchnellgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    model = bh->sud()->modelHauptgaerverlauf();
    table = ui->tableWidget_Hauptgaerverlauf;
    header = table->horizontalHeader();
    table->setModel(model);
    for (int i = 0; i < model->columnCount(); ++i)
        table->setColumnHidden(i, true);

    col = ModelHauptgaerverlauf::ColZeitstempel;
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelHauptgaerverlauf::ColRestextrakt;
    model->setHeaderData(col, Qt::Horizontal, tr("SRE [°P]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelHauptgaerverlauf::ColTemp;
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelHauptgaerverlauf::ColAlc;
    model->setHeaderData(col, Qt::Horizontal, tr("Alk. [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelHauptgaerverlauf::ColsEVG;
    model->setHeaderData(col, Qt::Horizontal, tr("sEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelHauptgaerverlauf::ColtEVG;
    model->setHeaderData(col, Qt::Horizontal, tr("tEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelSchnellgaerverlauf::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->setStretchLastSection(true);
    header->moveSection(header->visualIndex(col), 6);

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaHauptgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    model = bh->sud()->modelNachgaerverlauf();
    table = ui->tableWidget_Nachgaerverlauf;
    header = table->horizontalHeader();
    table->setModel(model);
    for (int i = 0; i < model->columnCount(); ++i)
        table->setColumnHidden(i, true);

    col = ModelNachgaerverlauf::ColZeitstempel;
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelNachgaerverlauf::ColDruck;
    model->setHeaderData(col, Qt::Horizontal, tr("Druck [bar]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 10.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelNachgaerverlauf::ColTemp;
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelNachgaerverlauf::ColCO2;
    model->setHeaderData(col, Qt::Horizontal, tr("CO2 [g/l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelSchnellgaerverlauf::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->setStretchLastSection(true);
    header->moveSection(header->visualIndex(col), 4);

    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->widget_DiaNachgaerverlauf, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    gSettings->beginGroup("TabGaerverlauf");

    mDefaultSplitterStateSchnellgaerung = ui->splitterSchnellgaerung->saveState();
    ui->splitterSchnellgaerung->restoreState(gSettings->value("splitterStateSchnellgaerung").toByteArray());

    mDefaultSplitterStateHauptgaerung = ui->splitterHautpgaerung->saveState();
    ui->splitterHautpgaerung->restoreState(gSettings->value("splitterStateHautpgaerung").toByteArray());

    mDefaultSplitterStateNachgaerung = ui->splitterNachgaerung->saveState();
    ui->splitterNachgaerung->restoreState(gSettings->value("splitterStateNachgaerung").toByteArray());

    gSettings->endGroup();

    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(onSchnellgaerverlaufRowInserted()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(onSchnellgaerverlaufRowInserted()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(onSchnellgaerverlaufRowInserted()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(modified()), this, SLOT(onSchnellgaerverlaufRowInserted()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(onHauptgaerverlaufRowInserted()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(onHauptgaerverlaufRowInserted()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(onHauptgaerverlaufRowInserted()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(modified()), this, SLOT(onHauptgaerverlaufRowInserted()));
    connect(bh->sud()->modelHefegaben(), SIGNAL(modified()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(onNachgaerverlaufRowInserted()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(onNachgaerverlaufRowInserted()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(onNachgaerverlaufRowInserted()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(modified()), this, SLOT(onNachgaerverlaufRowInserted()));

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
    gSettings->setValue("splitterStateSchnellgaerung", ui->splitterSchnellgaerung->saveState());
    gSettings->setValue("splitterStateHautpgaerung", ui->splitterHautpgaerung->saveState());
    gSettings->setValue("splitterStateNachgaerung", ui->splitterNachgaerung->saveState());
    gSettings->endGroup();
}

void TabGaerverlauf::restoreView()
{
    ui->splitterSchnellgaerung->restoreState(mDefaultSplitterStateSchnellgaerung);
    ui->splitterHautpgaerung->restoreState(mDefaultSplitterStateHauptgaerung);
    ui->splitterNachgaerung->restoreState(mDefaultSplitterStateNachgaerung);
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
                    pasteFromClipboardSchnellgaerverlauf();
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
                    pasteFromClipboardHauptgaerverlauf();
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
                    pasteFromClipboardNachgaerverlauf();
            break;
        }
    }
}

void TabGaerverlauf::sudLoaded()
{
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    ui->tbSWSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColRestextrakt).toDouble());
    ui->tbTempSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColTemp).toDouble());

    model = bh->sud()->modelHauptgaerverlauf();
    ui->tbSWHauptgaerprobe->setValue(model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColRestextrakt).toDouble());
    ui->tbTempHauptgaerprobe->setValue(model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColTemp).toDouble());

    model = bh->sud()->modelNachgaerverlauf();
    ui->tbNachgaerdruck->setValue(model->data(model->rowCount() - 1, ModelNachgaerverlauf::ColDruck).toDouble());
    ui->tbNachgaertemp->setValue(model->data(model->rowCount() - 1, ModelNachgaerverlauf::ColTemp).toDouble());

    ui->tbDatumSchnellgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumHautgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumNachgaerprobe->setDateTime(QDateTime::currentDateTime());

    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
}

void TabGaerverlauf::sudDataChanged(const QModelIndex& index)
{
    if (index.column() == ModelSud::ColStatus)
    {
        updateWeitereZutaten();
    }
}

void TabGaerverlauf::onSchnellgaerverlaufRowInserted()
{
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    ui->tbSWSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColRestextrakt).toDouble());
    ui->tbTempSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, ModelSchnellgaerverlauf::ColTemp).toDouble());
    updateDiagramm();
}

void TabGaerverlauf::onHauptgaerverlaufRowInserted()
{
    ProxyModel *model = bh->sud()->modelHauptgaerverlauf();
    ui->tbSWHauptgaerprobe->setValue(model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColRestextrakt).toDouble());
    ui->tbTempHauptgaerprobe->setValue(model->data(model->rowCount() - 1, ModelHauptgaerverlauf::ColTemp).toDouble());
    updateDiagramm();
}

void TabGaerverlauf::onNachgaerverlaufRowInserted()
{
    ProxyModel *model = bh->sud()->modelNachgaerverlauf();
    ui->tbNachgaerdruck->setValue(model->data(model->rowCount() - 1, ModelNachgaerverlauf::ColDruck).toDouble());
    ui->tbNachgaertemp->setValue(model->data(model->rowCount() - 1, ModelNachgaerverlauf::ColTemp).toDouble());
    updateDiagramm();
}

void TabGaerverlauf::checkEnabled()
{
    bool enabled;
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    enabled = bh->sud()->getStatus() == Sud_Status_Gebraut || gSettings->ForceEnabled;
    ui->wdgEditSchnellgaerung->setVisible(enabled);
    ui->tableWidget_Schnellgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = bh->sud()->getStatus() == Sud_Status_Gebraut  || gSettings->ForceEnabled;
    ui->wdgEditHauptgaerung1->setVisible(enabled);
    ui->wdgEditHauptgaerung2->setVisible(enabled);
    ui->tableWidget_Hauptgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = bh->sud()->getStatus() == Sud_Status_Abgefuellt  || gSettings->ForceEnabled;
    ui->wdgEditNachgaerung->setVisible(enabled);
    ui->tableWidget_Nachgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
}

void TabGaerverlauf::updateValues()
{
    checkEnabled();
    QDateTime dtCurrent = QDateTime::currentDateTime();
    ui->tbDatumSchnellgaerprobe->setMinimumDateTime(bh->sud()->getBraudatum());
    ui->tbDatumSchnellgaerprobe->setMaximumDateTime(dtCurrent);
    ui->tbDatumHautgaerprobe->setMinimumDateTime(bh->sud()->getBraudatum());
    ui->tbDatumHautgaerprobe->setMaximumDateTime(dtCurrent);
    ui->tbDatumNachgaerprobe->setMinimumDateTime(bh->sud()->getAbfuelldatum());
    ui->tbDatumNachgaerprobe->setMaximumDateTime(dtCurrent);
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

void TabGaerverlauf::on_btnSWSchnellgaerverlauf_clicked()
{
    DlgRestextrakt dlg(ui->tbSWSchnellgaerprobe->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTempSchnellgaerprobe->value(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        ui->tbSWSchnellgaerprobe->setValue(dlg.value());
        ui->tbTempSchnellgaerprobe->setValue(dlg.temperature());
    }
}

void TabGaerverlauf::on_btnAddSchnellgaerMessung_clicked()
{
    QMap<int, QVariant> values({{ModelSchnellgaerverlauf::ColSudID, bh->sud()->id()},
                                {ModelSchnellgaerverlauf::ColZeitstempel, ui->tbDatumSchnellgaerprobe->dateTime()},
                                {ModelSchnellgaerverlauf::ColRestextrakt, ui->tbSWSchnellgaerprobe->value()},
                                {ModelSchnellgaerverlauf::ColTemp, ui->tbTempSchnellgaerprobe->value()}});
    bh->sud()->modelSchnellgaerverlauf()->append(values);
}

void TabGaerverlauf::on_btnDelSchnellgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Schnellgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        bh->sud()->modelSchnellgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnSWHauptgaerverlauf_clicked()
{
    DlgRestextrakt dlg(ui->tbSWHauptgaerprobe->value(),
                       bh->sud()->getSWIst(),
                       ui->tbTempHauptgaerprobe->value(),
                       this);
    if (dlg.exec() == QDialog::Accepted)
    {
        ui->tbSWHauptgaerprobe->setValue(dlg.value());
        ui->tbTempHauptgaerprobe->setValue(dlg.temperature());
    }
}

void TabGaerverlauf::on_btnAddHauptgaerMessung_clicked()
{
    QMap<int, QVariant> values({{ModelHauptgaerverlauf::ColSudID, bh->sud()->id()},
                                {ModelHauptgaerverlauf::ColZeitstempel, ui->tbDatumHautgaerprobe->dateTime()},
                                {ModelHauptgaerverlauf::ColRestextrakt, ui->tbSWHauptgaerprobe->value()},
                                {ModelHauptgaerverlauf::ColTemp, ui->tbTempHauptgaerprobe->value()}});
    bh->sud()->modelHauptgaerverlauf()->append(values);
}

void TabGaerverlauf::updateWeitereZutaten()
{
    ui->comboBox_GaerungEwzAuswahl->clear();
    ui->comboBox_GaerungEwzAuswahlEntnahme->clear();
    for (int i = 0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); ++i)
    {
        int zeitpunkt = bh->sud()->modelWeitereZutatenGaben()->index(i, ModelWeitereZutatenGaben::ColZeitpunkt).data().toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung)
        {
            QString name = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColName).toString();
            int id = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColID).toInt();
            int type = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen ? 1 : 3;
            int status = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColZugabestatus).toInt();
            bool entnahme = !bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::ColEntnahmeindex).toBool();
            int menge = bh->sud()->modelWeitereZutatenGaben()->data(i, ModelWeitereZutatenGaben::Colerg_Menge).toInt();
            if (status == EWZ_Zugabestatus_nichtZugegeben)
                ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<int,int>(type, id)));
            else if (status == EWZ_Zugabestatus_Zugegeben && entnahme)
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
            ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", QVariant::fromValue(QPair<int,int>(2, id)));
        }
    }
    ui->widget_EwzZugeben->setVisible(ui->comboBox_GaerungEwzAuswahl->count() > 0);
    ui->widget_EwzEntnehmen->setVisible(ui->comboBox_GaerungEwzAuswahlEntnahme->count() > 0);
}

void TabGaerverlauf::on_btnGaerungEwzZugeben_clicked()
{
    QPair<int,int> data = ui->comboBox_GaerungEwzAuswahl->currentData().value<QPair<int,int>>();
    if (data.first == 2)
    {
        int row = bh->sud()->modelHefegaben()->getRowWithValue(ModelHefe::ColID, data.second);
        if (row >= 0)
        {
            QDate currentDate = QDate::currentDate();
            QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
            bh->sud()->modelHefegaben()->setData(row, ModelHefegaben::ColZugabeDatum, date);
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
            QDate currentDate = QDate::currentDate();
            QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabeDatum, date);
            bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_Zugegeben);

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
        QDate currentDate = QDate::currentDate();
        QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
        bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColEntnahmeDatum, date);
        bh->sud()->modelWeitereZutatenGaben()->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_Entnommen);
    }
}

void TabGaerverlauf::on_btnDelHauptgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Hauptgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        bh->sud()->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnAddNachgaerMessung_clicked()
{
    QMap<int, QVariant> values({{ModelNachgaerverlauf::ColSudID, bh->sud()->id()},
                                {ModelNachgaerverlauf::ColZeitstempel, ui->tbDatumNachgaerprobe->dateTime()},
                                {ModelNachgaerverlauf::ColDruck, ui->tbNachgaerdruck->value()},
                                {ModelNachgaerverlauf::ColTemp, ui->tbNachgaertemp->value()}});
    bh->sud()->modelNachgaerverlauf()->append(values);
}

void TabGaerverlauf::on_btnDelNachgaerMessung_clicked()
{
    QModelIndexList indices = ui->tableWidget_Nachgaerverlauf->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        bh->sud()->modelNachgaerverlauf()->removeRow(index.row());
}

QDateTime TabGaerverlauf::fromString(const QString& string)
{
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
    if (dt.isValid())
    {
        if (dt.date().year() < 2000)
            dt = dt.addYears(100);
    }
    return dt;
}

void TabGaerverlauf::pasteFromClipboardSchnellgaerverlauf()
{
    QString clipboardText = QApplication::clipboard()->text();
    QStringList rows = clipboardText.split("\n", QString::SkipEmptyParts);
    if (rows.size() == 0)
        return;
    bh->modelSchnellgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() > 0)
        {
            QDateTime dt = fromString(cols[0]);
            if (dt.isValid())
            {
                QMap<int, QVariant> values = {{ModelSchnellgaerverlauf::ColSudID, bh->sud()->id()},
                                              {ModelSchnellgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = QLocale().toDouble(cols[1], &ok);
                    if (!ok)
                        sre = cols[1].toDouble(&ok);
                    if (ok)
                    {
                        values[ModelSchnellgaerverlauf::ColRestextrakt] = sre;
                        values[ModelSchnellgaerverlauf::ColAlc] = BierCalc::alkohol(bh->sud()->getSWIst(), sre);
                    }
                }
                if (cols.size() > 2)
                {
                    bool ok = false;
                    double temp = QLocale().toDouble(cols[2], &ok);
                    if (!ok)
                        temp = cols[2].toDouble(&ok);
                    if (ok)
                        values[ModelSchnellgaerverlauf::ColTemp] = temp;
                }
                if (cols.size() > 6)
                    values[ModelSchnellgaerverlauf::ColBemerkung] = cols[6];
                bh->modelSchnellgaerverlauf()->appendDirect(values);
            }
        }
    }
    bh->modelSchnellgaerverlauf()->blockSignals(false);
    bh->modelSchnellgaerverlauf()->emitModified();
    bh->sud()->modelSchnellgaerverlauf()->invalidate();
}

void TabGaerverlauf::pasteFromClipboardHauptgaerverlauf()
{
    QString clipboardText = QApplication::clipboard()->text();
    QStringList rows = clipboardText.split("\n", QString::SkipEmptyParts);
    if (rows.size() == 0)
        return;
    bh->modelHauptgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() > 0)
        {
            QDateTime dt = fromString(cols[0]);
            if (dt.isValid())
            {
                QMap<int, QVariant> values = {{ModelHauptgaerverlauf::ColSudID, bh->sud()->id()},
                                              {ModelHauptgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    double sre = QLocale().toDouble(cols[1], &ok);
                    if (!ok)
                        sre = cols[1].toDouble(&ok);
                    if (ok)
                    {
                        values[ModelHauptgaerverlauf::ColRestextrakt] = sre;
                        values[ModelHauptgaerverlauf::ColAlc] = BierCalc::alkohol(bh->sud()->getSWIst(), sre);
                    }
                }
                if (cols.size() > 2)
                {
                    bool ok = false;
                    double temp = QLocale().toDouble(cols[2], &ok);
                    if (!ok)
                        temp = cols[2].toDouble(&ok);
                    if (ok)
                        values[ModelHauptgaerverlauf::ColTemp] = temp;
                }
                if (cols.size() > 6)
                    values[ModelHauptgaerverlauf::ColBemerkung] = cols[6];
                bh->modelHauptgaerverlauf()->appendDirect(values);
            }
        }
    }
    bh->modelHauptgaerverlauf()->blockSignals(false);
    bh->modelHauptgaerverlauf()->emitModified();
    bh->sud()->modelHauptgaerverlauf()->invalidate();
}

void TabGaerverlauf::pasteFromClipboardNachgaerverlauf()
{
    QString clipboardText = QApplication::clipboard()->text();
    QStringList rows = clipboardText.split("\n", QString::SkipEmptyParts);
    if (rows.size() == 0)
        return;
    bh->modelNachgaerverlauf()->blockSignals(true);
    for (const QString& row : rows)
    {
        QStringList cols = row.split("\t");
        if (cols.size() > 0)
        {
            QDateTime dt = fromString(cols[0]);
            if (dt.isValid())
            {
                double druck = 0;
                QMap<int, QVariant> values = {{ModelNachgaerverlauf::ColSudID, bh->sud()->id()},
                                              {ModelNachgaerverlauf::ColZeitstempel, dt}};
                if (cols.size() > 1)
                {
                    bool ok = false;
                    druck = QLocale().toDouble(cols[1], &ok);
                    if (!ok)
                        druck = cols[1].toDouble(&ok);
                    if (ok)
                    {
                        values[ModelNachgaerverlauf::ColDruck] = druck;
                    }
                }
                if (cols.size() > 2)
                {
                    bool ok = false;
                    double temp = QLocale().toDouble(cols[2], &ok);
                    if (!ok)
                        temp = cols[2].toDouble(&ok);
                    if (ok)
                    {
                        values[ModelNachgaerverlauf::ColTemp] = temp;
                        values[ModelNachgaerverlauf::ColCO2] = BierCalc::co2(druck, temp);
                    }
                }
                if (cols.size() > 4)
                    values[ModelNachgaerverlauf::ColBemerkung] = cols[4];
                bh->modelNachgaerverlauf()->appendDirect(values);
            }
        }
    }
    bh->modelNachgaerverlauf()->blockSignals(false);
    bh->modelNachgaerverlauf()->emitModified();
    bh->sud()->modelNachgaerverlauf()->invalidate();
}
