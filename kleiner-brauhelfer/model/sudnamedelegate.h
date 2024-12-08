#ifndef SUDNAME_H
#define SUDNAME_H

#include "textdelegate.h"

class SudNameDelegate : public TextDelegate
{
    Q_OBJECT

public:
    explicit SudNameDelegate(bool readonly = false, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter, QObject *parent = nullptr);
    SudNameDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // SUDNAME_H
