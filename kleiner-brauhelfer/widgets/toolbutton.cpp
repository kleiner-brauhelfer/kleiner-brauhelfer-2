#include "toolbutton.h"
#include "widgetdecorator.h"
//#include <QStyle>

ToolButton::ToolButton(QWidget *parent) :
    QToolButton(parent)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void ToolButton::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        //style()->unpolish(this);
        //style()->polish(this);
    }
}
