#ifndef MODELHOPFEN_H
#define MODELHOPFEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelHopfen : public SqlTableModel
{
public:
    ModelHopfen(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    QString getUniqueName(const QModelIndex &index, const QVariant &value);
private:
    Brauhelfer* bh;
};

#endif // MODELHOPFEN_H
