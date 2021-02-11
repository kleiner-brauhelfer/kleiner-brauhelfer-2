#include "doublespinbox.h"
#include "settings.h"

extern Settings *gSettings;

DoubleSpinBox::DoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    WidgetDecorator(),
    mError(false),
    mErrorOnLimit(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(valueChanged(double)), SLOT(on_valueChanged()));
}

void DoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QDoubleSpinBox::wheelEvent(event);
}

void DoubleSpinBox::updatePalette()
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

void DoubleSpinBox::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QDoubleSpinBox::paintEvent(event);
}

void DoubleSpinBox::on_valueChanged()
{
    waValueChanged(hasFocus());
}

void DoubleSpinBox::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QDoubleSpinBox::focusOutEvent(event);
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

void DoubleSpinBox::setErrorOnLimit(bool e)
{
    mErrorOnLimit = e;
}
