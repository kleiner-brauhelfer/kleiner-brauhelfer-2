#include "textdelegate.h"
#include <QMetaProperty>
#include "commands/undostack.h"

TextDelegate::TextDelegate(bool readonly, Qt::Alignment alignment, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(readonly),
    mAlignment(alignment)
{
}

TextDelegate::TextDelegate(QObject *parent) :
    TextDelegate(false, Qt::AlignLeft | Qt::AlignVCenter, parent)
{
}

QWidget* TextDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (mReadonly)
        return nullptr;
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TextDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QByteArray n = editor->metaObject()->userProperty().name();
    if (!n.isEmpty())
        gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), editor->property(n)));
}

void TextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.displayAlignment = mAlignment;
    QStyledItemDelegate::paint(painter, opt, index);
}
