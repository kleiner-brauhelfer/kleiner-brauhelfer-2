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

    ui->tbTemp->setValue(temp);
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

    adjustSize();
    gSettings->beginGroup("DlgRestextrakt");
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();
}

DlgRestextrakt::~DlgRestextrakt()
{
    gSettings->beginGroup("DlgRestextrakt");
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

void DlgRestextrakt::on_DlgRestextrakt_accepted()
{
    gSettings->beginGroup("General");
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
}

void DlgRestextrakt::on_tbPlato_valueChanged(double value)
{
    if (ui->tbPlato->hasFocus())
    {
        ui->tbDichte->setValue(BierCalc::platoToDichte(value));
        if (mSw)
        {
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            if (ui->cbEinheit->currentIndex() == 0)
                ui->tbBrix->setValue(BierCalc::platoToBrix(ui->tbSw->value()));
            else
                ui->tbBrix->setValue(ui->tbSw->value());
        }
        else
        {
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            ui->tbBrix->setValue(0.0);
        }
    }
}

void DlgRestextrakt::on_tbDichte_valueChanged(double value)
{
    if (ui->tbDichte->hasFocus())
    {
        ui->tbPlato->setValue(BierCalc::dichteToPlato(value));
        if (mSw)
        {
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            if (ui->cbEinheit->currentIndex() == 0)
                ui->tbBrix->setValue(BierCalc::platoToBrix(ui->tbSw->value()));
            else
                ui->tbBrix->setValue(ui->tbSw->value());
        }
        else
        {
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            ui->tbBrix->setValue(0.0);
        }
    }
}

void DlgRestextrakt::on_tbTemp_valueChanged(double)
{
    if (ui->tbTemp->hasFocus())
    {
        if (mSw)
        {
            ui->tbSw->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            if (ui->cbEinheit->currentIndex() == 0)
                ui->tbBrix->setValue(BierCalc::platoToBrix(ui->tbSw->value()));
            else
                ui->tbBrix->setValue(ui->tbSw->value());
        }
        else
        {
            ui->tbExtrakt->setValue(BierCalc::spindelKorrektur(ui->tbPlato->value(), ui->tbTemp->value()));
            ui->tbBrix->setValue(0.0);
        }
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
        dichte = BierCalc::platoToDichte(plato);
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
    if (ui->tbTemp->value() == 20.0)
    {
        ui->tbPlato->setValue(plato);
        ui->tbDichte->setValue(dichte);
    }
    else
    {
        ui->tbPlato->setValue(0.0);
        ui->tbDichte->setValue(0.0);
    }
}

void DlgRestextrakt::on_tbBrix_valueChanged(double)
{
    if (ui->tbBrix->hasFocus())
    {
        calculateFromRefraktometer();
    }
}

void DlgRestextrakt::on_cbEinheit_activated()
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

void DlgRestextrakt::on_comboBox_FormelBrixPlato_currentIndexChanged(const QString &)
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
