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
        ColPapiergroesse,
        ColAusrichtung,
        //ColHintergrundfarbe, // TODO
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelEtiketten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    bool setValuesFrom(int row, const QVariant& pfad);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    int getLastRow(const QVariant& pfad, int excludeRow) const;
};

#endif // MODELETIKETTEN_H
