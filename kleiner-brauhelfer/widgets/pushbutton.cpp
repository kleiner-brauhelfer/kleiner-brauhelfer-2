#include "pushbutton.h"
#include "settings.h"

extern Settings *gSettings;

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent),
    WidgetDecorator(),
    mError(false)
{
    connect(this, SIGNAL(clicked(bool)), SLOT(on_valueChanged()));
}

void PushButton::updatePalette()
{
    if (mValueChanged)
        setPalette(gSettings->paletteChanged);
    else if (mError)
        setPalette(gSettings->paletteErrorButton);
    else
        setPalette(gSettings->palette);
}

void PushButton::paintEvent(QPaintEvent *event)
{
    updatePalette();
    QPushButton::paintEvent(event);
}

void PushButton::on_valueChanged()
{
    waValueChanged(this);
    if (mValueChanged)
        repaint();
}

void PushButton::focusOutEvent(QFocusEvent *event)
{
    waFocusOutEvent();
    QPushButton::focusOutEvent(event);
}

void PushButton::setError(bool e)
{
    mError = e;
}
