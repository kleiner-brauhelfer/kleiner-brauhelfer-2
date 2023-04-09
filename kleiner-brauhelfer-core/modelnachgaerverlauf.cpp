// clazy:excludeall=skipped-base-method
#include "modelnachgaerverlauf.h"
#include "brauhelfer.h"
#include "biercalc.h"

ModelNachgaerverlauf::ModelNachgaerverlauf(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

QVariant ModelNachgaerverlauf::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColZeitstempel:
    {
        return QDateTime::fromString(QSqlTableModel::data(idx).toString(), Qt::ISODate);
    }
    default:
        return QVariant();
    }
}

bool ModelNachgaerverlauf::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColZeitstempel:
    {
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
    }
    case ColDruck:
    case ColTemp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            double p = data(idx.row(), ColDruck).toDouble();
            double T = data(idx.row(), ColTemp).toDouble();
            double co2 = BierCalc::co2(p, T);
            QSqlTableModel::setData(index(idx.row(), ColCO2), co2);
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

int ModelNachgaerverlauf::getLastRow(const QVariant &sudId) const
{
    int row = -1;
    QDateTime lastDt;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColSudID).toInt() == sudId && !data(r, fieldIndex(QStringLiteral("deleted"))).toBool())
        {
            QDateTime dt = data(r, ColZeitstempel).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
            {
                lastDt = dt;
                row = r;
            }
        }
    }
    return row;
}

QDateTime ModelNachgaerverlauf::getLastDateTime(const QVariant &sudId) const
{
    QDateTime lastDt;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColSudID) == sudId)
        {
            QDateTime dt = data(r, ColZeitstempel).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
                lastDt = dt;
        }
    }
    return lastDt;
}

double ModelNachgaerverlauf::getLastCO2(const QVariant &sudId) const
{
    double co2 = 0.0;
    QDateTime lastDt;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColSudID) == sudId)
        {
            QDateTime dt = data(r, ColZeitstempel).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
            {
                lastDt = dt;
                co2 = data(r, ColCO2).toDouble();
            }
        }
    }
    return co2;
}

void ModelNachgaerverlauf::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColZeitstempel))
        values.insert(ColZeitstempel, QDateTime::currentDateTime());
    if (values.contains(ColSudID))
    {
        int id = values.value(ColSudID, -1).toInt();
        int row = getLastRow(id);
        if (row >= 0)
        {
            if (!values.contains(ColDruck))
                values.insert(ColDruck, data(row, ColDruck));
            if (!values.contains(ColTemp))
                values.insert(ColTemp, data(row, ColTemp));
        }
    }
    if (!values.contains(ColDruck))
        values.insert(ColDruck, 0);
    if (!values.contains(ColTemp))
        values.insert(ColTemp, 18);
}
