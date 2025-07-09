#include "doublespinbox.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

DoubleSpinBox::DoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    mError(false),
    mErrorLimitMin(std::numeric_limits<double>::lowest()),
    mErrorLimitMax(std::numeric_limits<double>::max())
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setMinimum(std::numeric_limits<double>::lowest());
    setMaximum(std::numeric_limits<double>::max());
    connect(this, &QDoubleSpinBox::valueChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void DoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDoubleSpinBox::wheelEvent(event);
}

void DoubleSpinBox::paintEvent(QPaintEvent *event)
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
    QDoubleSpinBox::paintEvent(event);
}

void DoubleSpinBox::setValue(double val)
{
    if (std::fabs(value()-val) >= std::pow(10,-decimals())/2)
        QDoubleSpinBox::setValue(val);
}

void DoubleSpinBox::setReadOnly(bool r)
{
    QDoubleSpinBox::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
}

void DoubleSpinBox::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        update();
    }
}

void DoubleSpinBox::setErrorRange(double min, double max)
{
    mErrorLimitMin = min;
    mErrorLimitMax = max;
    update();
}
