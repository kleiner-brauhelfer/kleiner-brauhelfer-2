#include "ingredientnamedelegate.h"
#include "sqltablemodel.h"
#include "proxymodel.h"

IngredientNameDelegate::IngredientNameDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    mColInUsed(-1)
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
    if (mColInUsed == -1)
    {
        const SqlTableModel *model = qobject_cast<const SqlTableModel*>(index.model());
        if (model)
        {
            mColInUsed = model->fieldIndex("InGebrauch");
        }
        else
        {
            const ProxyModel *proxyModel = qobject_cast<const ProxyModel*>(index.model());
            if (proxyModel)
            {
                mColInUsed = proxyModel->fieldIndex("InGebrauch");
            }
        }
    }
    return index.siblingAtColumn(mColInUsed).data().toBool();
}
