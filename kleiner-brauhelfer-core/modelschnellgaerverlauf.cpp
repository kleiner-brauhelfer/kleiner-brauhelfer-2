// clazy:excludeall=skipped-base-method
#include "modelschnellgaerverlauf.h"
#include "brauhelfer.h"
#include "biercalc.h"

ModelSchnellgaerverlauf::ModelSchnellgaerverlauf(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append(QStringLiteral("sEVG"));
    mVirtualField.append(QStringLiteral("tEVG"));
}

QVariant ModelSchnellgaerverlauf::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColZeitstempel:
    {
        return QDateTime::fromString(QSqlTableModel::data(idx).toString(), Qt::ISODate);
    }
    case ColsEVG:
    {
        double sw = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColSWIst).toDouble();
        double sre = data(idx.row(), ColRestextrakt).toDouble();
        return BierCalc::vergaerungsgrad(sw, sre);
    }
    case ColtEVG:
    {
        double sw = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColSWIst).toDouble();
        double sre = data(idx.row(), ColRestextrakt).toDouble();
        double tre = BierCalc::toTRE(sw, sre);
        return BierCalc::vergaerungsgrad(sw, tre);
    }
    default:
        return QVariant();
    }
}

bool ModelSchnellgaerverlauf::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColZeitstempel:
    {
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
    }
    case ColRestextrakt:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant sudId = data(idx.row(), ColSudID);
            int row = bh->modelSud()->getRowWithValue(ModelSud::ColID, sudId);
            if (row >= 0)
            {
                double alc = BierCalc::alkohol(bh->modelSud()->data(row, ModelSud::ColSWIst).toDouble(), value.toDouble());
                QSqlTableModel::setData(index(idx.row(), ColAlc), alc);
                if (idx.row() == getLastRow(sudId))
                {
                    bh->modelSud()->setData(row, ModelSud::ColSWSchnellgaerprobe, value);
                }
            }
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

int ModelSchnellgaerverlauf::getLastRow(const QVariant &sudId) const
{
    int row = -1;
    QDateTime lastDt;
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, ColSudID) == sudId && !data(r, fieldIndex(QStringLiteral("deleted"))).toBool())
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

void ModelSchnellgaerverlauf::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColZeitstempel))
        values.insert(ColZeitstempel, QDateTime::currentDateTime());
    if (values.contains(ColSudID))
    {
        int id = values.value(ColSudID).toInt();
        int row = getLastRow(id);
        if (row >= 0)
        {
            if (!values.contains(ColRestextrakt))
                values.insert(ColRestextrakt, data(row, ColRestextrakt));
            if (!values.contains(ColTemp))
                values.insert(ColTemp, data(row, ColTemp));
        }
        else
        {
            if (!values.contains(ColRestextrakt))
            {
                int rowBrew = bh->modelSud()->getRowWithValue(ModelSud::ColID, id);
                if (rowBrew >= 0)
                    values.insert(ColRestextrakt, bh->modelSud()->data(rowBrew, ModelSud::ColSWIst));
            }
        }
    }
    if (!values.contains(ColRestextrakt))
        values.insert(ColRestextrakt, 0);
    if (!values.contains(ColTemp))
        values.insert(ColTemp, 18);
}
