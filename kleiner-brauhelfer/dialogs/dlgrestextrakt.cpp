#include "dlgrestextrakt.h"
#include "ui_dlgrestextrakt.h"
#include "widgets/widgetdecorator.h"
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgRestextrakt::DlgRestextrakt(double value, double sw, double temp, const QDateTime& dt, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRestextrakt)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

    ui->setupUi(this);

    gSettings->beginGroup("General");
    ui->tbEichtemp->setValue(gSettings->value("SpindelEichtemperatur", 20.0).toDouble());
    ui->comboBox_FormelBrixPlato->setCurrentIndex(gSettings->value("RefraktometerFormel", 0).toInt());
    BierCalc::faktorPlatoToBrix = gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble();
    ui->cbEinheit->setCurrentIndex(gSettings->value("RefraktometerEinheit", 0).toInt());
    ui->tbKorrekturFaktor->setValue(BierCalc::faktorPlatoToBrix);
    gSettings->endGroup();

    if (sw == 0.0)
    {
        setWindowTitle(tr("Stammwürze"));
        ui->WdgExtrakt->setVisible(false);
        ui->comboBox_FormelBrixPlato->setVisible(false);
        ui->label_FormelBrixPlato->setVisible(false);
        ui->tbSw->setValue(value);
        mSw = true;
    }
    else
    {
        setWindowTitle(tr("Restextrakt"));
        ui->tbSw->setValue(sw);
        ui->tbExtrakt->setValue(value);
        mSw = false;
    }

    if (temp < 0)
    {
        ui->tbTempRefraktometer->setVisible(false);
        ui->lblTempRefraktometer->setVisible(false);
        ui->lblTempRefraktometerEinheit->setVisible(false);
        ui->tbTempManuell->setVisible(false);
        ui->lblTempManuell->setVisible(false);
        ui->lblTempManuellEinheit->setVisible(false);
        temp = 20.0;
    }

    ui->dtDatum->setDateTime(dt);
    ui->wdgDatum->setVisible(dt.isValid());

    ui->tbTemp->setValue(temp);
    ui->tbTempRefraktometer->setValue(temp);
    ui->tbTempManuell->setValue(temp);
    if (ui->tbTemp->value() == 20.0)
    {
        ui->tbPlato->setValue(value);
        ui->tbDichte->setValue(BierCalc::platoToDichte(value));
    }
    else
    {
        ui->tbPlato->setValue(0.0);
        ui->tbDichte->setValue(0.0);
    }
    if (sw == 0.0)
    {
        if (ui->cbEinheit->currentIndex() == 0)
            ui->tbBrix->setValue(BierCalc::platoToBrix(value));
        else
            ui->tbBrix->setValue(value);
    }
    ui->tbPlatoManuell->setValue(value);

    adjustSize();
    gSettings->beginGroup(staticMetaObject.className());
    ui->cbAuswahl->setCurrentIndex(gSettings->value("type", 0).toInt());
    on_cbAuswahl_currentIndexChanged(ui->cbAuswahl->currentIndex());
    gSettings->endGroup();
}

DlgRestextrakt::~DlgRestextrakt()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("type", ui->cbAuswahl->currentIndex());
    gSettings->endGroup();
    delete ui;
}

void DlgRestextrakt::on_DlgRestextrakt_accepted()
{
    gSettings->beginGroup("General");
    gSettings->setValue("SpindelEichtemperatur", ui->tbEichtemp->value());
    gSettings->setValue("RefraktometerFormel", ui->comboBox_FormelBrixPlato->currentIndex());
    gSettings->setValue("RefraktometerKorrekturfaktor", ui->tbKorrekturFaktor->value());
    gSettings->setValue("RefraktometerEinheit", ui->cbEinheit->currentIndex());
    gSettings->endGroup();
}

double DlgRestextrakt::value() const
{
    return mSw ? ui->tbSw->value() : ui->tbExtrakt->value();
}

void DlgRestextrakt::setValue(double value)
{
    if (mSw)
        ui->tbSw->setValue(value);
    else
        ui->tbExtrakt->setValue(value);
}

double DlgRestextrakt::temperatur() const
{
    return ui->tbTemp->value();
}

void DlgRestextrakt::setTemperatur(double value)
{
    ui->tbTemp->setValue(value);
    ui->tbTempRefraktometer->setValue(value);
    ui->tbTempManuell->setValue(value);
}

