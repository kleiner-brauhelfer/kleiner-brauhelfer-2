#include "ebcdelegate.h"
#include <QPainter>
#include "biercalc.h"

EbcDelegate::EbcDelegate(QObject *parent) :
    DoubleSpinBoxDelegate(1, 0, 2000, 1, false, parent)
{
}

void EbcDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    double ebc = index.data(Qt::EditRole).toDouble();
    if (ebc > 0)
    {
        QColor color(BierCalc::ebcToColor(ebc));
        painter->fillRect(option.rect, color);
        opt.palette.setColor(QPalette::Text, ebc > 35 ? Qt::white : Qt::black);
    }
    DoubleSpinBoxDelegate::paint(painter, opt, index);
}
