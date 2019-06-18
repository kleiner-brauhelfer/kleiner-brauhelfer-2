#include "tabgaerverlauf.h"
#include "ui_tabgaerverlauf.h"
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "model/datetimedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "widgets/wdgweiterezutatgabe.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabGaerverlauf::TabGaerverlauf(QWidget *parent) :
    QWidget(parent),
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
    ui->widget_DiaNachgaerverlauf->BezeichnungL2 = tr("Temperatur [°C]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL2 = tr("°C");
    ui->widget_DiaNachgaerverlauf->L2Precision = 1;

    int col;
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    QTableView *table = ui->tableWidget_Schnellgaerverlauf;
    QHeaderView *header = table->horizontalHeader();
    table->setModel(model);
    for (int i = 0; i < model->columnCount(); ++i)
        table->setColumnHidden(i, true);

    col = model->fieldIndex("Zeitstempel");
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("SW");
    model->setHeaderData(col, Qt::Horizontal, tr("SRE [°P]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Temp");
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Alc");
    model->setHeaderData(col, Qt::Horizontal, tr("Alk. [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("tEVG");
    model->setHeaderData(col, Qt::Horizontal, tr("tEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

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

    col = model->fieldIndex("Zeitstempel");
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("SW");
    model->setHeaderData(col, Qt::Horizontal, tr("SRE [°P]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Temp");
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Alc");
    model->setHeaderData(col, Qt::Horizontal, tr("Alk. [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("tEVG");
    model->setHeaderData(col, Qt::Horizontal, tr("tEVG [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

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

    col = model->fieldIndex("Zeitstempel");
    model->setHeaderData(col, Qt::Horizontal, tr("Datum"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateTimeDelegate(false, false, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Druck");
    model->setHeaderData(col, Qt::Horizontal, tr("Druck [bar]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 10.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Temp");
    model->setHeaderData(col, Qt::Horizontal, tr("Temp. [°C]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("CO2");
    model->setHeaderData(col, Qt::Horizontal, tr("CO2 [g/l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

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
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(layoutChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));

    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
}

TabGaerverlauf::~TabGaerverlauf()
{
    delete ui;
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

void TabGaerverlauf::sudLoaded()
{
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    ui->tbSWSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, "SW").toDouble());
    ui->tbTempSchnellgaerprobe->setValue(model->data(model->rowCount() - 1, "Temp").toDouble());

    model = bh->sud()->modelHauptgaerverlauf();
    ui->tbSWHauptgaerprobe->setValue(model->data(model->rowCount() - 1, "SW").toDouble());
    ui->tbTempHauptgaerprobe->setValue(model->data(model->rowCount() - 1, "Temp").toDouble());

    model = bh->sud()->modelNachgaerverlauf();
    ui->tbNachgaerdruck->setValue(model->data(model->rowCount() - 1, "Druck").toDouble());
    ui->tbNachgaertemp->setValue(model->data(model->rowCount() - 1, "Temp").toDouble());

    ui->tbDatumSchnellgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumHautgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumNachgaerprobe->setDateTime(QDateTime::currentDateTime());

    updateDiagramm();
    updateValues();
    updateWeitereZutaten();
}

void TabGaerverlauf::updateValues()
{
    bool enabled;
    QDateTime dtCurrent = QDateTime::currentDateTime();
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    enabled = bh->sud()->getStatus() == Sud_Status_Gebraut;
    ui->wdgEditSchnellgaerung->setVisible(enabled);
    ui->tableWidget_Schnellgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
    ui->tbDatumSchnellgaerprobe->setMinimumDateTime(bh->sud()->getBraudatum());
    ui->tbDatumSchnellgaerprobe->setMaximumDateTime(dtCurrent);

    enabled = bh->sud()->getStatus() == Sud_Status_Gebraut;
    ui->wdgEditHauptgaerung1->setVisible(enabled);
    ui->wdgEditHauptgaerung2->setVisible(enabled);
    ui->tableWidget_Hauptgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
    ui->tbDatumHautgaerprobe->setMinimumDateTime(bh->sud()->getBraudatum());
    ui->tbDatumHautgaerprobe->setMaximumDateTime(dtCurrent);

    enabled = bh->sud()->getStatus() == Sud_Status_Abgefuellt;
    ui->wdgEditNachgaerung->setVisible(enabled);
    ui->tableWidget_Nachgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
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
        QDateTime dt = model->index(row, 2).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L3Datum.append(dt);
        diag->L1Daten.append(model->index(row, 3).data().toDouble());
        diag->L2Daten.append(model->index(row, 4).data().toDouble());
        diag->L3Daten.append(model->index(row, 5).data().toDouble());
    }
    diag->repaint();

    diag = ui->widget_DiaHauptgaerverlauf;
    model = bh->sud()->modelHauptgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, 2).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L3Datum.append(dt);
        diag->L1Daten.append(model->index(row, 3).data().toDouble());
        diag->L2Daten.append(model->index(row, 4).data().toDouble());
        diag->L3Daten.append(model->index(row, 5).data().toDouble());
    }
    diag->repaint();

    diag = ui->widget_DiaNachgaerverlauf;
    model = bh->sud()->modelNachgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, 2).data().toDateTime();
        diag->Ids.append(row);
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L1Daten.append(model->index(row, 5).data().toDouble());
        diag->L2Daten.append(model->index(row, 4).data().toDouble());
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
    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", ui->tbDatumSchnellgaerprobe->dateTime()},
                        {"SW", ui->tbSWSchnellgaerprobe->value()},
                        {"Temp", ui->tbTempSchnellgaerprobe->value()}});
    bh->sud()->modelSchnellgaerverlauf()->append(values);
}

void TabGaerverlauf::on_btnDelSchnellgaerMessung_clicked()
{
    for (const QModelIndex & index : ui->tableWidget_Schnellgaerverlauf->selectionModel()->selectedIndexes())
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
    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", ui->tbDatumHautgaerprobe->dateTime()},
                        {"SW", ui->tbSWHauptgaerprobe->value()},
                        {"Temp", ui->tbTempHauptgaerprobe->value()}});
    bh->sud()->modelHauptgaerverlauf()->append(values);
}

void TabGaerverlauf::updateWeitereZutaten()
{
    int colName = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("Name");
    int colId = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("ID");
    int colZeitpunkt = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("Zeitpunkt");
    int colStatus = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("Zugabestatus");
    int colEntnahme = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("Entnahmeindex");
    int colMenge = bh->sud()->modelWeitereZutatenGaben()->fieldIndex("erg_Menge");
    ui->comboBox_GaerungEwzAuswahl->clear();
    ui->comboBox_GaerungEwzAuswahlEntnahme->clear();
    for (int i = 0; i < bh->sud()->modelWeitereZutatenGaben()->rowCount(); ++i)
    {
        int zeitpunkt = bh->sud()->modelWeitereZutatenGaben()->index(i, colZeitpunkt).data().toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung)
        {
            QString name = bh->sud()->modelWeitereZutatenGaben()->index(i, colName).data().toString();
            int id = bh->sud()->modelWeitereZutatenGaben()->index(i, colId).data().toInt();
            int status = bh->sud()->modelWeitereZutatenGaben()->index(i, colStatus).data().toInt();
            bool entnahme = !bh->sud()->modelWeitereZutatenGaben()->index(i, colEntnahme).data().toBool();
            int menge = bh->sud()->modelWeitereZutatenGaben()->index(i, colMenge).data().toInt();
            if (status == EWZ_Zugabestatus_nichtZugegeben)
                ui->comboBox_GaerungEwzAuswahl->addItem(name + " (" + QString::number(menge) + "g)", id);
            else if (status == EWZ_Zugabestatus_Zugegeben && entnahme)
                ui->comboBox_GaerungEwzAuswahlEntnahme->addItem(name, id);
        }
    }
    ui->widget_EwzZugeben->setVisible(ui->comboBox_GaerungEwzAuswahl->count() > 0);
    ui->widget_EwzEntnehmen->setVisible(ui->comboBox_GaerungEwzAuswahlEntnahme->count() > 0);
}

void TabGaerverlauf::on_btnGaerungEwzZugeben_clicked()
{
    int id = ui->comboBox_GaerungEwzAuswahl->currentData().toInt();
    int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue("ID", id);
    if (row >= 0)
    {
        QDate currentDate = QDate::currentDate();
        QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
        bh->sud()->modelWeitereZutatenGaben()->setData(row, "ZugabeDatum", date);
        bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zugabestatus", EWZ_Zugabestatus_Zugegeben);
        if (QMessageBox::question(this, tr("Zutat vom Bestand abziehen"),
                                  tr("Sollen die Zutat vom Bestand abgezogen werden?")
           ) == QMessageBox::Yes)
        {
            QString name = bh->sud()->modelWeitereZutatenGaben()->data(row, "Name").toString();
            int typ = bh->sud()->modelWeitereZutatenGaben()->data(row, "Typ").toInt();
            double menge = bh->sud()->modelWeitereZutatenGaben()->data(row, "erg_Menge").toDouble();
            bh->sud()->zutatAbziehen(name, typ == EWZ_Typ_Hopfen ? 0 : 2, menge);
        }
    }
}

void TabGaerverlauf::on_btnGaerungEwzEntnehmen_clicked()
{
    int id = ui->comboBox_GaerungEwzAuswahlEntnahme->currentData().toInt();
    int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue("ID", id);
    if (row >= 0)
    {
        QDate currentDate = QDate::currentDate();
        QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
        bh->sud()->modelWeitereZutatenGaben()->setData(row, "EntnahmeDatum", date);
        bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zugabestatus", EWZ_Zugabestatus_Entnommen);
    }
}

void TabGaerverlauf::on_btnDelHauptgaerMessung_clicked()
{
    for (const QModelIndex & index : ui->tableWidget_Hauptgaerverlauf->selectionModel()->selectedIndexes())
        bh->sud()->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnAddNachgaerMessung_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", ui->tbDatumNachgaerprobe->dateTime()},
                        {"Druck", ui->tbNachgaerdruck->value()},
                        {"Temp", ui->tbNachgaertemp->value()}});
    bh->sud()->modelNachgaerverlauf()->append(values);
}

void TabGaerverlauf::on_btnDelNachgaerMessung_clicked()
{
    for (const QModelIndex & index : ui->tableWidget_Nachgaerverlauf->selectionModel()->selectedIndexes())
        bh->sud()->modelNachgaerverlauf()->removeRow(index.row());
}
