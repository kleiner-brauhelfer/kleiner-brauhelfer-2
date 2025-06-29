// clazy:excludeall=skipped-base-method
#include "modelbewertungen.h"
#include "brauhelfer.h"

ModelBewertungen::ModelBewertungen(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append(QStringLiteral("SudName"));
}

QVariant ModelBewertungen::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColDatum:
    {
        return QDateTime::fromString(QSqlTableModel::data(idx).toString(), Qt::ISODate);
    }
    case ColWoche:
    {
        QVariant sudID = data(idx.row(), ColSudID);
        QDateTime dt = bh->modelSud()->dataSud(sudID, ModelSud::ColReifungStart).toDateTime();
        if (dt.isValid())
            return dt.daysTo(data(idx.row(), ColDatum).toDateTime()) / 7 + 1;
        return 0;
    }
    case ColSudName:
    {
        QVariant sudID = data(idx.row(), ColSudID);
        return bh->modelSud()->dataSud(sudID, ModelSud::ColSudname).toString();
    }
    default:
        return QVariant();
    }
}

bool ModelBewertungen::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColDatum:
    {
        if (QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate)))
        {
            QModelIndex idx2 = index(idx.row(), ColWoche);
            QSqlTableModel::setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

int ModelBewertungen::max(const QVariant &sudId)
{
    int max = -1;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColDeleted).toBool())
            continue;
        if (data(r, ColSudID) == sudId)
        {
            int sterne = data(r, ColSterne).toInt();
            if (sterne > max)
                max = sterne;
        }
    }
    return max;
}

int ModelBewertungen::mean(const QVariant &sudId)
{
    int total = 0, n = 0;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColDeleted).toBool())
            continue;
        if (data(r, ColSudID) == sudId)
        {
            total += data(r, ColSterne).toInt();
            n++;
        }
    }
    if (n != 0)
    {
        return qRound(static_cast<double>(total)/n);
    }
    return -1;
}

void ModelBewertungen::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColID))
        values[ColID] = getNextId();
    if (!values.contains(ColDatum))
        values.insert(ColDatum, QDateTime::currentDateTime());
    if (!values.contains(ColSterne))
        values.insert(ColSterne, 0);
}
