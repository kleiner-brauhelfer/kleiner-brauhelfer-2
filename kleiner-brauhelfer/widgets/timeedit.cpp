#include "timeedit.h"
#include "settings.h"

extern Settings *gSettings;

TimeEdit::TimeEdit(QWidget *parent) :
    QTimeEdit(parent),
    WidgetDecorator(),
    mError(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setAlignment(Qt::AlignCenter);
    connect(this, SIGNAL(timeChanged(QTime)), SLOT(on_valueChanged()));
}

void TimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void TimeEdit::updatePalette()
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

void TimeEdit::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QDateTimeEdit::paintEvent(event);
}

void TimeEdit::on_valueChanged()
{
    waValueChanged(this);
}

void TimeEdit::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QTimeEdit::focusOutEvent(event);
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
        update();
    }
}
