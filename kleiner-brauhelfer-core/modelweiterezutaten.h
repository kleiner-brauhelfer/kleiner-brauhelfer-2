#ifndef MODELWEITEREZUTATEN_H
#define MODELWEITEREZUTATEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelWeitereZutaten : public SqlTableModel
{
public:
    ModelWeitereZutaten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    Brauhelfer* bh;
};

#endif // MODELWEITEREZUTATEN_H
