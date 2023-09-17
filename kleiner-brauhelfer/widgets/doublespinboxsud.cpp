#include "doublespinboxsud.h"
#include "brauhelfer.h"
#include "commands/undostack.h"

extern Brauhelfer* bh;

DoubleSpinBoxSud::DoubleSpinBoxSud(QWidget *parent) :
    DoubleSpinBox(parent),
    mCol(-1)
{
    connect(this, &DoubleSpinBox::valueChanged, this, &DoubleSpinBoxSud::on_valueChanged);
}

void DoubleSpinBoxSud::setColumn(int col)
{
    mCol = col;
}

int DoubleSpinBoxSud::column() const
{
    return mCol;
}

void DoubleSpinBoxSud::updateValue()
{
    setValue(bh->sud()->getValue(mCol).toDouble());
}

void DoubleSpinBoxSud::on_valueChanged(double val)
{
    if (hasFocus())
        gUndoStack->push(new SetModelDataCommand(bh->modelSud(), bh->sud()->row(), mCol, val));
}
