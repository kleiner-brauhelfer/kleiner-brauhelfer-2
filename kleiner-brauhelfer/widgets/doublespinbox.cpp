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
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (value() >= mErrorLimitMax || value() <= mErrorLimitMin)
        setPalette(gSettings->paletteError);
    else
        setPalette(gSettings->palette);
    QDoubleSpinBox::paintEvent(event);
}

void DoubleSpinBox::setValue(double val)
{
    if (std::fabs(value()-val) >= std::pow(10,-decimals())/2)
        QDoubleSpinBox::setValue(val);
}

void DoubleSpinBox::setEnabled(bool e)
{
    QDoubleSpinBox::setEnabled(e);
    setButtonSymbols(e ? UpDownArrows : NoButtons);
}

void DoubleSpinBox::setDisabled(bool d)
{
    setEnabled(!d);
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
