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
    connect(this, &QSpinBox::valueChanged, [this]()
    {
        WidgetDecorator::valueChanged(this, hasFocus());
        updatePalette();
    });
}

bool SpinBox::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QSpinBox::event(event);
}

void SpinBox::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QSpinBox::wheelEvent(event);
}

void SpinBox::updatePalette()
{
    if (WidgetDecorator::contains(this))
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (mError || value() >= mErrorLimitMax || value() <= mErrorLimitMin)
    {
        if (palette() != gSettings->paletteError)
            setPalette(gSettings->paletteError);
    }
    else
    {
        if (palette() != gSettings->palette)
            setPalette(gSettings->palette);
    }
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
        updatePalette();
    }
}

void SpinBox::setErrorRange(int min, int max)
{
    mErrorLimitMin = min;
    mErrorLimitMax = max;
    updatePalette();
}
