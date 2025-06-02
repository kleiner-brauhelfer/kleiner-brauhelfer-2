#include "dlgvolumen.h"
#include "ui_dlgvolumen.h"
#include <qmath.h>
#include "widgets/widgetdecorator.h"
#include "biercalc.h"

DlgVolumen::DlgVolumen(double durchmesser, double hoehe, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgVolumen)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

    ui->setupUi(this);
    Durchmesser = durchmesser;
    Hoehe = hoehe;

    bool vonObenUnten = Durchmesser > 0 && Hoehe > 0;
    ui->spinBox_VonOben->setVisible(vonObenUnten);
    ui->label_VonOben->setVisible(vonObenUnten);
    ui->label_VonObenEinheit->setVisible(vonObenUnten);
    ui->spinBox_VonUnten->setVisible(vonObenUnten);
    ui->label_VonUnten->setVisible(vonObenUnten);
    ui->label_VonUntenEinheit->setVisible(vonObenUnten);

    if (vonObenUnten)
    {
        double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
        ui->spinBox_Liter->setMaximum(Grundflaeche * (Hoehe / 100 ) / 10);
        ui->spinBox_VonOben->setMaximum(Hoehe);
        ui->spinBox_VonUnten->setMaximum(Hoehe);
    }

    adjustSize();
}

DlgVolumen::DlgVolumen(QWidget *parent) :
    DlgVolumen(0, 0, parent)
{
}

DlgVolumen::~DlgVolumen()
{
    delete ui;
}

void DlgVolumen::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
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

void DlgVolumen::setValue(double Liter)
{
    ui->spinBox_Liter20Grad->setValue(Liter);
    double VT2 = BierCalc::volumenWasser(20, ui->spinBox_Temperatur->value(), Liter);
    ui->spinBox_Liter->setValue(VT2);
	double Grundflaeche = pow(Durchmesser/2, 2) * M_PI ;
    ui->spinBox_VonUnten->setValue(ui->spinBox_Liter->value() * 1000 / Grundflaeche);
    ui->spinBox_VonOben->setValue(Hoehe - ui->spinBox_VonUnten->value());
}

double DlgVolumen::value() const
{
    return ui->spinBox_Liter20Grad->value();
}
