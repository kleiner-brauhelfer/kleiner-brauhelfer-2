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
        int days = dt.daysTo(data(index.row(), "Datum").toDateTime());
        if (days > 0)
            return days / 7 + 1;
        else
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
