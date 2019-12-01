#ifndef MODELETIKETTEN_H
#define MODELETIKETTEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelEtiketten : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColPfad,
        ColAnzahl,
        ColBreite,
        ColHoehe,
        ColAbstandHor,
        ColAbstandVert,
        ColRandOben,
        ColRandLinks,
        ColRandRechts,
        ColRandUnten,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };

public:

    ModelEtiketten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
};

#endif // MODELETIKETTEN_H
