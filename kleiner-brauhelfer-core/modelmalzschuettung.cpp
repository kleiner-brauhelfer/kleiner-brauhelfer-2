#include "modelmalzschuettung.h"
#include "brauhelfer.h"
#include "modelsud.h"

ModelMalzschuettung::ModelMalzschuettung(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
    mVirtualField.append("Extrakt");
    mVirtualField.append("ExtraktProzent");
}

QVariant ModelMalzschuettung::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColExtrakt:
    {
        double swMalz = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::ColSW_Malz).toDouble();
        double p = data(idx.row(), ColProzent).toDouble() / 100;
        return swMalz * p;
    }
    case ColExtraktProzent:
    {
        double sw = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::ColSW).toDouble();
        double extrakt = data(idx.row(), ColExtrakt).toDouble();
        return extrakt / sw * 100;
    }
    default:
        return QVariant();
    }
}

bool ModelMalzschuettung::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            int rowMalz = bh->modelMalz()->getRowWithValue(ModelMalz::ColName, value);
            if (rowMalz >= 0)
            {
                QSqlTableModel::setData(index(idx.row(), ColPotential), bh->modelMalz()->data(rowMalz, ModelMalz::ColPotential));
                QSqlTableModel::setData(index(idx.row(), ColFarbe), bh->modelMalz()->data(rowMalz, ModelMalz::ColFarbe));
                QSqlTableModel::setData(index(idx.row(), ColpH), bh->modelMalz()->data(rowMalz, ModelMalz::ColpH));
            }
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
    case ColExtrakt:
    {
        double swMalz = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::ColSW_Malz).toDouble();
        return setDataExt(index(idx.row(), ColProzent), value.toDouble() / swMalz * 100);
    }
    case ColExtraktProzent:
    {
        double sw = bh->modelSud()->dataSud(data(idx.row(), ColSudID).toInt(), ModelSud::ColSW).toDouble();
        return setDataExt(index(idx.row(), ColExtrakt), value.toDouble() * sw / 100);
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

int ModelMalzschuettung::import(int row)
{
    QMap<int, QVariant> values({{ModelMalz::ColName, data(row, ColName)},
                                {ModelMalz::ColFarbe, data(row, ColFarbe)},
                                {ModelMalz::ColPotential, data(row, ColPotential)},
                                {ModelMalz::ColpH, data(row, ColpH)}});
    return bh->modelMalz()->append(values);
}

void ModelMalzschuettung::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColName))
        values.insert(ColName, bh->modelMalz()->data(0, ModelMalz::ColName));
    if (!values.contains(ColProzent))
        values.insert(ColProzent, 0.0);
}
