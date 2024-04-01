#include "doublespinboxdelegate.h"
#include <QDoubleSpinBox>
#include <QPainter>
#include "settings.h"
#include "commands/undostack.h"

extern Settings* gSettings;

DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(int decimals, double min, double max, double step, bool zeroRed, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(false),
    mDecimals(decimals),
    mMin(min),
    mMax(max),
    mStep(step),
    mZeroRed(zeroRed)
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
    w->setValue(index.data(Qt::EditRole).toDouble());
}

void DoubleSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *w = static_cast<QDoubleSpinBox*>(editor);
    w->interpretText();
    gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->value()));
}

void DoubleSpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void DoubleSpinBoxDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = Qt::AlignCenter;
    /* called more often than paint()
    if (mZeroRed)
    {
        if (index.data(Qt::DisplayRole).toDouble() <= 0)
            option->backgroundBrush = gSettings->ErrorBase;
    }
    */
}

void DoubleSpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (mZeroRed)
    {
        if (index.data(Qt::DisplayRole).toDouble() <= 0.0)
            painter->fillRect(option.rect, gSettings->ErrorBase);
    }
    QStyledItemDelegate::paint(painter, option, index);
}

QString DoubleSpinBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return locale.toString(value.toDouble(), 'f', mDecimals);
}
