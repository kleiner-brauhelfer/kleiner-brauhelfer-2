#ifndef STAMMWUERZEDELEGATE_H
#define STAMMWUERZEDELEGATE_H

#include <QStyledItemDelegate>


class StammWuerzeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit StammWuerzeDelegate(int decimals, double min, double max, double step = 0.1, bool zeroRed = false, QObject *parent = nullptr);
    StammWuerzeDelegate(QObject *parent = nullptr);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    QString displayText(const QVariant &value, const QLocale &locale) const Q_DECL_OVERRIDE;
protected:
    bool mReadonly;
    int mDecimals;
    double mMin;
    double mMax;
    double mStep;
    bool mZeroRed;
};

#endif // STAMMWUERZEDELEGATE_H
