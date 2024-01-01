#include "timeedit.h"
#include "widgetdecorator.h"
#include <QStyle>

TimeEdit::TimeEdit(QWidget *parent) :
    QTimeEdit(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, &QDateTimeEdit::timeChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void TimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void TimeEdit::setReadOnly(bool r)
{
    QDateTimeEdit::setReadOnly(r);
    setButtonSymbols(r ? NoButtons : UpDownArrows);
}

void TimeEdit::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}
