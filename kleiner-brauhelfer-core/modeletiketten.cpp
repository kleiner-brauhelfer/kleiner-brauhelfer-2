#include "modeletiketten.h"
#include "brauhelfer.h"

ModelEtiketten::ModelEtiketten(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db)
{
}

void ModelEtiketten::defaultValues(QMap<int, QVariant> &values) const
{
    int row = -1;
    if (values.contains(ColPfad))
        row = getRowWithValue(ColPfad, values[ColPfad]);
    if (row < 0)
    {
        if (!values.contains(ColAnzahl))
            values.insert(ColAnzahl, 20);
        if (!values.contains(ColBreite))
            values.insert(ColBreite, 100);
        if (!values.contains(ColHoehe))
            values.insert(ColHoehe, 60);
        if (!values.contains(ColAbstandHor))
            values.insert(ColAbstandHor, 2);
        if (!values.contains(ColAbstandVert))
            values.insert(ColAbstandVert, 2);
        if (!values.contains(ColRandOben))
            values.insert(ColRandOben, 10);
        if (!values.contains(ColRandLinks))
            values.insert(ColRandLinks, 5);
        if (!values.contains(ColRandRechts))
            values.insert(ColRandRechts, 5);
        if (!values.contains(ColRandUnten))
            values.insert(ColRandUnten, 15);
    }
    else
    {
        if (!values.contains(ColAnzahl))
            values.insert(ColAnzahl, data(row, ColAnzahl));
        if (!values.contains(ColBreite))
            values.insert(ColBreite, data(row, ColBreite));
        if (!values.contains(ColHoehe))
            values.insert(ColHoehe, data(row, ColHoehe));
        if (!values.contains(ColAbstandHor))
            values.insert(ColAbstandHor, data(row, ColAbstandHor));
        if (!values.contains(ColAbstandVert))
            values.insert(ColAbstandVert, data(row, ColAbstandVert));
        if (!values.contains(ColRandOben))
            values.insert(ColRandOben, data(row, ColRandOben));
        if (!values.contains(ColRandLinks))
            values.insert(ColRandLinks, data(row, ColRandLinks));
        if (!values.contains(ColRandRechts))
            values.insert(ColRandRechts, data(row, ColRandRechts));
        if (!values.contains(ColRandUnten))
            values.insert(ColRandUnten, data(row, ColRandUnten));
    }
}
