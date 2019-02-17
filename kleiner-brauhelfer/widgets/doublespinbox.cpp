#include "doublespinbox.h"
#include "settings.h"

extern Settings *gSettings;

DoubleSpinBox::DoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
}

void DoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QDoubleSpinBox::wheelEvent(event);
}

void DoubleSpinBox::updatePalette()
{
    if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (isReadOnly())
        setPalette(gSettings->palette);
    else
        setPalette(gSettings->paletteInput);
}

void DoubleSpinBox::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QDoubleSpinBox::paintEvent(event);
}

void DoubleSpinBox::setReadOnly(bool r)
{
    QDoubleSpinBox::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
}

void DoubleSpinBox::setError(bool e)
{
    mError = e;
}
