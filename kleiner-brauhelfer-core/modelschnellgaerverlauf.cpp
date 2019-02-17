#include "modelschnellgaerverlauf.h"
#include "brauhelfer.h"

ModelSchnellgaerverlauf::ModelSchnellgaerverlauf(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("sEVG");
    mVirtualField.append("tEVG");
}

QVariant ModelSchnellgaerverlauf::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Zeitstempel")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "sEVG")
    {
        QVariant sudId = index.siblingAtColumn(fieldIndex("SudID")).data();
        double sw = bh->modelSud()->getValueFromSameRow("ID", sudId, "SWIst").toDouble();
        double sre = index.siblingAtColumn(fieldIndex("SW")).data().toDouble();
        return BierCalc::vergaerungsgrad(sw, sre);
    }
    if (field == "tEVG")
    {
        QVariant sudId = index.siblingAtColumn(fieldIndex("SudID")).data();
        double sw = bh->modelSud()->getValueFromSameRow("ID", sudId, "SWIst").toDouble();
        double sre = index.siblingAtColumn(fieldIndex("SW")).data().toDouble();
        double tre = BierCalc::toTRE(sw, sre);
        return BierCalc::vergaerungsgrad(sw, tre);
    }
    return QVariant();
}

bool ModelSchnellgaerverlauf::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Zeitstempel")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    else if (field == "SW")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int id = data(index.row(), "SudID").toInt();
            int row = bh->modelSud()->getRowWithValue("ID", id);
            double alc = BierCalc::alkohol(bh->modelSud()->data(row, "SWIst").toDouble(), value.toDouble());
            setData(index.row(), "Alc", alc);
            if (index.row() == getLastRow(id))
            {
                if (bh->modelSud()->data(row, "BierWurdeGebraut").toBool())
                    bh->modelSud()->setData(row, "SWSchnellgaerprobe", value);
            }
            return true;
        }
    }
    return false;
}

int ModelSchnellgaerverlauf::getLastRow(int id) const
{
    int row = -1;
    int colId = fieldIndex("SudID");
    int colDt = fieldIndex("Zeitstempel");
    QDateTime lastDt;
    for (int i = 0; i < rowCount(); i++)
    {
        if (data(index(i, colId)).toInt() == id)
        {
            QDateTime dt = data(index(i, colDt)).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
            {
                lastDt = dt;
                row = i;
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
                values.insert("SW", data(row, "SW"));
            if (!values.contains("Temp"))
                values.insert("Temp", data(row, "Temp"));
        }
        else
        {
            if (!values.contains("SW"))
            {
                int rowBrew = bh->modelSud()->getRowWithValue("ID", id);
                values.insert("SW", bh->modelSud()->data(rowBrew, "SWIst"));
            }
        }
    }
    if (!values.contains("SW"))
        values.insert("SW", 0.0);
    if (!values.contains("Temp"))
        values.insert("Temp", 20.0);
}
