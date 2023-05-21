#include "spinbox.h"
#include "settings.h"

extern Settings *gSettings;

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    WidgetDecorator(),
    mError(false),
    mErrorLimitMin(std::numeric_limits<int>::lowest()),
    mErrorLimitMax(std::numeric_limits<int>::max())
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, &QSpinBox::valueChanged, this, &SpinBox::on_valueChanged);
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QSpinBox::wheelEvent(event);
}

void SpinBox::updatePalette()
{
    if (mValueChanged)
        setPalette(gSettings->paletteChanged);
    else if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (value() >= mErrorLimitMax || value() <= mErrorLimitMin)
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
    waValueChanged(this, hasFocus());
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
    if (mError != e)
    {
        mError = e;
        update();
    }
}

void SpinBox::setErrorRange(int min, int max)
{
    mErrorLimitMin = min;
    mErrorLimitMax = max;
    update();
}
