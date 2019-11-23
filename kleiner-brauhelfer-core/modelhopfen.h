#ifndef MODELHOPFEN_H
#define MODELHOPFEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelHopfen : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColBeschreibung,
        ColAlpha,
        ColMenge,
        ColPreis,
        ColPellets,
        ColBemerkung,
        ColEigenschaften,
        ColTyp,
        ColEingelagert,
        ColMindesthaltbar,
        ColLink,
        // virtual
        ColDeleted,
        ColInGebrauch
    };

public:

    ModelHopfen(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;

private:

    Brauhelfer* bh;
};

#endif // MODELHOPFEN_H
