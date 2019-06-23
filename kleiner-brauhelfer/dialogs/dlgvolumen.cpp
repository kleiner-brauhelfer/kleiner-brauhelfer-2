#include "dlgvolumen.h"
#include "ui_dlgvolumen.h"
#include <qmath.h>
#include "brauhelfer.h"

DlgVolumen::DlgVolumen(double durchmesser, double hoehe, QWidget *parent) : 
	QDialog(parent),
    ui(new Ui::DlgVolumen)
{
    ui->setupUi(this);
    Durchmesser = durchmesser;
    Hoehe = hoehe;

    double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
    ui->spinBox_Liter->setMaximum(Grundflaeche * (Hoehe / 100 ) / 10);
    ui->spinBox_VonOben->setMaximum(Hoehe);
    ui->spinBox_VonUnten->setMaximum(Hoehe);
}

void DlgVolumen::on_buttonBox_accepted()
{
	accept();
}

void DlgVolumen::on_buttonBox_rejected()
{
    reject();
}

void DlgVolumen::on_spinBox_VonOben_valueChanged(double )
{
    if (ui->spinBox_VonOben->hasFocus()){
        double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
        ui->spinBox_Liter->setValue(Grundflaeche * (Hoehe - ui->spinBox_VonOben->value()) / 1000 );
        ui->spinBox_VonUnten->setValue(Hoehe - ui->spinBox_VonOben->value());
		BerLiter20Grad();
	}
}

void DlgVolumen::on_spinBox_VonUnten_valueChanged(double )
{
    if (ui->spinBox_VonUnten->hasFocus()){
        double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
        ui->spinBox_Liter->setValue(Grundflaeche * (ui->spinBox_VonUnten->value()) / 1000 );
        ui->spinBox_VonOben->setValue(Hoehe - ui->spinBox_VonUnten->value());
		BerLiter20Grad();
	}
}

void DlgVolumen::on_spinBox_Temperatur_valueChanged(int )
{
    if (ui->spinBox_Temperatur->hasFocus()){
		BerLiter20Grad();
	}
}

void DlgVolumen::on_spinBox_Liter_valueChanged(double )
{
    if (ui->spinBox_Liter->hasFocus()){
		double Grundflaeche;
		
		Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
        ui->spinBox_VonUnten->setValue(ui->spinBox_Liter->value() * 1000 / Grundflaeche);
        ui->spinBox_VonOben->setValue(Hoehe - ui->spinBox_VonUnten->value());
		BerLiter20Grad();
	}
}

void DlgVolumen::BerLiter20Grad()
{
    double V20 = BierCalc::volumenWasser(ui->spinBox_Temperatur->value(), 20, ui->spinBox_Liter->value());
    ui->spinBox_Liter20Grad->setValue(V20);
}

void DlgVolumen::setLiter(double Liter)
{
    ui->spinBox_Liter20Grad->setValue(Liter);
    double VT2 = BierCalc::volumenWasser(20, ui->spinBox_Temperatur->value(), Liter);
    ui->spinBox_Liter->setValue(VT2);
	double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
    ui->spinBox_VonUnten->setValue(ui->spinBox_Liter->value() * 1000 / Grundflaeche);
    ui->spinBox_VonOben->setValue(Hoehe - ui->spinBox_VonUnten->value());
}

double DlgVolumen::getLiter() const
{
    return ui->spinBox_Liter20Grad->value();
}

void DlgVolumen::setVisibleVonOben(bool value)
{
    ui->label_VonOben->setVisible(value);
    ui->spinBox_VonOben->setVisible(value);
    ui->label_VonObenEinheit->setVisible(value);
}

void DlgVolumen::setVisibleVonUnten(bool value)
{
    ui->label_VonUnten->setVisible(value);
    ui->spinBox_VonUnten->setVisible(value);
    ui->label_VonUntenEinheit->setVisible(value);
}
