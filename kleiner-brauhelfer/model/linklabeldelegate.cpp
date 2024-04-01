#include "linklabeldelegate.h"
#include <QMetaProperty>
#include "commands/undostack.h"

LinkLabelDelegate::LinkLabelDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void LinkLabelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QByteArray n = editor->metaObject()->userProperty().name();
    if (!n.isEmpty())
        gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), editor->property(n)));
}

void LinkLabelDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->features &= ~QStyleOptionViewItem::WrapText;
    option->font.setUnderline(true);
    option->palette.setColor(QPalette::Text, option->palette.link().color());
}
