#ifndef RESTEXTRAKTDELEGATE_H
#define RESTEXTRAKTDELEGATE_H

#include "doublespinboxdelegate.h"

class RestextraktDelegate : public DoubleSpinBoxDelegate
{
    Q_OBJECT

public:
    explicit RestextraktDelegate(bool hauptgaerung, QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE;
private:
    const bool mHauptgaerung;
};

#endif // RESTEXTRAKTDELEGATE_H
