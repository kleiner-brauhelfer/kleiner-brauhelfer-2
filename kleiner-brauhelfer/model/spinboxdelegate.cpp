#include "spinboxdelegate.h"
#include <QSpinBox>
#include <QPainter>
#include "settings.h"
#include "commands/undostack.h"

extern Settings* gSettings;

SpinBoxDelegate::SpinBoxDelegate(int min, int max, int step, bool zeroRed, QObject *parent) :
    QStyledItemDelegate(parent),
    mReadonly(false),
    mMin(min),
    mMax(max),
    mStep(step),
    mZeroRed(zeroRed)
{
}

SpinBoxDelegate::SpinBoxDelegate(QObject *parent) :
    SpinBoxDelegate(0, 100, 1, false, parent)
{
    mReadonly = true;
}

QWidget* SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    if (mReadonly)
        return nullptr;
    QSpinBox* w = new QSpinBox(parent);
    w->setAlignment(Qt::AlignHCenter);
    w->setMinimum(mMin);
    w->setMaximum(mMax);
    w->setSingleStep(mStep);
    return w;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSpinBox *w = static_cast<QSpinBox*>(editor);
    w->setValue(index.data(Qt::EditRole).toInt());
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox *w = static_cast<QSpinBox*>(editor);
    w->interpretText();
    gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->value()));
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void SpinBoxDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = Qt::AlignCenter;
    /* called more often than paint()
    if (mZeroRed)
    {
        if (index.data(Qt::DisplayRole).toInt() <= 0)
            option->backgroundBrush = gSettings->ErrorBase;
    }
    */
}

void SpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (mZeroRed)
    {
        if (index.data(Qt::DisplayRole).toInt() <= 0)
            painter->fillRect(option.rect, gSettings->ErrorBase);
    }
    QStyledItemDelegate::paint(painter, option, index);
}

QString SpinBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    return QString::number(value.toInt());
}
