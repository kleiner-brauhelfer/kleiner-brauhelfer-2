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

QVariant ModelWasser::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "CalciumMmol")
    {
        return data(index.row(), "Calcium").toDouble() / 40.8;
    }
    if (field == "MagnesiumMmol")
    {
        return data(index.row(), "Magnesium").toDouble() / 24.3;
    }
    if (field == "Calciumhaerte")
    {
        return data(index.row(), "Calcium").toDouble() / 40.8 / 0.1783;
    }
    if (field == "Magnesiumhaerte")
    {
        return data(index.row(), "Magnesium").toDouble() / 24.3 / 0.1783;
    }
    if (field == "Carbonathaerte")
    {
        return data(index.row(), "Saeurekapazitaet").toDouble() * 2.8;
    }
    if (field == "Restalkalitaet")
    {
        double carbh = data(index.row(), "Carbonathaerte").toDouble();
        double calch = data(index.row(), "Calciumhaerte").toDouble();
        double magh = data(index.row(), "Magnesiumhaerte").toDouble();
        return carbh - (calch + 0.5 * magh) / 3.5;
    }
    return QVariant();
}

bool ModelWasser::setDataExt(const QModelIndex &index, const QVariant &value)
{
     QString field = fieldName(index.column());
     /*
     // TODO: add colum name and uncomment
     if (field == "Name")
     {
         QString name = getUniqueName(index, value);
         QString prevValue = data(index).toString();
         if (QSqlTableModel::setData(index, name))
         {
             int col = bh->modelSud()->fieldIndex("Wasserprofil");
             for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
             {
                 QModelIndex index = bh->modelSud()->index(row, col);
                 if (bh->modelSud()->data(index).toString() == prevValue)
                     bh->modelSud()->setData(index, name);
             }
             return true;
         }
     }
    */
     if (field == "CalciumMmol")
     {
         return setData(index.row(), "Calcium", value.toDouble() * 40.8);
     }
     if (field == "MagnesiumMmol")
     {
         return setData(index.row(), "Magnesium", value.toDouble() * 24.3);
     }
     if (field == "Calciumhaerte")
     {
         return setData(index.row(), "Calcium", value.toDouble() * 40.8 * 0.1783);
     }
     if (field == "Magnesiumhaerte")
     {
         return setData(index.row(), "Magnesium", value.toDouble() * 24.3 * 0.1783);
     }
     if (field == "Carbonathaerte")
     {
         return setData(index.row(), "Saeurekapazitaet", value.toDouble() / 2.8);
     }
     return false;
}

QString ModelWasser::getUniqueName(const QModelIndex &index, const QVariant &value)
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
}

Qt::ItemFlags ModelWasser::flags(const QModelIndex &index) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(index);
    QString field = fieldName(index.column());
    if (field == "CalciumMmol" ||
        field == "MagnesiumMmol" ||
        field == "Calciumhaerte" ||
        field == "Magnesiumhaerte" ||
        field == "Carbonathaerte"
       )
        itemFlags |= Qt::ItemIsEditable;
    return itemFlags;
}
