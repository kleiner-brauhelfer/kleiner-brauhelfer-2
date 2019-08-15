#ifndef JSONSQLSTATEMENTMANAGER_H
#define JSONSQLSTATEMENTMANAGER_H

#include <QObject>
#include <QDir>
#include <QDebug>

class JsonSqlStatementManager : public QObject
{
    Q_OBJECT
public:
    explicit JsonSqlStatementManager(QObject *parent = nullptr);

private:
    QString currentPath;
};

#endif // JSONSQLSTATEMENTMANAGER_H
