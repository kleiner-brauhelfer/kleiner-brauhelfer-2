#ifndef INGREDIENTNAMEDELEGATE_H
#define INGREDIENTNAMEDELEGATE_H

#include <QStyledItemDelegate>

class IngredientNameDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IngredientNameDelegate(QObject *parent = nullptr);
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

protected:
    virtual void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    bool isUsed(const QModelIndex &index) const;

private:
    mutable int mColInUsed;
};

#endif // INGREDIENTNAMEDELEGATE_H
