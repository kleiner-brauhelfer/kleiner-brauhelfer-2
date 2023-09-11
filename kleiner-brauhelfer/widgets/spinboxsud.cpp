#include "spinboxsud.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

SpinBoxSud::SpinBoxSud(QWidget *parent) :
    SpinBox(parent),
    mCol(-1)
{
    connect(this, &SpinBox::valueChanged, this, &SpinBoxSud::on_valueChanged);
}

void SpinBoxSud::setColumn(int col)
{
    mCol = col;
}

int SpinBoxSud::column() const
{
    return mCol;
}

void SpinBoxSud::updateValue()
{
    setValue(bh->sud()->getValue(mCol).toInt());
}

void SpinBoxSud::on_valueChanged(int val)
{
    if (hasFocus())
        bh->sud()->setValue(mCol, val);
}
