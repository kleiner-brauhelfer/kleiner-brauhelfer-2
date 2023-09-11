#include "spinbox.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    mError(false),
    mErrorLimitMin(std::numeric_limits<int>::lowest()),
    mErrorLimitMax(std::numeric_limits<int>::max())
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, &QSpinBox::valueChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QSpinBox::wheelEvent(event);
}

void SpinBox::paintEvent(QPaintEvent *event)
{
    if (WidgetDecorator::contains(this))
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
    QSpinBox::paintEvent(event);
}

void SpinBox::setValue(int val)
{
    if (value() != val)
        QSpinBox::setValue(val);
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
