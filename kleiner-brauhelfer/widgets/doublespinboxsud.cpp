#include "doublespinboxsud.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

DoubleSpinBoxSud::DoubleSpinBoxSud(QWidget *parent) :
    DoubleSpinBox(parent),
    mCol(-1)
{
}

void DoubleSpinBoxSud::setColumn(int col)
{
    mCol = col;
    if (mCol != -1)
        connect(this, &DoubleSpinBox::valueChanged, this, &DoubleSpinBoxSud::on_valueChanged);
}

int DoubleSpinBoxSud::column() const
{
    return mCol;
}

void DoubleSpinBoxSud::updateValue()
{
    if (mCol == -1)
        return;
    if (!hasFocus() || isReadOnly())
        setValue(bh->sud()->getValue(mCol).toDouble());
}

void DoubleSpinBoxSud::on_valueChanged(double value)
{
    if (mCol == -1)
        return;
    if (hasFocus() && !isReadOnly())
        bh->sud()->setValue(mCol, value);
}
