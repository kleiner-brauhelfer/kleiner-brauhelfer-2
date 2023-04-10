#include "doublespinboxgrv.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DoubleSpinBoxGrV::DoubleSpinBoxGrV(QWidget *parent) :
  DoubleSpinBox(parent),
  mCol(-1)
{

}

void DoubleSpinBoxGrV::setColumn(int col)
{
  mCol = col;
  if (gSettings->GravityUnit() == BierCalc::GravityUnit::SG) {
        this->setDecimals(3);
        this->setMinimum(0.000);
        this->setSingleStep(0.001);
    } else {
                this->setDecimals(1);
                this->setMinimum(0);
                this->setSingleStep(0.1);
      }
    if (mCol != -1) {
  connect(this, SIGNAL(valueChanged(double)), SLOT(on_valueChanged(double)));
   }
}

int DoubleSpinBoxGrV::column() const
{
  return mCol;
}

void DoubleSpinBoxGrV::updateValue()
{
  BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
  if (mCol == -1) {
    return;
  }
  if (!hasFocus() || isReadOnly()) {
    this->setValue(BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getValue(mCol).toDouble()));
}
}

void DoubleSpinBoxGrV::on_valueChanged(double value)
{
  BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
  if (mCol == -1) {
   return;
  }
  if (hasFocus() && !isReadOnly()) {
   bh->sud()->setValue(mCol, BierCalc::convertGravity(grvunit,BierCalc::GravityUnit::Plato,value));
}
}
