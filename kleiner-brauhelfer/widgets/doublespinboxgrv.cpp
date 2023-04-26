#include "doublespinboxgrv.h"
#include "brauhelfer.h"
#include "settings.h"
#include "units.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DoubleSpinBoxGrV::DoubleSpinBoxGrV(QWidget *parent) :
  DoubleSpinBox(parent),
  mCol(-1)
{
    Units::Unit grvunit = Units::GravityUnit();
    setDecimals(Units::decimals(grvunit));
    setMinimum(Units::minimum(grvunit));
    setMaximum(Units::maximum(grvunit));
    setSingleStep(Units::singleStep(grvunit));
}

void DoubleSpinBoxGrV::setColumn(int col)
{
    mCol = col;
    if (mCol != -1)
        connect(this, SIGNAL(valueChanged(double)), SLOT(on_valueChanged(double)));
}

int DoubleSpinBoxGrV::column() const
{
    return mCol;
}

void DoubleSpinBoxGrV::updateValue()
{
    if (mCol == -1)
        return;
    if (!hasFocus() || isReadOnly())
        this->setValue(Units::convert(Units::Plato, Units::GravityUnit(), bh->sud()->getValue(mCol).toDouble()));
}

void DoubleSpinBoxGrV::on_valueChanged(double value)
{
    if (mCol == -1)
        return;
    if (hasFocus() && !isReadOnly())
        bh->sud()->setValue(mCol, Units::convert(Units::GravityUnit(), Units::Plato, value));
}
