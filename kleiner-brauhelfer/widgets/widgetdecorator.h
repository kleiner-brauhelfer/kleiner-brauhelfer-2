#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>
#include <QElapsedTimer>

class WidgetDecorator
{
public:
    WidgetDecorator();
    static bool suspendValueChanged(bool value);
    static bool suspendClear(bool value);

protected:
    void waFocusOutEvent();
    void waValueChanged(QWidget *wdg, bool hasFocus);
    bool mValueChanged;

private:
    static void repaintIfChanged(QWidget *wdg);
    static bool gSuspendValueChanged;
    static bool gSuspendClear;
    static QWidget* gActiveWidget;
    static QElapsedTimer gClearTimer;
};

#endif // WIDGETDECORATOR_H
