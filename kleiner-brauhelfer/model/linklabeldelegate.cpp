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

QSize LinkLabelDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.features &= ~QStyleOptionViewItem::WrapText;
    return QStyledItemDelegate::sizeHint(opt, index);
}

void LinkLabelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.features &= ~QStyleOptionViewItem::WrapText;
    opt.font.setUnderline(true);
    opt.palette.setColor(QPalette::Text, opt.palette.link().color());
    QStyledItemDelegate::paint(painter, opt, index);
}
