#include "jsonsqlstatementmanager.h"

JsonSqlStatementManager::JsonSqlStatementManager(QObject *parent) : QObject(parent),
    currentPath(QDir::currentPath())
{
    qDebug() << "currentPth: " << currentPath;
}
