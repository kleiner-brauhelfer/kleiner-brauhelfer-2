#include "doublespinboxsud.h"
#include "brauhelfer.h"
#include "commands/undostack.h"

DoubleSpinBoxSud::DoubleSpinBoxSud(QWidget *parent) :
    DoubleSpinBox(parent),
    mSud(nullptr),
    mCol(-1)
{
    connect(this, &DoubleSpinBox::valueChanged, this, &DoubleSpinBoxSud::on_valueChanged);
}

void DoubleSpinBoxSud::setColumn(SudObject *sud, int col)
{
    mSud = sud;
    mCol = col;
}

int DoubleSpinBoxSud::column() const
{
    return mCol;
}

void DoubleSpinBoxSud::updateValue()
{
    setValue(mSud->getValue(mCol).toDouble());
}

void DoubleSpinBoxSud::on_valueChanged(double val)
{
    if (hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), mCol, val));
}
