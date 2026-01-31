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
    setMinimum(std::numeric_limits<int>::lowest());
    setMaximum(std::numeric_limits<int>::max());
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
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (value() >= mErrorLimitMax || value() <= mErrorLimitMin)
        setPalette(gSettings->paletteError);
    else
        setPalette(gSettings->palette);
    QSpinBox::paintEvent(event);
}

void SpinBox::setValue(int val)
{
    if (value() != val)
        QSpinBox::setValue(val);
}

void SpinBox::setEnabled(bool e)
{
    QSpinBox::setEnabled(e);
    setButtonSymbols(e ? UpDownArrows : NoButtons);
}

void SpinBox::setDisabled(bool d)
{
    setEnabled(!d);
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
