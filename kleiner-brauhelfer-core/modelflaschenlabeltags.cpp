#include "modelflaschenlabeltags.h"
#include "brauhelfer.h"

ModelFlaschenlabelTags::ModelFlaschenlabelTags(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Global");
}

QVariant ModelFlaschenlabelTags::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Global")
    {
        return index.siblingAtColumn(fieldIndex("SudID")).data().toInt() < 0;
    }
    return QVariant();
}

bool ModelFlaschenlabelTags::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Tagname")
    {
        int sudId = data(index.row(), "SudID").toInt();
        return QSqlTableModel::setData(index, getUniqueName(index, value, sudId));
    }
    if (field == "Global")
    {
        int sudId = value.toBool() ? -1 : bh->sud()->id();
        return QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("SudID")), sudId);
    }
    return false;
}

bool ModelFlaschenlabelTags::isUnique(const QModelIndex &index, const QVariant &value, int sudId, bool ignoreIndexRow) const
{
    int colSudId = fieldIndex("SudID");
    for (int row = 0; row < rowCount(); ++row)
    {
        if (!ignoreIndexRow && row == index.row())
            continue;
        if (index.sibling(row, colSudId).data().toInt() != sudId)
            continue;
        if (index.siblingAtRow(row).data() == value)
            return false;
    }
    return true;
}

QString ModelFlaschenlabelTags::getUniqueName(const QModelIndex &index, const QVariant &value, int sudId, bool ignoreIndexRow) const
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name, sudId, ignoreIndexRow))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
}

Qt::ItemFlags ModelFlaschenlabelTags::flags(const QModelIndex &index) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(index);
    QString field = fieldName(index.column());
    if (field == "Global")
        itemFlags |= Qt::ItemIsEditable;
    return itemFlags;
}

void ModelFlaschenlabelTags::defaultValues(QVariantMap &values) const
{
    int sudId = -1;
    if (values.contains("SudID"))
        sudId = values["SudID"].toInt();
    if (values.contains("Tagname"))
        values["Tagname"] = getUniqueName(index(0, fieldIndex("Tagname")), values["Tagname"], sudId, true);
}
