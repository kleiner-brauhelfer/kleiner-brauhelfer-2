#include "datetimeedit.h"
#include <QCalendarWidget>
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

DateTimeEdit::DateTimeEdit(QWidget *parent) :
    QDateTimeEdit(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
    connect(this, &QDateTimeEdit::dateTimeChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool DateTimeEdit::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QDateTimeEdit::event(event);
}

void DateTimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void DateTimeEdit::updatePalette()
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

void DateTimeEdit::setEnabled(bool e)
{
    QDateTimeEdit::setEnabled(e);
    setButtonSymbols(e ? UpDownArrows : NoButtons);
    setCalendarPopup(e);
}

void DateTimeEdit::setDisabled(bool d)
{
    setEnabled(!d);
}

void DateTimeEdit::setReadOnly(bool r)
{
    QDateTimeEdit::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
    setCalendarPopup(!r);
}

bool DateTimeEdit::hasFocus() const
{
    return QDateTimeEdit::hasFocus() || (calendarPopup() && calendarWidget()->hasFocus());
}

void DateTimeEdit::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
