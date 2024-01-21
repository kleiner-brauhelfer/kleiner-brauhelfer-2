#ifndef ICONTHEMED_H
#define ICONTHEMED_H

#include <QIcon>

class IconThemed : public QIcon
{
public:
    explicit IconThemed(const QString &iconName, bool lightTheme);
};

#endif // ICONTHEMED_H
