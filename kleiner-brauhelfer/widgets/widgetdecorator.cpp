#include "widgetdecorator.h"
#include <QApplication>

bool WidgetDecorator::gSuspendValueChanged = false;
bool WidgetDecorator::gSuspendClear = false;
QWidget* WidgetDecorator::gActiveWidget = nullptr;
QElapsedTimer WidgetDecorator::gClearTimer;

WidgetDecorator::WidgetDecorator() :
    mValueChanged(false)
{
}

bool WidgetDecorator::suspendValueChanged(bool value)
{
    bool prevValue = gSuspendValueChanged;
    gSuspendValueChanged = value;
    return prevValue;
}

bool WidgetDecorator::suspendClear(bool value)
{
    bool prevValue = gSuspendClear;
    gSuspendClear = value;
    return prevValue;
}

void WidgetDecorator::waValueChanged(QWidget *wdg, bool hasFocus)
{
    if (gSuspendValueChanged)
        return;
    if (hasFocus)
    {
        if (wdg != gActiveWidget)
        {
            for (auto &it : qApp->topLevelWidgets())
                repaintIfChanged(it);
            gActiveWidget = wdg;
            gClearTimer.start();
        }
        mValueChanged = true;
    }
    else if (gActiveWidget)
    {
        mValueChanged = true;
    }
}

void WidgetDecorator::repaintIfChanged(QWidget *wdg)
{
    if (!wdg)
        return;
    WidgetDecorator* wa = dynamic_cast<WidgetDecorator*>(wdg);
    if (wa)
    {
        if (wa->mValueChanged)
        {
            wa->mValueChanged = false;
            wdg->update();
        }
    }
    for (int i = 0; i < wdg->children().size(); ++i)
    {
        QWidget* w = qobject_cast<QWidget *>(wdg->children().at(i));
        if (w)
            repaintIfChanged(w);
    }
}

void WidgetDecorator::waFocusOutEvent()
{
    if (gSuspendClear)
        return;
    if (gSuspendValueChanged)
        return;
    if (gClearTimer.elapsed() < 100)
        return;
    if (gActiveWidget)
    {
        mValueChanged = false;
        gActiveWidget = nullptr;
        for (auto &it : qApp->topLevelWidgets())
            repaintIfChanged(it);
    }
}
