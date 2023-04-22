#include "datetimeedit.h"
#include <QCalendarWidget>
#include "settings.h"

extern Settings *gSettings;

DateTimeEdit::DateTimeEdit(QWidget *parent) :
    QDateTimeEdit(parent),
    WidgetDecorator(),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
    connect(this, &QDateTimeEdit::dateTimeChanged, this, &DateTimeEdit::on_valueChanged);
}

void DateTimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void DateTimeEdit::updatePalette()
{
    if (mValueChanged)
        setPalette(gSettings->paletteChanged);
    else if (!isEnabled())
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

void DateTimeEdit::on_valueChanged()
{
    waValueChanged(this, hasFocus());
}

void DateTimeEdit::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QDateTimeEdit::focusOutEvent(event);
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
        update();
    }
}
