#include "datetimeedit.h"
#include <QCalendarWidget>
#include "settings.h"

extern Settings *gSettings;

DateTimeEdit::DateTimeEdit(QWidget *parent) :
    QDateTimeEdit(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
}

void DateTimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QDateTimeEdit::wheelEvent(event);
}

void DateTimeEdit::updatePalette()
{
    if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (isReadOnly())
        setPalette(gSettings->palette);
    else
        setPalette(gSettings->paletteInput);
}

void DateTimeEdit::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QDateTimeEdit::paintEvent(event);
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
    mError = e;
}
