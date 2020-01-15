#include "ingredientnamedelegate.h"
#include "sqltablemodel.h"
#include "proxymodel.h"

IngredientNameDelegate::IngredientNameDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void IngredientNameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.font.setItalic(isUsed(index));
    QStyledItemDelegate::paint(painter, opt, index);
}

bool IngredientNameDelegate::isUsed(const QModelIndex &index) const
{
    const SqlTableModel *model = dynamic_cast<const SqlTableModel*>(index.model());
    if (model)
        return model->data(index.row(), model->fieldIndex("InGebrauch")).toBool();
    const ProxyModel *proxyModel = static_cast<const ProxyModel*>(index.model());
    if (proxyModel)
        return proxyModel->data(index.row(), proxyModel->fieldIndex("InGebrauch")).toBool();
    return false;
}
