#include "dlgeinmaischtemp.h"
#include "ui_dlgeinmaischtemp.h"

DlgEinmaischTemp::DlgEinmaischTemp(double schuettung, int schuettungTemp, double hauptguss, int rastTemp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEinmaischTemp)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    ui->doubleSpinBox_Schuettung->setValue(schuettung);
    ui->spinBox_SchuettungTemp->setValue(schuettungTemp);
    ui->doubleSpinBox_Hauptguss->setValue(hauptguss);
    ui->spinBox_RastTemp->setValue(rastTemp);
    doCalc();
}

DlgEinmaischTemp::~DlgEinmaischTemp()
{
    delete ui;
}

void DlgEinmaischTemp::doCalc()
{
    double c_w = 4.2;
    double c_malt = 1.7;
    double m_w = ui->doubleSpinBox_Hauptguss->value() * c_w;
    double m_malt = ui->doubleSpinBox_Schuettung->value() * c_malt;
    int T_malt = ui->spinBox_SchuettungTemp->value();
    int T_rest = ui->spinBox_RastTemp->value();
    int T = (int)(T_rest + m_malt * (T_rest - T_malt) / m_w);
    ui->spinBox_EinmaischeTemp->setValue(T);
}

int DlgEinmaischTemp::value() const
{
    return ui->spinBox_EinmaischeTemp->value();
}

void DlgEinmaischTemp::on_doubleSpinBox_Schuettung_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    doCalc();
}

void DlgEinmaischTemp::on_spinBox_SchuettungTemp_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    doCalc();
}

void DlgEinmaischTemp::on_doubleSpinBox_Hauptguss_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    doCalc();
}

void DlgEinmaischTemp::on_spinBox_RastTemp_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    doCalc();
}
