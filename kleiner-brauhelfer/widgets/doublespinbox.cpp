#include "doublespinbox.h"
#include "widgetdecorator.h"
#include <QStyle>

DoubleSpinBox::DoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    mError(false),
    mErrorLimitMin(std::numeric_limits<double>::lowest()),
    mErrorLimitMax(std::numeric_limits<double>::max())
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, &QDoubleSpinBox::valueChanged, this, &DoubleSpinBox::onValueChanged);
    connect(this, &QDoubleSpinBox::valueChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void DoubleSpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDoubleSpinBox::wheelEvent(event);
}

void DoubleSpinBox::onValueChanged(double val)
{
    bool e = mError || val >= mErrorLimitMax || val <= mErrorLimitMin;
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
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
    mError = e;
    e = mError || value() >= mErrorLimitMax || value() <= mErrorLimitMin;
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void DoubleSpinBox::setErrorRange(double min, double max)
{
    mErrorLimitMin = min;
    mErrorLimitMax = max;
}
