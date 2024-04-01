#include "ingredientnamedelegate.h"
#include <QMetaProperty>
#include "sqltablemodel.h"
#include "proxymodel.h"
#include "commands/undostack.h"

IngredientNameDelegate::IngredientNameDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    mColInUsed(-1)
{
}

void IngredientNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QByteArray n = editor->metaObject()->userProperty().name();
    if (!n.isEmpty())
        gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), editor->property(n)));
}

void IngredientNameDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    /* called more often than paint()
    option->font.setItalic(isUsed(index));
    */
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
            mColInUsed = model->fieldIndex(QStringLiteral("InGebrauch"));
        }
        else
        {
            const ProxyModel *proxyModel = qobject_cast<const ProxyModel*>(index.model());
            if (proxyModel)
            {
                mColInUsed = proxyModel->fieldIndex(QStringLiteral("InGebrauch"));
            }
        }
    }
    return index.siblingAtColumn(mColInUsed).data().toBool();
}
