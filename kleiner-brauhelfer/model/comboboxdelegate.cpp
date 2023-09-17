#include "comboboxdelegate.h"
#include <QComboBox>
#include <QPainter>
#include "commands/undostack.h"

ComboBoxDelegate::ComboBoxDelegate(const QStringList &items, QObject *parent) :
    QStyledItemDelegate(parent),
    mItems(items)
{
}

ComboBoxDelegate::ComboBoxDelegate(const QList<QPair<QString, int> > &items, QObject *parent) :
    QStyledItemDelegate(parent),
    mItemsMapped(items)
{
}

ComboBoxDelegate::ComboBoxDelegate(const QStringList &items, const QList<QColor> &colors, QObject *parent) :
    ComboBoxDelegate(items, parent)
{
    setColors(colors);
}

ComboBoxDelegate::ComboBoxDelegate(const QList<QPair<QString, int> > &items, const QList<QColor> &colors, QObject *parent) :
    ComboBoxDelegate(items, parent)
{
    setColors(colors);
}

QWidget* ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QComboBox* w = new QComboBox(parent);
    if (mItemsMapped.empty())
    {
        w->addItems(mItems);
    }
    else
    {
        for (const auto &m : mItemsMapped)
            w->addItem(m.first, m.second);
    }
    return w;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *w = static_cast<QComboBox*>(editor);
    w->setCurrentIndex(index.data(Qt::EditRole).toInt());
    w->showPopup();
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *w = static_cast<QComboBox*>(editor);
    int idx = w->currentIndex();
    if (idx >= 0)
    {
        QVariant data = w->currentData();
        if (data.isNull())
            gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), idx));
        else
            gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), data));
    }
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::AlignCenter;
    int idx = index.data(Qt::DisplayRole).toInt();
    if (idx >= 0 && idx < mColors.count())
    {
        QColor color = mColors.at(idx);
        if (color.isValid())
            painter->fillRect(opt.rect, color);
    }
    QStyledItemDelegate::paint(painter, opt, index);
}

QString ComboBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    int idx = value.toInt();
    if (mItemsMapped.empty())
    {
        if (idx < 0 || idx >= mItems.count())
            return QString::number(idx);
        return mItems.at(idx);
    }
    else
    {
        for (const auto &m : mItemsMapped)
        {
            if (m.second == idx)
                return m.first;
        }
        return QString::number(idx);
    }
}

void ComboBoxDelegate::setColors(const QList<QColor>& colors)
{
    mColors = colors;
}

QList<QColor> ComboBoxDelegate::colors() const
{
    return mColors;
}
