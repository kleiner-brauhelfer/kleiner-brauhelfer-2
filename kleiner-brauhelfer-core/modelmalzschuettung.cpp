#include "modelmalzschuettung.h"
#include "brauhelfer.h"
#include "modelsud.h"

ModelMalzschuettung::ModelMalzschuettung(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

bool ModelMalzschuettung::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Name")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QVariant farbe = bh->modelMalz()->getValueFromSameRow("Beschreibung", value, "Farbe");
            QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Farbe")), farbe);
            return true;
        }
    }
    else if (field == "Prozent")
    {
        double fVal = value.toDouble();
        if (fVal < 0.0)
            fVal = 0.0;
        if (fVal > 100.0)
            fVal = 100.0;
        if (QSqlTableModel::setData(index, fVal))
        {
            double total = bh->modelSud()->getValueFromSameRow("ID", data(index.row(), "SudID").toInt(), "erg_S_Gesammt").toDouble();
            QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("erg_Menge")), fVal / 100 * total);
            return true;
        }
    }
    else if (field == "erg_Menge")
    {
        double fVal = value.toDouble();
        if (fVal < 0.0)
            fVal = 0.0;
        if (QSqlTableModel::setData(index, fVal))
        {
            double total = bh->modelSud()->getValueFromSameRow("ID", data(index.row(), "SudID").toInt(), "erg_S_Gesammt").toDouble();
            QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Prozent")), fVal * 100 / total);
            return true;
        }
    }
    return false;
}

void ModelMalzschuettung::onSudDataChanged(const QModelIndex &index)
{
    QString field = bh->modelSud()->fieldName(index.column());
    if (field == "erg_S_Gesammt")
    {
        int sudId = bh->modelSud()->data(index.row(), "ID").toInt();
        double total = index.data().toDouble();
        int colSudId = fieldIndex("SudID");
        int colProzent = fieldIndex("Prozent");
        int colMenge = fieldIndex("erg_Menge");
        for (int i = 0; i < rowCount(); ++i)
        {
            if (this->index(i, colSudId).data().toInt() == sudId)
            {
                double p = this->index(i, colProzent).data().toDouble();
                QSqlTableModel::setData(this->index(i, colMenge), p / 100 * total);
            }
        }
    }
}

void ModelMalzschuettung::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Name"))
        values.insert("Name", bh->modelMalz()->data(0, "Beschreibung"));
    if (!values.contains("Prozent"))
        values.insert("Prozent", 0.0);
}
