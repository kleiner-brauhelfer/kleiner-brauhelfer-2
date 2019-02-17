#ifndef MODELFLASCHENLABELTAGS_H
#define MODELFLASCHENLABELTAGS_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelFlaschenlabelTags : public SqlTableModel
{
public:
    ModelFlaschenlabelTags(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
    bool isUnique(const QModelIndex &index, const QVariant &value, int sudId);
    QString getUniqueName(const QModelIndex &index, const QVariant &value);
private:
    Brauhelfer* bh;
};

#endif // MODELFLASCHENLABELTAGS_H
