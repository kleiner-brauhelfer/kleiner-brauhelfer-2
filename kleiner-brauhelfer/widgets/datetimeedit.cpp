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

void DateTimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void DateTimeEdit::paintEvent(QPaintEvent *event)
{
    if (WidgetDecorator::contains(this))
        setPalette(gSettings->paletteChanged);
    else if (!isEnabled())
        setPalette(gSettings->palette);
    else if (mError)
        setPalette(gSettings->paletteError);
    else if (isReadOnly())
        setPalette(gSettings->palette);
    else
        setPalette(gSettings->paletteInput);
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
    if (mError != e)
    {
        mError = e;
        update();
    }
}
