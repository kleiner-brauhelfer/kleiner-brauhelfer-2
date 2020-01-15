#ifndef MODELETIKETTEN_H
#define MODELETIKETTEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelEtiketten : public SqlTableModel
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
    Q_ENUM(Column)

public:

    ModelEtiketten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
};

#endif // MODELETIKETTEN_H
