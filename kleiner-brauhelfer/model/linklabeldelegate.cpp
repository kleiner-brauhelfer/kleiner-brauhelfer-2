#include "linklabeldelegate.h"

LinkLabelDelegate::LinkLabelDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
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
