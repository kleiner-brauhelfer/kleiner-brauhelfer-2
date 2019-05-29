#include "dlgrestextrakt.h"
#include "ui_dlgrestextrakt.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgRestextrakt::DlgRestextrakt(double value, double sw, double temp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRestextrakt)
{
    ui->setupUi(this);

    gSettings->beginGroup("General");
    ui->comboBox_FormelBrixPlato->setCurrentIndex(gSettings->value("RefraktometerFormel", 0).toInt());
    ui->tbKorrekturFaktor->setValue(gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble());
    gSettings->endGroup();

    SWAnstellen = sw;
    ui->spinBox_SwPlato->setValue(value);
    ui->spinBox_S_Temperatur->setValue(temp);
    if (ui->spinBox_S_Temperatur->value() == 20.0)
    {
        ui->spinBox_S_SwPlato->setValue(value);
        ui->spinBox_S_SwDichte->setValue(BierCalc::platoToDichte(value));
    }
    else
    {
        ui->spinBox_S_SwPlato->setValue(0.0);
        ui->spinBox_S_SwDichte->setValue(0.0);
    }
    ui->spinBox_R_SwBrix->setValue(SWAnstellen == 0.0 ? BierCalc::platoToBrix(value) : 0.0);
    ui->spinBox_SwPlato->setFocus();
    ui->comboBox_FormelBrixPlato->setVisible(SWAnstellen > 0.0);
    ui->label_FormelBrixPlato->setVisible(SWAnstellen > 0.0);
}

DlgRestextrakt::~DlgRestextrakt()
{
    gSettings->beginGroup("General");
    gSettings->setValue("RefraktometerFormel", ui->comboBox_FormelBrixPlato->currentIndex());
    gSettings->setValue("RefraktometerKorrekturfaktor", ui->tbKorrekturFaktor->value());
    gSettings->endGroup();
    delete ui;
}

double DlgRestextrakt::value() const
{
    return ui->spinBox_SwPlato->value();
}

double DlgRestextrakt::temperature() const
{
    return ui->spinBox_S_Temperatur->value();
}

void DlgRestextrakt::on_spinBox_S_SwPlato_valueChanged(double value)
{
    if (ui->spinBox_S_SwPlato->hasFocus())
    {
        ui->spinBox_S_SwDichte->setValue(BierCalc::platoToDichte(value));
        ui->spinBox_SwPlato->setValue(BierCalc::dichteAtTemp(ui->spinBox_S_SwPlato->value(), ui->spinBox_S_Temperatur->value()));
        ui->spinBox_R_SwBrix->setValue(SWAnstellen == 0.0 ? BierCalc::platoToBrix(ui->spinBox_SwPlato->value()) : 0.0);
    }
}

void DlgRestextrakt::on_spinBox_S_SwDichte_valueChanged(double value)
{
    if (ui->spinBox_S_SwDichte->hasFocus())
    {
        ui->spinBox_S_SwPlato->setValue(BierCalc::dichteToPlato(value));
        ui->spinBox_SwPlato->setValue(BierCalc::dichteAtTemp(ui->spinBox_S_SwPlato->value(), ui->spinBox_S_Temperatur->value()));
        ui->spinBox_R_SwBrix->setValue(SWAnstellen == 0.0 ? BierCalc::platoToBrix(ui->spinBox_SwPlato->value()) : 0.0);
    }
}

void DlgRestextrakt::on_spinBox_S_Temperatur_valueChanged(double)
{
    if (ui->spinBox_S_Temperatur->hasFocus())
    {
        ui->spinBox_SwPlato->setValue(BierCalc::dichteAtTemp(ui->spinBox_S_SwPlato->value(), ui->spinBox_S_Temperatur->value()));
        ui->spinBox_R_SwBrix->setValue(SWAnstellen == 0.0 ? BierCalc::platoToBrix(ui->spinBox_SwPlato->value()) : 0.0);
    }
}

void DlgRestextrakt::calculateFromRefraktometer()
{
    double plato, dichte;
    if (SWAnstellen == 0.0)
    {
        plato = BierCalc::brixToPlato(ui->spinBox_R_SwBrix->value());
        dichte = BierCalc::platoToDichte(plato);
    }
    else
    {
        dichte = BierCalc::brixToDichte(SWAnstellen, ui->spinBox_R_SwBrix->value(), (BierCalc::FormulaBrixToPlato)ui->comboBox_FormelBrixPlato->currentIndex());
        plato = BierCalc::dichteToPlato(dichte);
    }
    ui->spinBox_SwPlato->setValue(plato);
    if (ui->spinBox_S_Temperatur->value() == 20.0)
    {
        ui->spinBox_S_SwPlato->setValue(plato);
        ui->spinBox_S_SwDichte->setValue(dichte);
    }
    else
    {
        ui->spinBox_S_SwPlato->setValue(0.0);
        ui->spinBox_S_SwDichte->setValue(0.0);
    }
}

void DlgRestextrakt::on_spinBox_R_SwBrix_valueChanged(double)
{
    if (ui->spinBox_R_SwBrix->hasFocus())
    {
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_tbKorrekturFaktor_valueChanged(double value)
{
    if (ui->tbKorrekturFaktor->hasFocus())
    {
        BierCalc::faktorBrixToPlato = value;
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_comboBox_FormelBrixPlato_currentIndexChanged(const QString &)
{
    if (ui->comboBox_FormelBrixPlato->hasFocus())
    {
        calculateFromRefraktometer();
    }
}
