#include "pushbutton.h"
#include "widgetdecorator.h"
#include "settings.h"

extern Settings *gSettings;

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent),
    mError(false)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void PushButton::paintEvent(QPaintEvent *event)
{
    if (WidgetDecorator::contains(this))
        setPalette(gSettings->paletteChanged);
    else if (mError)
        setPalette(gSettings->paletteErrorButton);
    else
        setPalette(gSettings->palette);
    QPushButton::paintEvent(event);
}

void PushButton::setError(bool e)
{
    if (mError != e)
    {
        mError = e;
        update();
    }
}
