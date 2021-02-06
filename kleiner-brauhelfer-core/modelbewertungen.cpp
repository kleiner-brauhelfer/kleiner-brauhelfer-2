#include "modelbewertungen.h"
#include "brauhelfer.h"

ModelBewertungen::ModelBewertungen(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
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
        QDateTime dt = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColReifungStart).toDateTime();
        if (dt.isValid())
            return dt.daysTo(data(idx.row(), ColDatum).toDateTime()) / 7 + 1;
        return 0;
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
    if (!values.contains(ColDatum))
        values.insert(ColDatum, QDateTime::currentDateTime());
    if (!values.contains(ColSterne))
        values.insert(ColSterne, 0);
}
