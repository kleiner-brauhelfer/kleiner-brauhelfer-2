#include "doublespinboxdelegate.h"
#include <QDoubleSpinBox>
#include <QPainter>
#include "brauhelfer.h"
#include "settings.h"

extern Settings* gSettings;
extern Brauhelfer* bh;

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(int decimals, double min, double max, double step, bool zeroRed, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(false),
    mDecimals(decimals),
    mMin(min),
    mMax(max),
    mStep(step),
    mZeroRed(zeroRed),
    mConvert(false)
{
}

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(int decimals, QObject *parent) :
    DoubleSpinBoxDelegate(decimals, 0.0, 100.0, 0.1, false, parent)
{
    mReadonly = true;
}


QWidget* DoubleSpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    if (mReadonly)
        return nullptr;
    QDoubleSpinBox* w = new QDoubleSpinBox(parent);
    w->setAlignment(Qt::AlignHCenter);
    w->setMinimum(mMin);
    w->setMaximum(mMax);
    w->setDecimals(mDecimals);
    w->setSingleStep(mStep);
    return w;
}

void DoubleSpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    if (mConvert) {
        w->setValue(bh->convertGravity("Plato",gSettings->GravityName(),index.data(Qt::EditRole).toDouble()));
    } else {
    w->setValue(index.data(Qt::EditRole).toDouble());
    }
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    w->interpretText();
    if (mConvert) {
    model->setData(index, bh->convertGravity(gSettings->GravityName(),"Plato",w->value()), Qt::EditRole);
    } else {
    model->setData(index, w->value(), Qt::EditRole);
    }
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void DoubleSpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::AlignCenter;
    if (mZeroRed)
    {
        if (index.data(Qt::DisplayRole).toDouble() <= 0.0)
            painter->fillRect(opt.rect, gSettings->ErrorBase);
    }
    QStyledItemDelegate::paint(painter, opt, index);
}

QString DoubleSpinBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if (mConvert) {
        return locale.toString(bh->convertGravity("Plato",gSettings->GravityName(),value.toDouble()), 'f', mDecimals);
    } else {
    return locale.toString(value.toDouble(), 'f', mDecimals);
    }
}
