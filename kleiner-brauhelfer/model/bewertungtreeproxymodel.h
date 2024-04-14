#ifndef BEWERTUNGTREEPROXYMODEL_H
#define BEWERTUNGTREEPROXYMODEL_H

#include "treeproxymodel.h"

class BewertungTreeProxyModel : public TreeProxyModel
{
    Q_OBJECT

public:
    explicit BewertungTreeProxyModel(QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // BEWERTUNGTREEPROXYMODEL_H
