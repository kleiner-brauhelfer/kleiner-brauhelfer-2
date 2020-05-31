#ifndef MODELWASSER_H
#define MODELWASSER_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelWasser : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColHydrogencarbonat,
        ColCalcium,
        ColMagnesium,
        ColSufat,
        ColChlorid,
        ColNatrium,
        ColRestalkalitaetAdd,
        ColBemerkung,
        // virtual
        ColDeleted,
        ColHydrogencarbonatMmol,
        ColCalciumMmol,
        ColMagnesiumMmol,
        ColSufatMmol,
        ColChloridMmol,
        ColNatriumMmol,
        ColCarbonatHaerte,
        ColCalciumHaerte,
        ColMagnesiumHaerte,
        ColRestalkalitaet,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelWasser(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    Brauhelfer* bh;
};

#endif // MODELWASSER_H
