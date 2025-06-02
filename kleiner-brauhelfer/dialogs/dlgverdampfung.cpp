#include "dlgverdampfung.h"
#include "ui_dlgverdampfung.h"
#include <qmath.h>
#include "biercalc.h"
#include "widgets/widgetdecorator.h"

DlgVerdampfung::DlgVerdampfung(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgVerdampfung),
    mFlaeche(0)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

	ui->setupUi(this);
    adjustSize();
}

DlgVerdampfung::~DlgVerdampfung()
{
	delete ui;
}

void DlgVerdampfung::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgVerdampfung::on_buttonBox_accepted()
{
	accept();
}

void DlgVerdampfung::on_buttonBox_rejected()
{
    reject();
}

void DlgVerdampfung::on_dSpinBox_Menge1_valueChanged(double )
{
    if (ui->dSpinBox_Menge1->hasFocus())
    {
        ui->dSpinBox_cmVomBodenMenge1->setValue(ui->dSpinBox_Menge1->value() * 1000 / mFlaeche);
        ui->dSpinBox_cmVonObenMenge1->setValue(Hoehe - ui->dSpinBox_cmVomBodenMenge1->value());
    }
    berechne();
}

void DlgVerdampfung::on_dSpinBox_Menge2_valueChanged(double )
{
    if (ui->dSpinBox_Menge2->hasFocus())
    {
        ui->dSpinBox_cmVomBodenMenge2->setValue(ui->dSpinBox_Menge2->value() * 1000 / mFlaeche);
        ui->dSpinBox_cmVonObenMenge2->setValue(Hoehe - ui->dSpinBox_cmVomBodenMenge2->value());
    }
    berechne();
}

void DlgVerdampfung::on_spinBox_Kochdauer_valueChanged(int )
{
    berechne();
}

void DlgVerdampfung::on_dSpinBox_cmVonObenMenge1_valueChanged(double )
{
    if (ui->dSpinBox_cmVonObenMenge1->hasFocus())
    {
        ui->dSpinBox_Menge1->setValue(mFlaeche * (Hoehe - ui->dSpinBox_cmVonObenMenge1->value()) / 1000 );
        ui->dSpinBox_cmVomBodenMenge1->setValue(Hoehe - ui->dSpinBox_cmVonObenMenge1->value());
    }
    berechne();
}

void DlgVerdampfung::on_dSpinBox_cmVomBodenMenge1_valueChanged(double )
{
    if (ui->dSpinBox_cmVomBodenMenge1->hasFocus())
    {
        ui->dSpinBox_Menge1->setValue(mFlaeche * (ui->dSpinBox_cmVomBodenMenge1->value()) / 1000 );
        ui->dSpinBox_cmVonObenMenge1->setValue(Hoehe - ui->dSpinBox_cmVomBodenMenge1->value());
    }
    berechne();
}

void DlgVerdampfung::on_dSpinBox_cmVonObenMenge2_valueChanged(double )
{
    if (ui->dSpinBox_cmVonObenMenge2->hasFocus())
    {
        ui->dSpinBox_Menge2->setValue(mFlaeche * (Hoehe - ui->dSpinBox_cmVonObenMenge2->value()) / 1000 );
        ui->dSpinBox_cmVomBodenMenge2->setValue(Hoehe - ui->dSpinBox_cmVonObenMenge2->value());
    }
    berechne();
}

void DlgVerdampfung::on_dSpinBox_cmVomBodenMenge2_valueChanged(double )
{
    if (ui->dSpinBox_cmVomBodenMenge2->hasFocus())
    {
        ui->dSpinBox_Menge2->setValue(mFlaeche * (ui->dSpinBox_cmVomBodenMenge2->value()) / 1000 );
        ui->dSpinBox_cmVonObenMenge2->setValue(Hoehe - ui->dSpinBox_cmVomBodenMenge2->value());
    }
    berechne();
}

void DlgVerdampfung::berechne()
{
    double erg = BierCalc::verdampfungsziffer(ui->dSpinBox_Menge1->value(), ui->dSpinBox_Menge2->value(), ui->spinBox_Kochdauer->value());
    ui->dSpinBox_Verdampfungsziffer->setValue(erg);
    erg = BierCalc::verdampfungsrate(ui->dSpinBox_Menge1->value(), ui->dSpinBox_Menge2->value(), ui->spinBox_Kochdauer->value());
    ui->dSpinBox_Verdampfungsrate->setValue(erg);
}

void DlgVerdampfung::setHoehe(double value)
{
    Hoehe = value;
}

void DlgVerdampfung::setDurchmesser(double value)
{
    mFlaeche = pow(value/2, 2) * M_PI;
}

void DlgVerdampfung::setMenge1(double value)
{
    ui->dSpinBox_Menge1->setValue(value);
    ui->dSpinBox_cmVomBodenMenge1->setValue(ui->dSpinBox_Menge1->value() * 1000 / mFlaeche);
    ui->dSpinBox_cmVonObenMenge1->setValue(Hoehe - ui->dSpinBox_cmVomBodenMenge1->value());
}

void DlgVerdampfung::setMenge2(double value)
{
    ui->dSpinBox_Menge2->setValue(value);
    ui->dSpinBox_cmVomBodenMenge2->setValue(ui->dSpinBox_Menge2->value() * 1000 / mFlaeche);
    ui->dSpinBox_cmVonObenMenge2->setValue(Hoehe-ui->dSpinBox_cmVomBodenMenge2->value());
}

void DlgVerdampfung::setKochdauer(int value)
{
    ui->spinBox_Kochdauer->setValue(value);
}

double DlgVerdampfung::getVerdampfungsziffer()
{
    return ui->dSpinBox_Verdampfungsziffer->value();
}

double DlgVerdampfung::getVerdampfungsrate()
{
    return ui->dSpinBox_Verdampfungsrate->value();
}
