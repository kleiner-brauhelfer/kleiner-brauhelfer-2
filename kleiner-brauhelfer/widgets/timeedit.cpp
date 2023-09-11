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
    connect(this, &QDateTimeEdit::timeChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void TimeEdit::wheelEvent(QWheelEvent *event)
{
    if (hasFocus())
        QDateTimeEdit::wheelEvent(event);
}

void TimeEdit::paintEvent(QPaintEvent *event)
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
