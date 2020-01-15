#include "spinbox.h"
#include "settings.h"

extern Settings *gSettings;

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    mError(false),
    mErrorOnLimit(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QSpinBox::wheelEvent(event);
}

void SpinBox::updatePalette()
{
    if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError || (mErrorOnLimit && (value() >= maximum() || value() <= minimum())))
        setPalette(gSettings->paletteError);
    else if (isReadOnly())
        setPalette(gSettings->palette);
    else
        setPalette(gSettings->paletteInput);
}

void SpinBox::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QSpinBox::paintEvent(event);
}

void SpinBox::setReadOnly(bool r)
{
    QSpinBox::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
}

void SpinBox::setError(bool e)
{
    mError = e;
}

void SpinBox::setErrorOnLimit(bool e)
{
    mErrorOnLimit = e;
}
