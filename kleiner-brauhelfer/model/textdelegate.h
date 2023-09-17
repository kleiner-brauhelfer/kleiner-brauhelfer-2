#ifndef TEXTDELEGATE_H
#define TEXTDELEGATE_H

#include <QStyledItemDelegate>

class TextDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TextDelegate(bool readonly = false, Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter, QObject *parent = nullptr);
    TextDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    const bool mReadonly;
    const Qt::Alignment mAlignment;
};

#endif // TEXTDELEGATE_H
