#include "dlgphmalz.h"
#include "ui_dlgphmalz.h"
#include "widgets/widgetdecorator.h"
#include "biercalc.h"

DlgPhMalz::DlgPhMalz(double farbe, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgPhMalz),
    mFarbe(farbe)
{
    WidgetDecorator::suspendValueChanged = true;
    connect(this, &QDialog::finished, [](){WidgetDecorator::suspendValueChanged = false;});

    ui->setupUi(this);
    adjustSize();
}

DlgPhMalz::~DlgPhMalz()
{
    delete ui;
}

void DlgPhMalz::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

double DlgPhMalz::pH() const
{
    return ui->tbPh->value();
}

void DlgPhMalz::setPh(double value)
{
    ui->tbPh->setValue(value);
    ui->cbTyp->setCurrentIndex(0);
}

void DlgPhMalz::on_cbTyp_currentIndexChanged(int index)
{
    ui->tbPh->setReadOnly(true);
    switch (index)
    {
    case 1:
        ui->tbPh->setValue(BierCalc::phMalz(mFarbe));
        break;
    case 2:
        ui->tbPh->setValue(BierCalc::phMalzCarafa(mFarbe));
        break;
    case 3:
        ui->tbPh->setValue(BierCalc::phMalzRoest(mFarbe));
        break;
    case 0:
    default:
        ui->tbPh->setReadOnly(false);
        break;
    }
}
