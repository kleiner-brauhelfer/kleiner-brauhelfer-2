#include "dateedit.h"
#include <QCalendarWidget>
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

DateEdit::DateEdit(QWidget *parent) :
    QDateEdit(parent),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
    connect(this, &QDateTimeEdit::dateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

bool DateEdit::event(QEvent *event)
{
    if (event->type() == WidgetDecorator::valueChangedEmphasis || event->type() == WidgetDecorator::valueChangedEmphasisLeave)
    {
        updatePalette();
        return true;
    }
    return QDateEdit::event(event);
}

void DateEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateEdit::wheelEvent(event);
}

void DateEdit::updatePalette()
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

void DateEdit::setEnabled(bool e)
{
    QDateEdit::setEnabled(e);
    setButtonSymbols(e ? UpDownArrows : NoButtons);
    setCalendarPopup(e);
}

void DateEdit::setDisabled(bool d)
{
    setEnabled(!d);
}

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
    if (mError != e)
    {
        mError = e;
        updatePalette();
    }
}
