#include "pushbutton.h"
#include "widgetdecorator.h"
#include <QStyle>

PushButton::PushButton(QWidget *parent) :
    QPushButton(parent)
{
    setAutoDefault(false);
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void PushButton::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        style()->unpolish(this);
        style()->polish(this);
    }
}
