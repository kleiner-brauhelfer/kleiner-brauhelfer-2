#include "spinboxsud.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

SpinBoxSud::SpinBoxSud(QWidget *parent) :
    SpinBox(parent),
    mCol(-1)
{
}

void SpinBoxSud::setColumn(int col)
{
    mCol = col;
    if (mCol != -1)
        connect(this, &SpinBox::valueChanged, this, &SpinBoxSud::on_valueChanged);
}

int SpinBoxSud::column() const
{
    return mCol;
}

void SpinBoxSud::updateValue()
{
    if (mCol == -1)
        return;
    if (!hasFocus() || isReadOnly())
        setValue(bh->sud()->getValue(mCol).toInt());
}

void SpinBoxSud::on_valueChanged(int value)
{
    if (mCol == -1)
        return;
    if (hasFocus() && !isReadOnly())
        bh->sud()->setValue(mCol, value);
}
