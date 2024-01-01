#include "dateedit.h"
#include <QCalendarWidget>
#include "widgetdecorator.h"
#include <QStyle>

DateEdit::DateEdit(QWidget *parent) :
    QDateEdit(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
    connect(this, &QDateTimeEdit::dateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void DateEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateEdit::wheelEvent(event);
}
/*
void DateEdit::paintEvent(QPaintEvent *event)
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
    QDateEdit::paintEvent(event);
}
*/
void DateEdit::setReadOnly(bool r)
{
    QDateEdit::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
    setCalendarPopup(!r);
}

bool DateEdit::hasFocus() const
{
    return QDateEdit::hasFocus() || (calendarPopup() && calendarWidget()->hasFocus());
}

void DateEdit::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}
