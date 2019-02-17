#ifndef RATINGDELEGATE_H
#define RATINGDELEGATE_H

#include <QStyledItemDelegate>

class RatingDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RatingDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE;
private:
    int mSize;
    QPixmap mPixmap;
};

#endif // RATINGDELEGATE_H
