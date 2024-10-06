#include "checkbox.h"
#include "widgetdecorator.h"
//#include <QStyle>

CheckBox::CheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    connect(this, &QCheckBox::stateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

CheckBox::CheckBox(const QString &text, QWidget *parent) :
    QCheckBox(text, parent)
{
    connect(this, &QCheckBox::stateChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void CheckBox::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        //style()->unpolish(this);
        //style()->polish(this);
    }
}
