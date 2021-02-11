#include "spinbox.h"
#include "settings.h"

extern Settings *gSettings;

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    WidgetDecorator(),
    mError(false),
    mErrorOnLimit(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(valueChanged(int)), SLOT(on_valueChanged()));
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QSpinBox::wheelEvent(event);
}

void SpinBox::updatePalette()
{
    if (mValueChanged)
        setPalette(gSettings->paletteChanged);
    else if (!isEnabled())
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

void SpinBox::on_valueChanged()
{
    waValueChanged(hasFocus());
}

void SpinBox::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QSpinBox::focusOutEvent(event);
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
