#include "ratingdelegate.h"
#include <QPainter>

RatingDelegate::RatingDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    mSize = 16;
    mPixmap = QIcon::fromTheme("star").pixmap(QSize(mSize, mSize));
}

void RatingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int n = index.data().toInt();
    int x = (option.rect.width() - mSize * n) / 2;
    int y = (option.rect.height() - mSize) / 2;
    painter->save();
    QStyledItemDelegate::paint(painter, option, index);
    painter->translate(option.rect.x() + x, option.rect.y() + y);
    for (int i = 0; i < n; ++i)
    {
        painter->drawPixmap(QPoint(0, 0), mPixmap);
        painter->translate(mSize, 0);
    }
    painter->restore();
}

QSize RatingDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    return QSize(mSize * index.data().toInt(), mSize);
}

QString RatingDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(value)
    Q_UNUSED(locale)
    return QString();
}
