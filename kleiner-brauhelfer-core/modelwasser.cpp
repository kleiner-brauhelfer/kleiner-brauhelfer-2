#include "modelwasser.h"
#include "brauhelfer.h"

ModelWasser::ModelWasser(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("HydrogencarbonatMmol");
    mVirtualField.append("CalciumMmol");
    mVirtualField.append("MagnesiumMmol");
    mVirtualField.append("SulfatMmol");
    mVirtualField.append("ChloridMmol");
    mVirtualField.append("NatriumMmol");
    mVirtualField.append("CarbonatHaerte");
    mVirtualField.append("CalciumHaerte");
    mVirtualField.append("MagnesiumHaerte");
    mVirtualField.append("Restalkalitaet");
}

QVariant ModelWasser::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColHydrogencarbonatMmol:
        return data(idx.row(), ColHydrogencarbonat).toDouble() / 61.02;
    case ColCalciumMmol:
        return data(idx.row(), ColCalcium).toDouble() / 40.08;
    case ColMagnesiumMmol:
        return data(idx.row(), ColMagnesium).toDouble() / 24.31;
    case ColSulfatMmol:
        return data(idx.row(), ColSulfat).toDouble() / 96.06;
    case ColChloridMmol:
        return data(idx.row(), ColChlorid).toDouble() / 35.45;
    case ColNatriumMmol:
        return data(idx.row(), ColNatrium).toDouble() / 22.99;
    case ColCalciumHaerte:
        return data(idx.row(), ColCalcium).toDouble() / (40.08 * 0.1783);
    case ColMagnesiumHaerte:
        return data(idx.row(), ColMagnesium).toDouble() / (24.31 * 0.1783);
    case ColCarbonatHaerte:
        return data(idx.row(), ColHydrogencarbonat).toDouble() / 61.02 * 2.8;
    case ColRestalkalitaet:
    {
        double carbh = data(idx.row(), ColCarbonatHaerte).toDouble();
        double calch = data(idx.row(), ColCalciumHaerte).toDouble();
        double magh = data(idx.row(), ColMagnesiumHaerte).toDouble();
        double add = data(idx.row(), ColRestalkalitaetAdd).toDouble();
        return carbh - (calch + 0.5 * magh) / 3.5 + add;
    }
    default:
        return QVariant();
    }
}

bool ModelWasser::setDataExt(const QModelIndex &idx, const QVariant &value)
{
     switch(idx.column())
     {
     case ColName:
     {
         QString name = getUniqueName(idx, value);
         QVariant prevName = data(idx);
         if (QSqlTableModel::setData(idx, name))
         {
             for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
             {
                 QModelIndex idx2 = bh->modelSud()->index(row, ModelSud::ColWasserprofil);
                 if (bh->modelSud()->data(idx2) == prevName)
                     bh->modelSud()->setData(idx2, name);
             }
             return true;
         }
         return false;
     }
     case ColHydrogencarbonatMmol:
         return QSqlTableModel::setData(index(idx.row(), ColHydrogencarbonat), value.toDouble() * 61.02);
     case ColCalciumMmol:
         return QSqlTableModel::setData(index(idx.row(), ColCalcium), value.toDouble() * 40.08);
     case ColMagnesiumMmol:
         return QSqlTableModel::setData(index(idx.row(), ColMagnesium), value.toDouble() * 24.31);
     case ColSulfatMmol:
         return QSqlTableModel::setData(index(idx.row(), ColSulfat), value.toDouble() * 96.06);
     case ColChloridMmol:
         return QSqlTableModel::setData(index(idx.row(), ColChlorid), value.toDouble() * 35.45);
     case ColNatriumMmol:
         return QSqlTableModel::setData(index(idx.row(), ColNatrium), value.toDouble() * 22.99);
     case ColCalciumHaerte:
         return QSqlTableModel::setData(index(idx.row(), ColCalcium), value.toDouble() * 40.08 * 0.1783);
     case ColMagnesiumHaerte:
         return QSqlTableModel::setData(index(idx.row(), ColMagnesium), value.toDouble() * 24.31 * 0.1783);
     case ColCarbonatHaerte:
         return QSqlTableModel::setData(index(idx.row(), ColHydrogencarbonat), value.toDouble() * 61.02 / 2.8);
     default:
         return false;
     }
}

Qt::ItemFlags ModelWasser::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(idx);
    switch (idx.column())
    {
    case ColHydrogencarbonatMmol:
    case ColCalciumMmol:
    case ColMagnesiumMmol:
    case ColSulfatMmol:
    case ColChloridMmol:
    case ColNatriumMmol:
    case ColCarbonatHaerte:
    case ColCalciumHaerte:
    case ColMagnesiumHaerte:
        itemFlags |= Qt::ItemIsEditable;
        break;
    }
    return itemFlags;
}

void ModelWasser::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
}
