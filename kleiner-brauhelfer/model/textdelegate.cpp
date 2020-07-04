#include "textdelegate.h"

TextDelegate::TextDelegate(bool readonly, Qt::Alignment alignment, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(readonly),
    mAlignment(alignment)
{
}

QWidget* TextDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (mReadonly)
        return nullptr;
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TextDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.displayAlignment = mAlignment;
    QStyledItemDelegate::paint(painter, opt, index);
}
