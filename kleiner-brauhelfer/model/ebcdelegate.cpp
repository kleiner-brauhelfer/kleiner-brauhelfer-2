#include "ebcdelegate.h"
#include <QPainter>
#include "biercalc.h"

EbcDelegate::EbcDelegate(QObject *parent) :
    SpinBoxDelegate(0, 2000, 1, false, parent)
{
}

void EbcDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    double ebc = index.model()->data(index, Qt::EditRole).toDouble();
    if (ebc > 0)
    {
        QColor color(BierCalc::ebcToColor(ebc));
        painter->fillRect(option.rect, color);
        opt.palette.setColor(QPalette::Text, ebc > 35 ? Qt::white : Qt::black);
    }
    SpinBoxDelegate::paint(painter, opt, index);
}
