#ifndef WIDGETDECORATOR_H
#define WIDGETDECORATOR_H

#include <QWidget>
#include <QEvent>

class WidgetDecorator
{
public:
    static const QEvent::Type valueChangedEmphasis;
    static const QEvent::Type valueChangedEmphasisLeave;

    static void valueChanged(QWidget *wdg, bool hasFocus);
    static void clearValueChanged();
    static bool contains(const QWidget *wdg);

    static bool suspendValueChanged;
    static bool suspendValueChangedClear;
    static bool focusRequired;
    static QList<QPointer<QWidget>> valueChangedWidgets;

private:
    static QEvent valueChangedEmphasisEvent;
    static QEvent valueChangedEmphasisLeaveEvent;
};

#endif // WIDGETDECORATOR_H
