#include "iconthemed.h"

IconThemed::IconThemed(const QString &iconName, bool lightTheme) :
    QIcon()
{
    if (lightTheme)
        addFile(QString(":/images/light/%1.svg").arg(iconName), QSize(), Normal, Off);
    else
        addFile(QString(":/images/dark/%1.svg").arg(iconName), QSize(), Normal, Off);
    addFile(QString(":/images/disabled/%1.svg").arg(iconName), QSize(), Disabled, Off);
}
