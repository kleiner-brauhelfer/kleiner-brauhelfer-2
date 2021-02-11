#include "dateedit.h"
#include <QCalendarWidget>
#include "settings.h"

extern Settings *gSettings;

DateEdit::DateEdit(QWidget *parent) :
    QDateEdit(parent),
    WidgetDecorator(),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    setCalendarPopup(true);
    connect(this, SIGNAL(dateChanged(const QDate&)), SLOT(on_valueChanged()));
}

void DateEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        return QDateEdit::wheelEvent(event);
}

void DateEdit::updatePalette()
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

void DateEdit::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QDateEdit::paintEvent(event);
}

void DateEdit::on_valueChanged()
{
    waValueChanged(hasFocus());
}

void DateEdit::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QDateEdit::focusOutEvent(event);
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
    mError = e;
}
