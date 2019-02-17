#ifndef MODELAUSRUESTUNG_H
#define MODELAUSRUESTUNG_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelAusruestung : public SqlTableModel
{
public:
    ModelAusruestung(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
    QString name(int id) const;
    int id(const QString& name) const;
private:
    QString getUniqueName(const QModelIndex &index, const QVariant &value);
private:
    Brauhelfer* bh;
};

#endif // MODELAUSRUESTUNG_H
