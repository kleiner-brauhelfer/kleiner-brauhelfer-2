#ifndef STAMMWUERZEDELEGATE_H
#define STAMMWUERZEDELEGATE_H

#include "doublespinboxdelegate.h"


class StammWuerzeDelegate : public DoubleSpinBoxDelegate

{
    Q_OBJECT

public:
    explicit StammWuerzeDelegate(bool readonly = false, QObject *parent = nullptr);
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE;
};

#endif // STAMMWUERZEDELEGATE_H
