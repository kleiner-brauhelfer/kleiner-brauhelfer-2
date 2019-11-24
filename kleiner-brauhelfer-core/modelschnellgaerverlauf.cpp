#include "modelschnellgaerverlauf.h"
#include "brauhelfer.h"

ModelSchnellgaerverlauf::ModelSchnellgaerverlauf(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("sEVG");
    mVirtualField.append("tEVG");
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
        double sre = data(idx.row(), ColSW).toDouble();
        return BierCalc::vergaerungsgrad(sw, sre);
    }
    case ColtEVG:
    {
        double sw = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColSWIst).toDouble();
        double sre = data(idx.row(), ColSW).toDouble();
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
    case ColSW:
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
        if (data(r, ColSudID) == sudId)
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

void ModelSchnellgaerverlauf::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Zeitstempel"))
        values.insert("Zeitstempel", QDateTime::currentDateTime());
    if (values.contains("SudID"))
    {
        int id = values.value("SudID").toInt();
        int row = getLastRow(id);
        if (row >= 0)
        {
            if (!values.contains("SW"))
                values.insert("SW", data(row, ColSW));
            if (!values.contains("Temp"))
                values.insert("Temp", data(row, ColTemp));
        }
        else
        {
            if (!values.contains("SW"))
            {
                int rowBrew = bh->modelSud()->getRowWithValue(ModelSud::ColID, id);
                if (rowBrew >= 0)
                    values.insert("SW", bh->modelSud()->data(rowBrew, ModelSud::ColSWIst));
            }
        }
    }
    if (!values.contains("SW"))
        values.insert("SW", 0.0);
    if (!values.contains("Temp"))
        values.insert("Temp", 20.0);
}
