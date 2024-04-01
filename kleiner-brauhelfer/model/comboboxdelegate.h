#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

class ComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ComboBoxDelegate(const QStringList &items, QObject *parent = nullptr);
    ComboBoxDelegate(const QList<QPair<QString, int> > &items, QObject *parent = nullptr);
    ComboBoxDelegate(const QStringList &items, const QList<QColor> &colors, QObject *parent = nullptr);
    ComboBoxDelegate(const QList<QPair<QString, int> > &items, const QList<QColor> &colors, QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE;
    void setColors(const QList<QColor>& colors);
    QList<QColor> colors() const;

protected:
    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QStringList mItems;
    QList<QPair<QString, int> > mItemsMapped;
    QList<QColor> mColors;
};

#endif // COMBOBOXDELEGATE_H
