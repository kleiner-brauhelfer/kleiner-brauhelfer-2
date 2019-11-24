#include "modelflaschenlabeltags.h"
#include "brauhelfer.h"

ModelFlaschenlabelTags::ModelFlaschenlabelTags(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Global");
}

QVariant ModelFlaschenlabelTags::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColGlobal:
    {
        return data(idx.row(), ColSudID).toInt() < 0;
    }
    default:
        return QVariant();
    }
}

bool ModelFlaschenlabelTags::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColTagname:
    {
        return QSqlTableModel::setData(idx, getUniqueName(idx, value, data(idx.row(), ColSudID)));
    }
    case ColGlobal:
    {
        QVariant sudId = value.toBool() ? -1 : bh->sud()->id();
        if (QSqlTableModel::setData(index(idx.row(), ColSudID), sudId))
        {
            QModelIndex idx2 = index(idx.row(), ColTagname);
            setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

bool ModelFlaschenlabelTags::isUnique(const QModelIndex &index, const QVariant &value, const QVariant &sudId, bool ignoreIndexRow) const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        if (!ignoreIndexRow && row == index.row())
            continue;
        if (data(row, ColSudID) != sudId)
            continue;
        if (data(row, index.column()) == value)
            return false;
    }
    return true;
}

QString ModelFlaschenlabelTags::getUniqueName(const QModelIndex &index, const QVariant &value, const QVariant &sudId, bool ignoreIndexRow) const
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name, sudId, ignoreIndexRow))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
}

Qt::ItemFlags ModelFlaschenlabelTags::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(idx);
    if (idx.column() == ColGlobal)
        itemFlags |= Qt::ItemIsEditable;
    return itemFlags;
}

void ModelFlaschenlabelTags::defaultValues(QMap<int, QVariant> &values) const
{
    int sudId = -1;
    if (values.contains(ColSudID))
        sudId = values[ColSudID].toInt();
    if (values.contains(ColTagname))
        values[ColTagname] = getUniqueName(index(0, ColTagname), values[ColTagname], sudId, true);
}
