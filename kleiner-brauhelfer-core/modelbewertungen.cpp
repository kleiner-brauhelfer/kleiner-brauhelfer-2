#include "modelbewertungen.h"
#include "brauhelfer.h"
#include "modelnachgaerverlauf.h"

ModelBewertungen::ModelBewertungen(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

QVariant ModelBewertungen::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Datum")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Woche")
    {
        QDateTime dt = bh->modelNachgaerverlauf()->getLastDateTime(data(index.row(), "SudID").toInt());
        if (!dt.isValid())
        {
            int sudId = data(index.row(), "SudID").toInt();
            dt = bh->modelSud()->getValueFromSameRow("ID", sudId, "Abfuelldatum").toDateTime();
        }
        if (dt.isValid())
            return dt.daysTo(data(index.row(), "Datum").toDateTime()) / 7 + 1;
        return 0;
    }
    return QVariant();
}

bool ModelBewertungen::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Datum")
    {
        if (QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate)))
        {
            QModelIndex index2 = this->index(index.row(), fieldIndex("Woche"));
            QSqlTableModel::setData(index2, dataExt(index2));
            return true;
        }
    }
    return false;
}

void ModelBewertungen::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Datum"))
        values.insert("Datum", QDateTime::currentDateTime());
    if (!values.contains("Sterne"))
        values.insert("Sterne", 0);
}

int ModelBewertungen::max(int sudId)
{
    int max = -1;
    int colSudId = fieldIndex("SudID");
    int colSterne = fieldIndex("Sterne");
    for (int i = 0; i < rowCount(); i++)
    {
        if (data(index(i, colSudId)).toInt() == sudId)
        {
            int sterne = data(index(i, colSterne)).toInt();
            if (sterne > max)
                max = sterne;
        }
    }
    return max;
}

int ModelBewertungen::mean(int sudId)
{
    int colSudId = fieldIndex("SudID");
    int colSterne = fieldIndex("Sterne");
    int total = 0;
    int n = 0;
    for (int i = 0; i < rowCount(); i++)
    {
        if (data(index(i, colSudId)).toInt() == sudId)
        {
            total += data(index(i, colSterne)).toInt();
            n++;
        }
    }
    if (n != 0)
    {
        return (int)round((double)total / n);
    }
    return 0;
}
