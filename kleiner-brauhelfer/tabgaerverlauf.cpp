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

    ui->tbDatumSchnellgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumHautgaerprobe->setDateTime(QDateTime::currentDateTime());
    ui->tbDatumNachgaerprobe->setDateTime(QDateTime::currentDateTime());

    ui->widget_DiaSchnellgaerverlauf->BezeichnungL1 = tr("Restextrakt [°P]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL1 = tr("°P");
    ui->widget_DiaSchnellgaerverlauf->BezeichnungL2 = tr("Alkohol [%]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL2 = tr("%");
    ui->widget_DiaSchnellgaerverlauf->BezeichnungL3 = tr("Temperatur [°C]");
    ui->widget_DiaSchnellgaerverlauf->KurzbezeichnungL3 = tr("°C");

    ui->widget_DiaHauptgaerverlauf->BezeichnungL1 = tr("Restextrakt [°P]");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL1 = tr("°P");
    ui->widget_DiaHauptgaerverlauf->BezeichnungL2 = tr("Alkohol [%]");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL2 = tr("%");
    ui->widget_DiaHauptgaerverlauf->BezeichnungL3 = tr("Temperatur °C");
    ui->widget_DiaHauptgaerverlauf->KurzbezeichnungL3 = tr("°C");

    ui->widget_DiaNachgaerverlauf->BezeichnungL1 = tr("CO₂-Gehalt [g/l]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL1 = tr("g/l");
    ui->widget_DiaNachgaerverlauf->BezeichnungL2 = tr("Temperatur [°C]");
    ui->widget_DiaNachgaerverlauf->KurzbezeichnungL2 = tr("°C");

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
    connect(bh->sud(), SIGNAL(modified()), this, SLOT(updateEnabled()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(sortChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelSchnellgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(sortChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelHauptgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(modified()), this, SLOT(updateWeitereZutaten()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(sortChanged()), this, SLOT(updateDiagramm()));
    connect(bh->sud()->modelNachgaerverlauf(), SIGNAL(modified()), this, SLOT(updateDiagramm()));

    updateDiagramm();
    updateEnabled();
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

    updateDiagramm();
    updateEnabled();
    updateWeitereZutaten();
}

void TabGaerverlauf::updateEnabled()
{
    bool enabled;
    QAbstractItemView::EditTriggers triggers = QAbstractItemView::EditTrigger::DoubleClicked |
            QAbstractItemView::EditTrigger::EditKeyPressed |
            QAbstractItemView::EditTrigger::AnyKeyPressed;
    QAbstractItemView::EditTriggers notriggers = QAbstractItemView::EditTrigger::NoEditTriggers;

    enabled = bh->sud()->getSchnellgaerprobeAktiv() && bh->sud()->getBierWurdeGebraut() && !bh->sud()->getBierWurdeAbgefuellt();
    ui->wdgEditSchnellgaerung->setVisible(enabled);
    ui->tableWidget_Schnellgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = bh->sud()->getBierWurdeGebraut() && !bh->sud()->getBierWurdeAbgefuellt();
    ui->wdgEditHauptgaerung1->setVisible(enabled);
    ui->wdgEditHauptgaerung2->setVisible(enabled);
    ui->tableWidget_Hauptgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);

    enabled = bh->sud()->getBierWurdeAbgefuellt() && !bh->sud()->getBierWurdeVerbraucht();
    ui->wdgEditNachgaerung->setVisible(enabled);
    ui->tableWidget_Nachgaerverlauf->setEditTriggers(enabled ? triggers : notriggers);
}

void TabGaerverlauf::updateDiagramm()
{
    WdgDiagramView *diag = ui->widget_DiaSchnellgaerverlauf;
    ProxyModel *model = bh->sud()->modelSchnellgaerverlauf();
    diag->DiagrammLeeren();
    for (int row = 0; row < model->rowCount(); row++)
    {
        QDateTime dt = model->index(row, 2).data().toDateTime();
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
        diag->L1Datum.append(dt);
        diag->L2Datum.append(dt);
        diag->L1Daten.append(model->index(row, 5).data().toDouble());
        diag->L2Daten.append(model->index(row, 4).data().toDouble());
    }
    diag->setWertLinie1(bh->sud()->getCO2());
    diag->repaint();
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
    QDate currentDate = QDate::currentDate();
    QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
    bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zeitpunkt_von", date);
    bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zugabestatus", EWZ_Zugabestatus_Zugegeben);
    if (QMessageBox::question(this, tr("Zutat vom Bestand abziehen"),
                              tr("Sollen die Zutat vom Bestand abgezogen werden?")
       ) == QMessageBox::Yes)
    {
        QString name = bh->sud()->modelWeitereZutatenGaben()->data(row, "Name").toString();
        int typ = bh->sud()->modelWeitereZutatenGaben()->data(row, "Typ").toInt();
        double menge = bh->sud()->modelWeitereZutatenGaben()->data(row, "erg_Menge").toDouble();
        bh->sud()->substractIngredient(name, typ == EWZ_Typ_Hopfen, menge);
    }
}

void TabGaerverlauf::on_btnGaerungEwzEntnehmen_clicked()
{
    int id = ui->comboBox_GaerungEwzAuswahlEntnahme->currentData().toInt();
    int row = bh->sud()->modelWeitereZutatenGaben()->getRowWithValue("ID", id);
    QDate currentDate = QDate::currentDate();
    QDate date = currentDate < ui->tbDatumHautgaerprobe->date() ? currentDate : ui->tbDatumHautgaerprobe->date();
    bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zeitpunkt_bis", date);
    bh->sud()->modelWeitereZutatenGaben()->setData(row, "Zugabestatus", EWZ_Zugabestatus_Entnommen);
}

void TabGaerverlauf::on_btnDelHauptgaerMessung_clicked()
{
    for (const QModelIndex & index : ui->tableWidget_Hauptgaerverlauf->selectionModel()->selectedIndexes())
        bh->sud()->modelHauptgaerverlauf()->removeRow(index.row());
}

void TabGaerverlauf::on_btnAddNachgaerMessung_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Druck", ui->tbNachgaerdruck->value()},
                        {"Temp", ui->tbNachgaertemp->value()}});
    bh->sud()->modelNachgaerverlauf()->append(values);
}

void TabGaerverlauf::on_btnDelNachgaerMessung_clicked()
{
    for (const QModelIndex & index : ui->tableWidget_Nachgaerverlauf->selectionModel()->selectedIndexes())
        bh->sud()->modelNachgaerverlauf()->removeRow(index.row());
}
