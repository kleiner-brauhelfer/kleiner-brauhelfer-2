#include "readonlydelegate.h"

ReadonlyDelegate::ReadonlyDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* ReadonlyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(parent)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return nullptr;
}