QDateTime DlgRestextrakt::datum() const
{
    return ui->dtDatum->dateTime();
}

void DlgRestextrakt::setDatum(const QDateTime& value)
{
    ui->dtDatum->setDateTime(value);
}

void DlgRestextrakt::on_cbAuswahl_currentIndexChanged(int index)
{
    ui->wdgSpindel->setVisible(index == 0);
    ui->wdgRefraktometer->setVisible(index == 1);
    ui->wdgManuell->setVisible(index == 2);
    adjustSize();
}

void DlgRestextrakt::on_tbPlato_valueChanged(double value)
{
    if (ui->tbPlato->hasFocus())
    {
        ui->tbDichte->setValue(BierCalc::platoToDichte(value));
        if (mSw)
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
        else
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
    }
}

void DlgRestextrakt::on_tbDichte_valueChanged(double value)
{
    if (ui->tbDichte->hasFocus())
    {
        ui->tbPlato->setValue(BierCalc::dichteToPlato(value));
        if (mSw)
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
        else
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
    }
}

void DlgRestextrakt::on_tbTemp_valueChanged(double value)
{
    if (ui->tbTemp->hasFocus())
    {
        ui->tbTempRefraktometer->setValue(value);
        ui->tbTempManuell->setValue(value);
        if (mSw)
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
        else
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), ui->tbEichtemp->value()));
    }
}

void DlgRestextrakt::on_tbEichtemp_valueChanged(double value)
{
    if (ui->tbEichtemp->hasFocus())
    {
        if (mSw)
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), value));
        else
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value(), value));
    }
}

void DlgRestextrakt::calculateFromRefraktometer()
{
    double plato, dichte;
    if (mSw)
    {
        if (ui->cbEinheit->currentIndex() == 0)
            plato = BierCalc::brixToPlato(ui->tbBrix->value());
        else
            plato = ui->tbBrix->value();
        ui->tbSw->setValue(plato);
    }
    else
    {
        if (ui->cbEinheit->currentIndex() == 0)
            dichte = BierCalc::brixToDichte(ui->tbSw->value(), ui->tbBrix->value(), (BierCalc::FormulaBrixToPlato)ui->comboBox_FormelBrixPlato->currentIndex());
        else
            dichte = BierCalc::brixToDichte(ui->tbSw->value(), BierCalc::platoToBrix(ui->tbBrix->value()), (BierCalc::FormulaBrixToPlato)ui->comboBox_FormelBrixPlato->currentIndex());
        plato = BierCalc::dichteToPlato(dichte);
        ui->tbExtrakt->setValue(plato);
    }
}

void DlgRestextrakt::on_tbBrix_valueChanged(double)
{
    if (ui->tbBrix->hasFocus())
    {
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_cbEinheit_activated(int)
{
    calculateFromRefraktometer();
}

void DlgRestextrakt::on_tbKorrekturFaktor_valueChanged(double value)
{
    if (ui->tbKorrekturFaktor->hasFocus())
    {
        BierCalc::faktorPlatoToBrix = value;
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_comboBox_FormelBrixPlato_currentTextChanged(const QString &)
{
    if (ui->comboBox_FormelBrixPlato->hasFocus())
    {
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_btnKorrekturFaktorDefault_clicked()
{
    ui->tbKorrekturFaktor->setValue(1.03);
    BierCalc::faktorPlatoToBrix = ui->tbKorrekturFaktor->value();
    calculateFromRefraktometer();
}

void DlgRestextrakt::on_tbTempRefraktometer_valueChanged(double value)
{
    if (ui->tbTempRefraktometer->hasFocus())
    {
        ui->tbTemp->setValue(value);
        ui->tbTempManuell->setValue(value);
    }
}

void DlgRestextrakt::on_tbPlatoManuell_valueChanged(double value)
{
    if (ui->tbPlatoManuell->hasFocus())
    {
        if (mSw)
            ui->tbSw->setValue(value);
        else
            ui->tbExtrakt->setValue(value);
    }
}

void DlgRestextrakt::on_tbTempManuell_valueChanged(double value)
{
    if (ui->tbTempManuell->hasFocus())
    {
        ui->tbTemp->setValue(value);
        ui->tbTempRefraktometer->setValue(value);
    }
}
