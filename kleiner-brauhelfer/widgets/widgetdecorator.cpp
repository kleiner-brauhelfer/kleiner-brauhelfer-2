#include "widgetdecorator.h"
#include <QApplication>

bool WidgetDecorator::mGlobalSuspendValueChanged = false;
QWidget* WidgetDecorator::mGlobalWidget = nullptr;
QElapsedTimer WidgetDecorator::mGlobalTimer;

WidgetDecorator::WidgetDecorator() :
    mValueChanged(false)
{
}

bool WidgetDecorator::suspendValueChanged(bool value)
{
    bool prevValue = mGlobalSuspendValueChanged;
    mGlobalSuspendValueChanged = value;
    return prevValue;
}

void WidgetDecorator::waValueChanged(QWidget *wdg, bool hasFocus)
{
    if (mGlobalSuspendValueChanged)
        return;
    if (hasFocus)
    {
        if (wdg != mGlobalWidget)
        {
            for (auto &it : qApp->topLevelWidgets())
                repaintIfChanged(it);
            mGlobalWidget = wdg;
            mGlobalTimer.start();
        }
        mValueChanged = true;
    }
    else if (mGlobalWidget)
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
    if (mGlobalSuspendValueChanged)
        return;
    if (mGlobalTimer.elapsed() < 100)
        return;
    if (mGlobalWidget)
    {
        mValueChanged = false;
        mGlobalWidget = nullptr;
        for (auto &it : qApp->topLevelWidgets())
            repaintIfChanged(it);
    }
}
