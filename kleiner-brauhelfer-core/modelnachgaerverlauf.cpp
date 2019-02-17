#include "modelnachgaerverlauf.h"
#include "brauhelfer.h"

ModelNachgaerverlauf::ModelNachgaerverlauf(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

QVariant ModelNachgaerverlauf::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Zeitstempel")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    return QVariant();
}

bool ModelNachgaerverlauf::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Zeitstempel")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    else if (field == "Druck" || field == "Temp")
    {
        if (QSqlTableModel::setData(index, value))
        {
            double p = data(index.row(), "Druck").toDouble();
            double T = data(index.row(), "Temp").toDouble();
            double co2 = BierCalc::co2(p, T);
            setData(index.row(), "CO2", co2);
            return true;
        }
    }
    return false;
}

int ModelNachgaerverlauf::getLastRow(int id) const
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

void ModelNachgaerverlauf::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Zeitstempel"))
        values.insert("Zeitstempel", QDateTime::currentDateTime());
    if (values.contains("SudID"))
    {
        int id = values.value("SudID", -1).toInt();
        int row = getLastRow(id);
        if (row >= 0)
        {
            if (!values.contains("Druck"))
                values.insert("Druck", data(row, "Druck"));
            if (!values.contains("Temp"))
                values.insert("Temp", data(row, "Temp"));
        }
    }
    if (!values.contains("Druck"))
        values.insert("Druck", 0.0);
    if (!values.contains("Temp"))
        values.insert("Temp", 20.0);
}

QDateTime ModelNachgaerverlauf::getLastDateTime(int id) const
{
    QDateTime lastDt;
    int colSudId = fieldIndex("SudID");
    int col = fieldIndex("Zeitstempel");
    for (int i = 0; i < rowCount(); i++)
    {
        if (data(index(i, colSudId)).toInt() == id)
        {
            QDateTime dt = data(index(i, col)).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
                lastDt = dt;
        }
    }
    return lastDt;
}

double ModelNachgaerverlauf::getLastCO2(int id) const
{
    double co2 = 0.0;
    QDateTime lastDt;
    int colSudId = fieldIndex("SudID");
    int col = fieldIndex("Zeitstempel");
    int col2 = fieldIndex("CO2");
    for (int i = 0; i < rowCount(); i++)
    {
        if (data(index(i, colSudId)).toInt() == id)
        {
            QDateTime dt = data(index(i, col)).toDateTime();
            if (!lastDt.isValid() || dt > lastDt)
            {
                lastDt = dt;
                co2 = data(index(i, col2)).toDouble();
            }
        }
    }
    return co2;
}
