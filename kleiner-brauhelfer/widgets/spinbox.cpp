#include "spinbox.h"
#include "widgetdecorator.h"
#include <QStyle>

SpinBox::SpinBox(QWidget *parent) :
    QSpinBox(parent),
    mError(false),
    mErrorLimitMin(std::numeric_limits<int>::lowest()),
    mErrorLimitMax(std::numeric_limits<int>::max())
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, &QSpinBox::valueChanged, this, &SpinBox::onValueChanged);
    connect(this, &QSpinBox::valueChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QSpinBox::wheelEvent(event);
}

void SpinBox::onValueChanged(int val)
{
    bool e = mError || val >= mErrorLimitMax || val <= mErrorLimitMin;
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
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
    mError = e;
    e = mError || value() >= mErrorLimitMax || value() <= mErrorLimitMin;
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void SpinBox::setErrorRange(int min, int max)
{
    mErrorLimitMin = min;
    mErrorLimitMax = max;
}
