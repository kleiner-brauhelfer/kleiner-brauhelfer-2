#ifndef MODELWEITEREZUTATENGABEN_H
#define MODELWEITEREZUTATENGABEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelWeitereZutatenGaben : public SqlTableModel
{
    Q_OBJECT
public:
    ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private slots:
    void onSudDataChanged(const QModelIndex &index);
private:
    Brauhelfer* bh;
};

#endif // MODELWEITEREZUTATENGABEN_H
