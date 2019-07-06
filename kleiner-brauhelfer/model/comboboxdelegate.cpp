#include "comboboxdelegate.h"
#include <QComboBox>
#include <QPainter>

ComboBoxDelegate::ComboBoxDelegate(const QStringList &items, QObject *parent) :
    QStyledItemDelegate(parent),
    mItems(items)
{
}

QWidget* ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QComboBox* w = new QComboBox(parent);
    w->addItems(mItems);
    return w;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *w = static_cast<QComboBox*>(editor);
    w->setCurrentIndex(index.model()->data(index, Qt::EditRole).toInt());
    w->showPopup();
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *w = static_cast<QComboBox*>(editor);
    model->setData(index, w->currentIndex(), Qt::EditRole);
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
    int idx = index.model()->data(index).toInt();
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
    if (idx < 0 || idx >= mItems.count())
        return QString();
    return mItems.at(idx);
}

void ComboBoxDelegate::setColors(const QList<QColor>& colors)
{
    mColors = colors;
}

QList<QColor> ComboBoxDelegate::colors() const
{
    return mColors;
}
