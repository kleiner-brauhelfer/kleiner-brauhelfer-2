#ifndef TAGGLOBALDELEGATE_H
#define TAGGLOBALDELEGATE_H

#include "checkboxdelegate.h"

class SudObject;

class TagGlobalDelegate : public CheckBoxDelegate
{
    Q_OBJECT

public:
    explicit TagGlobalDelegate(SudObject *sud, QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
private:
    SudObject* mSud;
};

#endif // TAGGLOBALDELEGATE_H
