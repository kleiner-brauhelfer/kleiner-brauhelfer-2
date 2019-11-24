#ifndef PROXYMODELSUDCOLORED_H
#define PROXYMODELSUDCOLORED_H

#include "proxymodelsud.h"

class ProxyModelSudColored : public ProxyModelSud
{
    Q_OBJECT

public:
    ProxyModelSudColored(QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // PROXYMODELSUDCOLORED_H
