#ifndef READONLYDELEGATE_H
#define READONLYDELEGATE_H

#include <QStyledItemDelegate>

class ReadonlyDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ReadonlyDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};


#endif // READONLYDELEGATE_H
