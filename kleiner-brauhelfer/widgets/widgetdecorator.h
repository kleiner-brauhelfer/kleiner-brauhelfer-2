#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>

class WidgetDecorator
{
public:
    WidgetDecorator();

protected:
    void waFocusOutEvent();
    void waValueChanged(bool hasFocus);
    bool mValueChanged;

private:
    void repaintIfChanged(QWidget *wdg);
    static bool mGlobalValueChanged;

};

#endif // WIDGETDECORATOR_H
