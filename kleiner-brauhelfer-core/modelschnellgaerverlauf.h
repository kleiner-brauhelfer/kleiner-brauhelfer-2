#ifndef MODELSCHNELLGAERVERLAUF_H
#define MODELSCHNELLGAERVERLAUF_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelSchnellgaerverlauf : public SqlTableModel
{
public:
    ModelSchnellgaerverlauf(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    int getLastRow(int id) const;
private:
    Brauhelfer* bh;
};

#endif // MODELSCHNELLGAERVERLAUF_H
