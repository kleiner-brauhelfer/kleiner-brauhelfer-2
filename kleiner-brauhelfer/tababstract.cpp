#include "tababstract.h"

TabAbstract::TabAbstract(QWidget *parent) :
    QWidget(parent),
    mIsTabActive(false)
{
}

void TabAbstract::saveSettings()
{
}

void TabAbstract::restoreView()
{
}

void TabAbstract::setTabActive(bool active)
{
    if (mIsTabActive != active)
    {
        mIsTabActive = active;
        if (mIsTabActive)
            onTabActivated();
        else
            onTabDeactivated();
    }
}

bool TabAbstract::isTabActive() const
{
    return mIsTabActive;
}

void TabAbstract::onTabActivated()
{
}

void TabAbstract::onTabDeactivated()
{
    setFocus();
}
