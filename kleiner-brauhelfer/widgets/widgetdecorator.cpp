#include "widgetdecorator.h"
#include <QPointer>
#include <QStyle>

bool WidgetDecorator::suspendValueChanged = false;
bool WidgetDecorator::suspendValueChangedClear = false;
bool WidgetDecorator::focusRequired = true;
QList<QPointer<QWidget>> WidgetDecorator::valueChangedWidgets = QList<QPointer<QWidget>>();

void WidgetDecorator::valueChanged(QWidget *wdg, bool hasFocus)
{
    if (suspendValueChanged)
        return;
    if (hasFocus || valueChangedWidgets.size() > 0 || !focusRequired)
    {
        if (!contains(wdg))
        {
            wdg->setProperty("ValueChanged", true);
            wdg->style()->unpolish(wdg);
            wdg->style()->polish(wdg);
            valueChangedWidgets.append(QPointer<QWidget>(wdg));
        }
    }
}

bool WidgetDecorator::contains(const QWidget *wdg)
{
    return wdg->property("ValueChanged").toBool();
}

void WidgetDecorator::clearValueChanged()
{
    if (suspendValueChanged || suspendValueChangedClear)
        return;
    for (auto&& wdg : valueChangedWidgets)
    {
        if (!wdg.isNull())
        {
            wdg->setProperty("ValueChanged", false);
            wdg->style()->unpolish(wdg);
            wdg->style()->polish(wdg);
        }
    }
    valueChangedWidgets.clear();
}
