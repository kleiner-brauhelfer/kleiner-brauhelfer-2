#ifndef PHMALZDELEGATE_H
#define PHMALZDELEGATE_H

#include "doublespinboxdelegate.h"

class PhMalzDelegate : public DoubleSpinBoxDelegate
{
    Q_OBJECT

public:
    explicit PhMalzDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // PHMALZDELEGATE_H
