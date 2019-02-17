#include "tabbraudaten.h"
#include "ui_tabbraudaten.h"
#include <QMessageBox>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgvolumen.h"
#include "dialogs/dlgsudteilen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBraudaten::TabBraudaten(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBraudaten)
{
    ui->setupUi(this);
    ui->lblCurrency->setText(QLocale().currencySymbol());
    ui->lblCurrency2->setText(QLocale().currencySymbol() + "/" + tr("l"));

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    int col;
    ProxyModel *model = bh->sud()->modelMalzschuettung();
    QTableView *table = ui->tableMalz;
    QHeaderView *header = table->horizontalHeader();
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = model->fieldIndex("Name");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Malzschüttung"));
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);
    col = model->fieldIndex("Prozent");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Anteil [%]"));
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);
    col = model->fieldIndex("erg_Menge");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [kg]"));
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    ProxyModel *proxy = new ProxyModel(this);
    model = bh->sud()->modelWeitereZutatenGaben();
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(model->fieldIndex("Zeitpunkt"));
    proxy->setFilterString(QString::number(EWZ_Zeitpunkt_Maischen));
    table = ui->tableWeitereZutatenMaischen;
    table->setModel(proxy);
    header = table->horizontalHeader();
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = model->fieldIndex("Name");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Weitere Zutat"));
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);
    col = model->fieldIndex("erg_Menge");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [g]"));
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    model = bh->sud()->modelHopfengaben();
    table = ui->tableHopfen;
    table->setModel(model);
    header = table->horizontalHeader();
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = model->fieldIndex("Name");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Hopfengabe"));
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);
    col = model->fieldIndex("Vorderwuerze");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Vorderwürze"));
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);
    col = model->fieldIndex("Zeit");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Kochdauer [min]"));
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);
    col = model->fieldIndex("erg_Menge");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [g]"));
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    proxy = new ProxyModel(this);
    model = bh->sud()->modelWeitereZutatenGaben();
    proxy->setSourceModel(model);
    proxy->setFilterKeyColumn(model->fieldIndex("Zeitpunkt"));
    proxy->setFilterString(QString::number(EWZ_Zeitpunkt_Kochen));
    table = ui->tableWeitereZutatenKochen;
    table->setModel(proxy);
    header = table->horizontalHeader();
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = model->fieldIndex("Name");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Weitere Zutat"));
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);
    col = model->fieldIndex("Zugabedauer");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Kochdauer [min]"));
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);
    col = model->fieldIndex("erg_Menge");
    table->setColumnHidden(col, false);
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [g]"));
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    //model = bh->sud()->modelHefegaben();
    table = ui->tableHefe;
    header = table->horizontalHeader();
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    gSettings->beginGroup("TabBraudaten");

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));
}

TabBraudaten::~TabBraudaten()
{
    delete ui;
}

void TabBraudaten::saveSettings()
{
    gSettings->beginGroup("TabBraudaten");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->endGroup();
}

void TabBraudaten::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
}

void TabBraudaten::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabBraudaten::sudLoaded()
{
    checkEnabled();
    updateValues();
}

void TabBraudaten::sudDataChanged(const QModelIndex& index)
{
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    QString fieldname = model->fieldName(index.column());
    if (fieldname == "BierWurdeGebraut" ||
        fieldname == "BierWurdeAbgefuellt" ||
        fieldname == "BierWurdeVerbraucht")
    {
        checkEnabled();
    }
}

void TabBraudaten::checkEnabled()
{
    bool gebraut = bh->sud()->getBierWurdeGebraut();
    ui->tbBraudatum->setReadOnly(gebraut);
    ui->btnBraudatumHeute->setVisible(!gebraut);
    ui->lblSpacer->setVisible(!gebraut);
    ui->tbWuerzemengeKochbeginn->setReadOnly(gebraut);
    ui->btnWuerzemengeKochbeginn->setVisible(!gebraut);
    ui->tbWuerzemengeKochende->setReadOnly(gebraut);
    ui->btnWuerzemengeKochende->setVisible(!gebraut);
    ui->tbSWKochende->setReadOnly(gebraut);
    ui->btnSWKochende->setVisible(!gebraut);
    ui->tbSWAnstellen->setReadOnly(gebraut);
    ui->btnSWAnstellen->setVisible(!gebraut);
    ui->tbWuerzemengeAnstellenTotal->setReadOnly(gebraut);
    ui->tbSpeisemenge->setReadOnly(gebraut);
    ui->tbWuerzemengeAnstellen->setReadOnly(gebraut);
    ui->btnSudGebraut->setEnabled(!gebraut);
}

