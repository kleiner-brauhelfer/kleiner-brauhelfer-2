#ifndef MODELWASSER_H
#define MODELWASSER_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelWasser : public SqlTableModel
{
public:
    ModelWasser(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    Brauhelfer* bh;
};

#endif // MODELWASSER_H
