#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>
#include <QElapsedTimer>

class WidgetDecorator
{
public:
    WidgetDecorator();
    static void suspendValueChanged(bool value);

protected:
    void waFocusOutEvent();
    void waValueChanged(QWidget *wdg);
    bool mValueChanged;

private:
    static void repaintIfChanged(QWidget *wdg);
    static bool mGlobalSuspendValueChanged;
    static QWidget* mGlobalWidget;
    static QElapsedTimer mGlobalTimer;
};

#endif // WIDGETDECORATOR_H
