#include "modelwasser.h"
#include "brauhelfer.h"

ModelWasser::ModelWasser(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("CalciumMmol");
    mVirtualField.append("MagnesiumMmol");
    mVirtualField.append("Calciumhaerte");
    mVirtualField.append("Magnesiumhaerte");
    mVirtualField.append("Carbonathaerte");
    mVirtualField.append("Restalkalitaet");
}

QVariant ModelWasser::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColCalciumMmol:
    {
        return data(idx.row(), ColCalcium).toDouble() / 40.8;
    }
    case ColMagnesiumMmol:
    {
        return data(idx.row(), ColMagnesium).toDouble() / 24.3;
    }
    case ColCalciumhaerte:
    {
        return data(idx.row(), ColCalcium).toDouble() / 40.8 / 0.1783;
    }
    case ColMagnesiumhaerte:
    {
        return data(idx.row(), ColMagnesium).toDouble() / 24.3 / 0.1783;
    }
    case ColCarbonathaerte:
    {
        return data(idx.row(), ColSaeurekapazitaet).toDouble() * 2.8;
    }
    case ColRestalkalitaet:
    {
        double carbh = data(idx.row(), ColCarbonathaerte).toDouble();
        double calch = data(idx.row(), ColCalciumhaerte).toDouble();
        double magh = data(idx.row(), ColMagnesiumhaerte).toDouble();
        return carbh - (calch + 0.5 * magh) / 3.5;
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
     case ColCalciumMmol:
     {
         return QSqlTableModel::setData(index(idx.row(), ColCalcium), value.toDouble() * 40.8);
     }
     case ColMagnesiumMmol:
     {
         return QSqlTableModel::setData(index(idx.row(), ColMagnesium), value.toDouble() * 24.3);
     }
     case ColCalciumhaerte:
     {
         return QSqlTableModel::setData(index(idx.row(), ColCalcium), value.toDouble() * 40.8 * 0.1783);
     }
     case ColMagnesiumhaerte:
     {
         return QSqlTableModel::setData(index(idx.row(), ColMagnesium), value.toDouble() * 24.3 * 0.1783);
     }
     case ColCarbonathaerte:
     {
         return QSqlTableModel::setData(index(idx.row(), ColSaeurekapazitaet), value.toDouble() / 2.8);
     }
     default:
         return false;
     }
}

Qt::ItemFlags ModelWasser::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(idx);
    switch (idx.column())
    {
    case ColCalciumMmol:
    case ColMagnesiumMmol:
    case ColCalciumhaerte:
    case ColMagnesiumhaerte:
    case ColCarbonathaerte:
        itemFlags |= Qt::ItemIsEditable;
        break;
    }
    return itemFlags;
}

void ModelWasser::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
}
