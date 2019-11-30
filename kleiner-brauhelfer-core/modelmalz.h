#ifndef MODELMALZ_H
#define MODELMALZ_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelMalz : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColBeschreibung,
        ColFarbe,
        ColMaxProzent,
        ColMenge,
        ColPreis,
        ColBemerkung,
        ColAnwendung,
        ColEingelagert,
        ColMindesthaltbar,
        ColLink,
        // virtual
        ColDeleted,
        ColInGebrauch,
        // number of columns
        NumCols
    };

public:

    ModelMalz(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    Brauhelfer* bh;
};

#endif // MODELMALZ_H
