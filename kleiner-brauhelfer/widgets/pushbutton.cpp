#include "pushbutton.h"
#include "widgetdecorator.h"
#include <QStyle>

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent)
{
    setAutoDefault(false);
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

/*
void PushButton::paintEvent(QPaintEvent *event)
{
    if (WidgetDecorator::contains(this))
        setPalette(gSettings->paletteChanged);
    else if (mError)
        setPalette(gSettings->paletteErrorButton);
    else
        setPalette(mDefaultPalette);
    QPushButton::paintEvent(event);
}
*/

void PushButton::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}
