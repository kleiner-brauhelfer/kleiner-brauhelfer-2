#include "widgetdecorator.h"
#include <QPointer>
#include <QApplication>

const QEvent::Type WidgetDecorator::valueChangedEmphasis = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type WidgetDecorator::valueChangedEmphasisLeave = static_cast<QEvent::Type>(QEvent::registerEventType());

QEvent WidgetDecorator::valueChangedEmphasisEvent = QEvent(valueChangedEmphasis);
QEvent WidgetDecorator::valueChangedEmphasisLeaveEvent = QEvent(valueChangedEmphasisLeave);

bool WidgetDecorator::suspendValueChanged = false;
bool WidgetDecorator::suspendValueChangedClear = false;
bool WidgetDecorator::focusRequired = true;
QList<QPointer<QWidget>> WidgetDecorator::valueChangedWidgets = QList<QPointer<QWidget>>();

void WidgetDecorator::valueChanged(QWidget *wdg, bool hasFocus)
{
    if (suspendValueChanged)
        return;
    if (focusRequired && !hasFocus && valueChangedWidgets.empty())
        return;
    if (valueChangedWidgets.contains(wdg))
        return;
    valueChangedWidgets.append(QPointer<QWidget>(wdg));
    qApp->sendEvent(wdg, &valueChangedEmphasisEvent);
}

bool WidgetDecorator::contains(const QWidget *wdg)
{
    return valueChangedWidgets.contains(wdg);
}

void WidgetDecorator::clearValueChanged()
{
    if (suspendValueChanged || suspendValueChangedClear)
        return;
    QList<QPointer<QWidget>> list;
    list.swap(valueChangedWidgets);
    for (auto&& wdg : list)
    {
        if (!wdg.isNull())
            qApp->sendEvent(wdg, &valueChangedEmphasisLeaveEvent);
    }
}
