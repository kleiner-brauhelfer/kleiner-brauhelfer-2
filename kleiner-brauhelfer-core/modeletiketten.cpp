// clazy:excludeall=skipped-base-method
#include "modeletiketten.h"
#include "brauhelfer.h"

ModelEtiketten::ModelEtiketten(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db)
{
}

int ModelEtiketten::getLastRow(const QVariant& pfad, int excludeRow) const
{
    for (int row = rowCount() - 1; row >= 0; --row)
    {
        if (row == excludeRow)
            continue;
        if (data(row, ColPfad) == pfad && !data(row, fieldIndex(QStringLiteral("deleted"))).toBool())
            return row;
    }
    return -1;
}

bool ModelEtiketten::setValuesFrom(int row, const QVariant& pfad)
{
    int rowFrom = getLastRow(pfad, row);
    if (row >= 0 && rowFrom >= 0)
    {
        setData(row, ColBreite, data(rowFrom, ColBreite));
        setData(row, ColHoehe, data(rowFrom, ColHoehe));
        setData(row, ColAbstandHor, data(rowFrom, ColAbstandHor));
        setData(row, ColAbstandVert, data(rowFrom, ColAbstandVert));
        setData(row, ColRandOben, data(rowFrom, ColRandOben));
        setData(row, ColRandLinks, data(rowFrom, ColRandLinks));
        setData(row, ColRandRechts, data(rowFrom, ColRandRechts));
        setData(row, ColRandUnten, data(rowFrom, ColRandUnten));
        setData(row, ColPapiergroesse, data(rowFrom, ColPapiergroesse));
        setData(row, ColAusrichtung, data(rowFrom, ColAusrichtung));
        return true;
    }
    return false;
}

void ModelEtiketten::defaultValues(QMap<int, QVariant> &values) const
{
    int row = -1;
    if (values.contains(ColPfad))
        row = getLastRow(values[ColPfad], -1);
    if (row < 0)
    {
        if (!values.contains(ColAnzahl))
            values.insert(ColAnzahl, 20);
        if (!values.contains(ColBreite))
            values.insert(ColBreite, 0);
        if (!values.contains(ColHoehe))
            values.insert(ColHoehe, 0);
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
        if (!values.contains(ColPapiergroesse))
            values.insert(ColPapiergroesse, 0);
        if (!values.contains(ColAusrichtung))
            values.insert(ColAusrichtung, 0);
        if (!values.contains(ColHintergrundfarbe))
            values.insert(ColHintergrundfarbe, 0xffffff);
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
        if (!values.contains(ColPapiergroesse))
            values.insert(ColPapiergroesse, data(row, ColPapiergroesse));
        if (!values.contains(ColAusrichtung))
            values.insert(ColAusrichtung, data(row, ColAusrichtung));
        if (!values.contains(ColHintergrundfarbe))
            values.insert(ColHintergrundfarbe, data(row, ColHintergrundfarbe));
    }
}
