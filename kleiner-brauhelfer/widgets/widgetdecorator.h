#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>

class WidgetDecorator
{
public:
    WidgetDecorator();
    static void suspendValueChanged(bool value);

protected:
    void waFocusOutEvent();
    void waValueChanged(bool hasFocus);
    bool mValueChanged;

private:
    void repaintIfChanged(QWidget *wdg);
    static bool mGlobalValueChanged;
    static bool mGlobalSuspendValueChanged;
};

#endif // WIDGETDECORATOR_H
