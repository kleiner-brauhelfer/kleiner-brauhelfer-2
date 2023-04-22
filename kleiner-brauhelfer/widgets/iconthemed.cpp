#include "iconthemed.h"

IconThemed::IconThemed(const QString &iconName, bool lightTheme) :
    QIcon()
{
    if (lightTheme)
        addFile(QStringLiteral(":/images/light/%1.svg").arg(iconName), QSize(), Normal, Off);
    else
        addFile(QStringLiteral(":/images/dark/%1.svg").arg(iconName), QSize(), Normal, Off);
    addFile(QStringLiteral(":/images/disabled/%1.svg").arg(iconName), QSize(), Disabled, Off);
}
