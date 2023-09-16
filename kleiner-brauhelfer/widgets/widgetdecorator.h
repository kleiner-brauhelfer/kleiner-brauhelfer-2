#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>

class WidgetDecorator
{
public:
    static void valueChanged(QWidget *wdg, bool hasFocus);
    static void clearValueChanged();
    static bool contains(const QWidget *wdg);

    static bool suspendValueChanged;
    static bool suspendValueChangedClear;
    static bool focusRequired;
    static QList<QPointer<QWidget>> valueChangedWidgets;
};

#endif // WIDGETDECORATOR_H
