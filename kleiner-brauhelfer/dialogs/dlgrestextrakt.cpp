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

    //ui->comboBox_FormelBrixPlato->setCurrentText(settings.value("FormelBrixPlato").toString());
    ui->comboBox_FormelBrixPlato->setCurrentIndex(0);

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
}

DlgRestextrakt::~DlgRestextrakt()
{
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
/*
double DlgRestextrakt::brixToPlato()
{
    if (SWAnstellen == 0.0)
    {
        return ui->spinBox_R_SwBrix->value() / ui->spinBox_R_Factor->value();
    }
    else
    {

        double brix = ui->spinBox_R_SwBrix->value();
        double sw = SWAnstellen;
        double brixF = ui->spinBox_R_SwBrix->value() / ui->spinBox_R_Factor->value();

        //Standardformel
        QString formel = ui->comboBox_FormelBrixPlato->currentText();
        if (formel == "Standardformel")
        {
            double dichte =  1.001843-0.002318474*sw - 0.000007775*sw*sw - 0.000000034*sw*sw*sw + 0.00574*brix + 0.00003344*brix*brix + 0.000000086*brix*brix*brix;
            return QBerechnungen::GetGradPlato(dichte);
        }

        else if (formel == "Terrill")
        {
           double dichte = 1 - 0.0044993*sw + 0.0117741*brixF + 0.000275806*sw*sw - 0.00127169*brixF*brixF - 0.00000727999*sw*sw*sw + 0.0000632929*brixF*brixF*brixF;
           return QBerechnungen::GetGradPlato(dichte);
        }

        else if (formel == "Terrill linear")
        {
            double dichte = 1.0000 - 0.00085683*sw + 0.0034941*brixF;
            return QBerechnungen::GetGradPlato(dichte);
        }

        // User Kleier -> Quelle: http://hobbybrauer.de/modules.php?name=eBoard&file=viewthread&tid=11943&page=2#pid129201
        else
        {
            double Ballingkonstante = 2.0665;
            //tatsächlicher Restextrakt
            double tr =(Ballingkonstante * brix - Gaerungskorrektur * sw)/(Ballingkonstante
                    * ui->spinBox_R_Factor->value() - Gaerungskorrektur);
            //Scheinbarer Restextrakt
            return tr * (1.22 + 0.001 * sw) - ((0.22 + 0.001 * sw) * sw);
        }
    }
}
*/
void DlgRestextrakt::on_spinBox_R_SwBrix_valueChanged(double value)
{
    if (ui->spinBox_R_SwBrix->hasFocus())
    {
        double plato, dichte;
        if (SWAnstellen == 0.0)
        {
            plato = BierCalc::brixToPlato(value);
            dichte = BierCalc::platoToDichte(plato);
        }
        else
        {
            dichte = BierCalc::brixToDichte(SWAnstellen, value, (BierCalc::FormulaBrixToPlato)ui->comboBox_FormelBrixPlato->currentIndex());
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
}

void DlgRestextrakt::on_comboBox_FormelBrixPlato_currentIndexChanged(const QString &)
{
    if (ui->comboBox_FormelBrixPlato->hasFocus())
    {
        ui->spinBox_R_SwBrix->setFocus();
        on_spinBox_R_SwBrix_valueChanged(ui->spinBox_R_SwBrix->value());
        /*
        double plato, dichte;
        if (SWAnstellen == 0.0)
        {
            plato = BierCalc::brixToPlato(ui->spinBox_R_SwBrix->value());
            dichte = BierCalc::platoToDichte(plato);
        }
        else
        {
            dichte = BierCalc::brixToDichte(SWAnstellen, ui->spinBox_R_SwBrix->value());
            plato = BierCalc::dichteToPlato(dichte);
        }
        ui->spinBox_SwPlato->setValue(plato);
        ui->spinBox_S_Temperatur->setValue(20.0);
        ui->spinBox_S_SwPlato->setValue(plato);
        ui->spinBox_S_SwDichte->setValue(dichte);
        */
    }
}