void TabBraudaten::updateValues()
{
    double value;

    ui->tableMalz->setVisible(ui->tableMalz->model()->rowCount() > 0);
    ui->tableWeitereZutatenMaischen->setVisible(ui->tableWeitereZutatenMaischen->model()->rowCount() > 0);
    ui->tableHopfen->setVisible(ui->tableHopfen->model()->rowCount() > 0);
    ui->tableWeitereZutatenKochen->setVisible(ui->tableWeitereZutatenKochen->model()->rowCount() > 0);
    ui->tableHefe->setVisible(false/*ui->tableHefe->model()->rowCount()*/);

    QDateTime dt = bh->sud()->getBraudatum();
    if (bh->sud()->getBierWurdeGebraut() || dt.isValid())
        ui->tbBraudatum->setDateTime(dt);
    else
        ui->tbBraudatum->setDateTime(QDateTime::currentDateTime());

    if (!ui->tbWuerzemengeKochbeginn->hasFocus())
           ui->tbWuerzemengeKochbeginn->setValue(bh->sud()->getWuerzemengeVorHopfenseihen());
    ui->tbWuerzemengeKochende->setMaximum(ui->tbWuerzemengeKochbeginn->value());
    if (!ui->tbWuerzemengeKochende->hasFocus())
        ui->tbWuerzemengeKochende->setValue(bh->sud()->getWuerzemengeKochende());
    if (!ui->tbSWKochende->hasFocus())
        ui->tbSWKochende->setValue(bh->sud()->getSWKochende());
    ui->tbSWAnstellenSoll->setValue(bh->sud()->getSWSollAnstellen());
    value = BierCalc::verschneidung(bh->sud()->getSWAnstellen(),
                                    bh->sud()->getSWSollAnstellen(),
                                    bh->sud()->getWuerzemengeKochende() * (1 + bh->sud()->gethighGravityFaktor()/100));
    ui->tbWasserVerschneidung->setValue(value);
    ui->tbSWAnstellen->setMaximum(ui->tbSWKochende->value());
    if (!ui->tbSWAnstellen->hasFocus())
        ui->tbSWAnstellen->setValue(bh->sud()->getSWAnstellen());
    ui->tbWuerzemengeAnstellenTotal->setMaximum(ui->tbWuerzemengeKochende->value());
    if (!ui->tbWuerzemengeAnstellenTotal->hasFocus())
        ui->tbWuerzemengeAnstellenTotal->setValue(bh->sud()->getWuerzemengeAnstellenTotal());
    if (!ui->tbSpeisemenge->hasFocus())
        ui->tbSpeisemenge->setValue(bh->sud()->getSpeisemenge());
    ui->tbWuerzemengeAnstellen->setMaximum(ui->tbWuerzemengeKochende->value() - ui->tbSpeisemenge->value());
    if (!ui->tbWuerzemengeAnstellen->hasFocus())
        ui->tbWuerzemengeAnstellen->setValue(bh->sud()->getWuerzemengeAnstellen());

    ui->tbVerlustAnstellen->setValue(ui->tbWuerzemengeKochende->value() - ui->tbWuerzemengeAnstellenTotal->value());
    ui->tbSpeisemengeNoetig->setValue(BierCalc::speise(bh->sud()->getCO2(), bh->sud()->getSWAnstellen(), 3.0, 3.0, 20.0) * bh->sud()->getWuerzemengeAnstellen());


    ui->cbDurchschnittIgnorieren->setChecked(bh->sud()->getAusbeuteIgnorieren());

    value = bh->sud()->getRestalkalitaetFaktor();
    ui->tbHauptguss->setValue(bh->sud()->geterg_WHauptguss());
    ui->tbMilchsaeureHG->setValue(ui->tbHauptguss->value() * value);
    ui->tbNachguss->setValue(bh->sud()->geterg_WNachguss());
    ui->tbMilchsaeureNG->setValue(ui->tbNachguss->value() * value);
    ui->tbMengeSollKochbeginn20->setValue(bh->sud()->getMengeSollKochbeginn());
    ui->tbMengeSollKochbeginn100->setValue(BierCalc::volumenWasser(20.0, 100.0, ui->tbMengeSollKochbeginn20->value()));
    value = pow(bh->sud()->getAnlageValue("Sudpfanne_Durchmesser").toDouble() / 2, 2) * M_PI;
    ui->tbMengeSollcmVomBoden->setValue(bh->sud()->getMengeSollKochbeginn() * 1000 / value);
    ui->tbMengeSollcmVonOben->setValue(bh->sud()->getAnlageValue("Sudpfanne_Hoehe").toDouble() - ui->tbMengeSollcmVomBoden->value());
    ui->tbSWSollKochbeginn->setValue(bh->sud()->getSWSollKochbeginn());
    ui->tbMengeSollKochende20->setValue(bh->sud()->getMengeSollKochende());
    ui->tbMengeSollKochende100->setValue(BierCalc::volumenWasser(20.0, 100.0, ui->tbMengeSollKochende20->value()));
    ui->tbSWSollKochende->setValue(bh->sud()->getSWSollKochende());
    ui->tbVerdampfung->setValue(bh->sud()->getVerdampfungsziffer());
    ui->tbAusbeute->setValue(bh->sud()->geterg_Sudhausausbeute());
    ui->tbAusbeuteEffektiv->setValue(bh->sud()->geterg_EffektiveAusbeute());
    ui->tbKosten->setValue(bh->sud()->geterg_Preis());
}

