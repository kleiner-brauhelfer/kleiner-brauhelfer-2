#ifndef MODELNACHGAERVERLAUF_H
#define MODELNACHGAERVERLAUF_H

#include "sqltablemodel.h"
#include <QDateTime>

class Brauhelfer;

class ModelNachgaerverlauf : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColZeitstempel,
        ColDruck,
        ColTemp,
        ColCO2,
        // virtual
        ColDeleted
    };

public:

    ModelNachgaerverlauf(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
    QDateTime getLastDateTime(int id) const;
    double getLastCO2(int id) const;

private:

    int getLastRow(int id) const;

private:

    Brauhelfer* bh;
};

#endif // MODELNACHGAERVERLAUF_H
