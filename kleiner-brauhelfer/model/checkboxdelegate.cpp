#include "checkboxdelegate.h"
#include <QCheckBox>
#include <QApplication>
#include "commands/undostack.h"

CheckBoxDelegate::CheckBoxDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    QCheckBox* w = new QCheckBox(parent);
    return w;
}

void CheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QCheckBox *w = static_cast<QCheckBox*>(editor);
    w->setChecked(!index.data(Qt::EditRole).toBool());
}

void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *w = static_cast<QCheckBox*>(editor);
    gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->checkState() == Qt::Checked));
}

void CheckBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() + option.rect.width()/2 - checkbox_rect.width()/2);
    editor->setGeometry(checkboxstyle.rect);
}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton checkboxstyle;
    QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator, &checkboxstyle);
    checkboxstyle.rect = option.rect;
    checkboxstyle.rect.setLeft(option.rect.x() + option.rect.width()/2 - checkbox_rect.width()/2);
    if(index.data(Qt::DisplayRole).toBool())
        checkboxstyle.state = QStyle::State_On | QStyle::State_Enabled;
    else
        checkboxstyle.state = QStyle::State_Off | QStyle::State_Enabled;
    QStyledItemDelegate::paint(painter, option, index);
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkboxstyle, painter);
}

QString CheckBoxDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(value)
    Q_UNUSED(locale)
    return QString();
}
