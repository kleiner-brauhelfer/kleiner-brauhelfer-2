#include "timeedit.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

TimeEdit::TimeEdit(QWidget *parent) :
    QTimeEdit(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(false);
    connect(this, &QDateTimeEdit::timeChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool TimeEdit::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QTimeEdit::event(event);
}

void TimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void TimeEdit::updatePalette()
{
    if (WidgetDecorator::contains(this))
    {
        if (palette() != gSettings->paletteChanged)
            setPalette(gSettings->paletteChanged);
    }
    else if (mError)
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

void TimeEdit::setEnabled(bool e)
{
    QTimeEdit::setEnabled(e);
    setButtonSymbols(e ? UpDownArrows : NoButtons);
}

void TimeEdit::setDisabled(bool d)
{
    setEnabled(!d);
}

void TimeEdit::setReadOnly(bool r)
{
    QDateTimeEdit::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
}

void TimeEdit::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
