#ifndef MODELHAUPTGAERVERLAUF_H
#define MODELHAUPTGAERVERLAUF_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelHauptgaerverlauf : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColZeitstempel,
        ColSW,
        ColAlc,
        ColTemp,
        // virtual
        ColDeleted,
        ColsEVG,
        ColtEVG
    };

public:

    ModelHauptgaerverlauf(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;

private:

    int getLastRow(int id) const;

private:

    Brauhelfer* bh;
};

#endif // MODELHAUPTGAERVERLAUF_H
