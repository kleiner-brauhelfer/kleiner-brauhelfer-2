#ifndef MODELMALZ_H
#define MODELMALZ_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMalz : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColMenge,
        ColPotential,
        ColFarbe,
        ColpH,
        ColMaxProzent,
        ColBemerkung,
        ColEingenschaften,
        ColAlternativen,
        ColPreis,
        ColEingelagert,
        ColMindesthaltbar,
        ColLink,
        // virtual
        ColDeleted,
        ColInGebrauch,
        ColInGebrauchListe,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMalz(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    Brauhelfer* bh;
};

#endif // MODELMALZ_H
