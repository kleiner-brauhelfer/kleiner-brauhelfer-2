#include "linklabeldelegate.h"

LinkLabelDelegate::LinkLabelDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void LinkLabelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.font.setUnderline(true);
    opt.palette.setColor(QPalette::Text, opt.palette.link().color());
    QStyledItemDelegate::paint(painter, opt, index);
}
