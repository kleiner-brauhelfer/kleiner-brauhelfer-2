#ifndef EBCDELEGATE_H
#define EBCDELEGATE_H

#include "spinboxdelegate.h"

class EbcDelegate : public SpinBoxDelegate
{
    Q_OBJECT

public:
    explicit EbcDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // EBCDELEGATE_H
