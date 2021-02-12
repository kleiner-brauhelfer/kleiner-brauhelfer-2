#include "widgetdecorator.h"
#include <QApplication>

bool WidgetDecorator::mGlobalValueChanged = false;
bool WidgetDecorator::mGlobalSuspendValueChanged = false;

WidgetDecorator::WidgetDecorator() :
    mValueChanged(false)
{
}

void WidgetDecorator::suspendValueChanged(bool value)
{
    mGlobalSuspendValueChanged = value;
}

void WidgetDecorator::waValueChanged(bool hasFocus)
{
    if (mGlobalSuspendValueChanged)
        return;
    if (hasFocus)
    {
        mValueChanged = true;
        mGlobalValueChanged = true;
    }
    else if (mGlobalValueChanged)
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
            wdg->repaint();
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
    if (mGlobalValueChanged)
    {
        mValueChanged = false;
        mGlobalValueChanged = false;
        for (auto &it : qApp->topLevelWidgets())
            repaintIfChanged(it);
    }
}