void TabBraudaten::on_tbBraudatum_dateTimeChanged(const QDateTime &dateTime)
{
    if (ui->tbBraudatum->hasFocus())
        bh->sud()->setBraudatum(dateTime);
}

void TabBraudaten::on_btnBraudatumHeute_clicked()
{
    bh->sud()->setBraudatum(QDateTime::currentDateTime());
}

void TabBraudaten::on_tbWuerzemengeKochbeginn_valueChanged(double value)
{
    if (ui->tbWuerzemengeKochbeginn->hasFocus())
        bh->sud()->setWuerzemengeVorHopfenseihen(value);
}

void TabBraudaten::on_btnWuerzemengeKochbeginn_clicked()
{
    double d = bh->sud()->getAnlageValue("Sudpfanne_Durchmesser").toDouble();
    double h = bh->sud()->getAnlageValue("Sudpfanne_Hoehe").toDouble();
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeKochbeginn->value());
    if (dlg.exec() == QDialog::Accepted)
        ui->tbWuerzemengeKochbeginn->setValue(dlg.getLiter());
}

void TabBraudaten::on_tbWuerzemengeKochende_valueChanged(double value)
{
    if (ui->tbWuerzemengeKochende->hasFocus())
        bh->sud()->setWuerzemengeKochende(value);
}

void TabBraudaten::on_btnWuerzemengeKochende_clicked()
{
    double d = bh->sud()->getAnlageValue("Sudpfanne_Durchmesser").toDouble();
    double h = bh->sud()->getAnlageValue("Sudpfanne_Hoehe").toDouble();
    DlgVolumen dlg(d, h, this);
    dlg.setLiter(ui->tbWuerzemengeKochende->value());
    dlg.setVisibleVonOben(false);
    dlg.setVisibleVonUnten(false);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbWuerzemengeKochende->setValue(dlg.getLiter());
}

void TabBraudaten::on_tbSWKochende_valueChanged(double value)
{
    if (ui->tbSWKochende->hasFocus())
        bh->sud()->setSWKochende(value);
}

void TabBraudaten::on_btnSWKochende_clicked()
{
    DlgRestextrakt dlg(ui->tbSWKochende->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWKochende->setValue(dlg.value());
}

void TabBraudaten::on_tbSWAnstellen_valueChanged(double value)
{
    if (ui->tbSWAnstellen->hasFocus())
        bh->sud()->setSWAnstellen(value);
}

void TabBraudaten::on_btnSWAnstellen_clicked()
{
    DlgRestextrakt dlg(ui->tbSWAnstellen->value(), 0.0, 20.0, this);
    if (dlg.exec() == QDialog::Accepted)
        ui->tbSWAnstellen->setValue(dlg.value());
}

void TabBraudaten::on_tbWuerzemengeAnstellenTotal_valueChanged(double value)
{
    if (ui->tbWuerzemengeAnstellenTotal->hasFocus())
        bh->sud()->setWuerzemengeAnstellenTotal(value);
}

void TabBraudaten::on_tbSpeisemenge_valueChanged(double value)
{
    if (ui->tbSpeisemenge->hasFocus())
        bh->sud()->setSpeisemenge(value);
}

void TabBraudaten::on_tbWuerzemengeAnstellen_valueChanged(double value)
{
    if (ui->tbWuerzemengeAnstellen->hasFocus())
        bh->sud()->setWuerzemengeAnstellen(value);
}

void TabBraudaten::on_tbNebenkosten_valueChanged(double value)
{
    if (ui->tbNebenkosten->hasFocus())
        bh->sud()->setKostenWasserStrom(value);
}

void TabBraudaten::on_btnSudGebraut_clicked()
{
    bh->sud()->setBraudatum(ui->tbBraudatum->dateTime());
    bh->sud()->setBierWurdeGebraut(true);

    if (QMessageBox::question(this, tr("Zutaten vom Bestand abziehen"),
                              tr("Sollen die bisher verwendeten Zutaten vom Bestand abgezogen werden?")
       ) == QMessageBox::Yes)
        bh->sud()->substractBrewIngredients();

    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Zeitstempel", bh->sud()->getBraudatum()},
                        {"SW", bh->sud()->getSWIst()}, {"Temp", 20.0}});
    if (bh->sud()->modelSchnellgaerverlauf()->rowCount() == 0)
        bh->sud()->modelSchnellgaerverlauf()->append(values);
    if (bh->sud()->modelHauptgaerverlauf()->rowCount() == 0)
        bh->sud()->modelHauptgaerverlauf()->append(values);
}

void TabBraudaten::on_btnSudTeilen_clicked()
{
    DlgSudTeilen dlg(bh->sud()->getSudname(), bh->sud()->getMengeIst(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
    }
}
