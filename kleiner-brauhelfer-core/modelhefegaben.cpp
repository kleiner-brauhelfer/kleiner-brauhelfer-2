#include "modelhefegaben.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelHefegaben::ModelHefegaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("ZugabeDatum");
    mVirtualField.append("Abfuellbereit");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelHefegaben::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "ZugabeDatum")
    {
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
            return braudatum.addDays(data(index.row(), "ZugabeNach").toInt());
        return QDateTime();
    }
    if (field == "Abfuellbereit")
    {
        return data(index.row(), "Zugegeben").toBool();
    }
    return QVariant();
}

bool ModelHefegaben::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "ZugabeDatum")
    {
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
            return QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("ZugabeNach")), braudatum.daysTo(value.toDateTime()));
    }
    return false;
}

void ModelHefegaben::onSudDataChanged(const QModelIndex &idx)
{
    QString field = bh->modelSud()->fieldName(idx.column());
    if (field == "Status")
    {
        int status = idx.data().toInt();
        int sudId = bh->modelSud()->data(idx.row(), "ID").toInt();
        int colSudId = fieldIndex("SudID");
        int colZugegeben = fieldIndex("Zugegeben");
        int colZugabeNach = fieldIndex("ZugabeNach");
        mSignalModifiedBlocked = true;
        if (status == Sud_Status_Rezept)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(index(row, colSudId)).toInt() == sudId)
                    setData(index(row, colZugegeben), false);
            }
        }
        else if (status == Sud_Status_Gebraut)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(index(row, colSudId)).toInt() == sudId &&
                    data(index(row, colZugabeNach)).toInt() == 0)
                    setData(index(row, colZugegeben), true);
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelHefegaben::defaultValues(QVariantMap &values) const
{
    if (values.contains("SudID"))
    {
        QVariant sudId = values.value("SudID");
        if (!values.contains("ZugabeNach"))
        {
            if (bh->modelSud()->getValueFromSameRow("ID", sudId, "Status").toInt() != Sud_Status_Rezept)
            {
                QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
                if (braudatum.isValid())
                    values.insert("ZugabeNach", braudatum.daysTo(QDateTime::currentDateTime()));
            }
        }
    }
    if (!values.contains("Name"))
        values.insert("Name", bh->modelHefe()->data(0, "Beschreibung"));
    if (!values.contains("Menge"))
        values.insert("Menge", 1);
    if (!values.contains("Zugegeben"))
        values.insert("Zugegeben", 0);
    if (!values.contains("ZugabeNach"))
        values.insert("ZugabeNach", 0);
}
