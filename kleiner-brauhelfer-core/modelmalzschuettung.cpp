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

bool ModelMalzschuettung::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant farbe = bh->modelMalz()->getValueFromSameRow(ModelMalz::ColBeschreibung, value, ModelMalz::ColFarbe);
            QSqlTableModel::setData(index(idx.row(), ColFarbe), farbe);
            return true;
        }
        return false;
    }
    case ColProzent:
    {
        double fVal = value.toDouble();
        if (fVal < 0.0)
            fVal = 0.0;
        if (fVal > 100.0)
            fVal = 100.0;
        if (QSqlTableModel::setData(idx, fVal))
        {
            double total = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::Colerg_S_Gesamt).toDouble();
            QSqlTableModel::setData(index(idx.row(), Colerg_Menge), fVal / 100 * total);
            return true;
        }
        return false;
    }
    case Colerg_Menge:
    {
        double total = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::Colerg_S_Gesamt).toDouble();
        return setData(idx.row(), ColProzent, value.toDouble() * 100 / total);
    }
    default:
        return false;
    }
}

void ModelMalzschuettung::onSudDataChanged(const QModelIndex &idx)
{
    if (idx.column() == ModelSud::Colerg_S_Gesamt)
    {
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        for (int r = 0; r < rowCount(); ++r)
        {
            if (data(r, ColSudID) == sudId)
            {
                QModelIndex idx2 = index(r, ColProzent);
                setData(idx2, data(idx2));
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelMalzschuettung::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColName))
        values.insert(ColName, bh->modelMalz()->data(0, ModelMalz::ColBeschreibung));
    if (!values.contains(ColProzent))
        values.insert(ColProzent, 0.0);
}
