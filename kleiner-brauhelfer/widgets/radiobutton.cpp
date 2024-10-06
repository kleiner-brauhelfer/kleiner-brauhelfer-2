#include "radiobutton.h"
#include "widgetdecorator.h"
//#include <QStyle>

RadioButton::RadioButton(QWidget *parent) :
    QRadioButton(parent)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

RadioButton::RadioButton(const QString &text, QWidget *parent) :
    QRadioButton(text, parent)
{
    connect(this, &QAbstractButton::clicked, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void RadioButton::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        //style()->unpolish(this);
        //style()->polish(this);
    }
}
