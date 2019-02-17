#ifndef INGREDIENTNAMEDELEGATE_H
#define INGREDIENTNAMEDELEGATE_H

#include <QStyledItemDelegate>

class IngredientNameDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IngredientNameDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
    bool isUsed(const QModelIndex &index) const;
};

#endif // INGREDIENTNAMEDELEGATE_H
