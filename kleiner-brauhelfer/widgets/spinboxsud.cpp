#include "spinboxsud.h"
#include "brauhelfer.h"
#include "commands/undostack.h"

SpinBoxSud::SpinBoxSud(QWidget *parent) :
    SpinBox(parent),
    mSud(nullptr),
    mCol(-1)
{
    connect(this, &SpinBox::valueChanged, this, &SpinBoxSud::on_valueChanged);
}

void SpinBoxSud::setColumn(SudObject *sud, int col)
{
    mSud = sud;
    mCol = col;
}

int SpinBoxSud::column() const
{
    return mCol;
}

void SpinBoxSud::updateValue()
{
    setValue(mSud->getValue(mCol).toInt());
}

void SpinBoxSud::on_valueChanged(int val)
{
    if (hasFocus())
        gUndoStack->push(new SetModelDataCommand(mSud->bh()->modelSud(), mSud->row(), mCol, val));
}
