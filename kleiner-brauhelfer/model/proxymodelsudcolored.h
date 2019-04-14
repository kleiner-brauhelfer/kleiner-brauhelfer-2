#ifndef PROXYMODELSUDCOLORED_H
#define PROXYMODELSUDCOLORED_H

#include "proxymodelsud.h"

class ProxyModelSudColored : public ProxyModelSud
{
public:
    ProxyModelSudColored(QObject* parent = nullptr);
    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    int mColStatus;
    int mColMerklistenID;
};

#endif // PROXYMODELSUDCOLORED_H
