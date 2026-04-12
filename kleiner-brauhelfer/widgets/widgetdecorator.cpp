#include "widgetdecorator.h"
#include <QPointer>
#include <QApplication>
#include "checkbox.h"
#include "combobox.h"
#include "dateedit.h"
#include "datetimeedit.h"
#include "doublespinbox.h"
#include "lineedit.h"
#include "pushbutton.h"
#include "radiobutton.h"
#include "spinbox.h"
#include "timeedit.h"
#include "toolbutton.h"

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

void WidgetDecorator::addDecortor(QList<QWidget*> list)
{
    for (QWidget *w : list)
    {
        if (CheckBox *o = qobject_cast<CheckBox*>(w))
            o->addChangeDecorator();
        else if (ComboBox *o = qobject_cast<ComboBox*>(w))
            o->addChangeDecorator();
        else if (DateEdit *o = qobject_cast<DateEdit*>(w))
            o->addChangeDecorator();
        else if (DateTimeEdit *o = qobject_cast<DateTimeEdit*>(w))
            o->addChangeDecorator();
        else if (DoubleSpinBox *o = qobject_cast<DoubleSpinBox*>(w))
            o->addChangeDecorator();
        else if (LineEdit *o = qobject_cast<LineEdit*>(w))
            o->addChangeDecorator();
        else if (PushButton *o = qobject_cast<PushButton*>(w))
            o->addChangeDecorator();
        else if (RadioButton *o = qobject_cast<RadioButton*>(w))
            o->addChangeDecorator();
        else if (SpinBox *o = qobject_cast<SpinBox*>(w))
            o->addChangeDecorator();
        else if (TimeEdit *o = qobject_cast<TimeEdit*>(w))
            o->addChangeDecorator();
        else if (ToolButton *o = qobject_cast<ToolButton*>(w))
            o->addChangeDecorator();
    }
}
