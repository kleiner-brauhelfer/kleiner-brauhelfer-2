#ifndef MODELHEFE_H
#define MODELHEFE_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelHefe : public SqlTableModel
{
public:
    ModelHefe(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    Brauhelfer* bh;
};

#endif // MODELHEFE_H
