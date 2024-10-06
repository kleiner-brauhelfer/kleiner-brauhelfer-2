#include "lineedit.h"
#include "widgetdecorator.h"
//#include <QStyle>

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    connect(this, &QLineEdit::textChanged, [this](){WidgetDecorator::valueChanged(this, hasFocus());});
}

void LineEdit::setError(bool e)
{
    if (property("ErrorState").toBool() != e)
    {
        setProperty("ErrorState", e);
        //style()->unpolish(this);
        //style()->polish(this);
    }
}

