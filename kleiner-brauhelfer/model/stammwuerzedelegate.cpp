#include "stammwuerzedelegate.h"
#include <QDoubleSpinBox>
#include <QPainter>
#include "brauhelfer.h"
#include "settings.h"

extern Settings* gSettings;


StammWuerzeDelegate::StammWuerzeDelegate(int decimals, double min, double max, double step, bool zeroRed, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(false),
    mDecimals(decimals),
    mMin(min),
    mMax(max),
    mStep(step),
    mZeroRed(zeroRed)
{
}

StammWuerzeDelegate::StammWuerzeDelegate(QObject *parent) :
    StammWuerzeDelegate(1, 0.0, 100.0, 0.1, false, parent)
{
    mReadonly = true;
    mDecimals = gSettings->GravityDecimals();
}


QWidget* StammWuerzeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

void StammWuerzeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
        QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
        BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
        w->setValue(BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,index.data(Qt::EditRole).toDouble()));
}

void StammWuerzeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    w->interpretText();
    model->setData(index, w->value(), Qt::EditRole);
}

void StammWuerzeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void StammWuerzeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QString StammWuerzeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
        BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
        return locale.toString(BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,value.toDouble()), 'f', gSettings->GravityDecimals());
}

