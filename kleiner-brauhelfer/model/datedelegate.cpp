#include "datedelegate.h"
#include <QDateEdit>
#include <QPainter>
#include "settings.h"
#include "commands/undostack.h"

extern Settings* gSettings;

DateDelegate::DateDelegate(bool expiredRed, bool readonly, QObject *parent) :
    QStyledItemDelegate(parent),
    mExpiredRed(expiredRed),
    mReadonly(readonly)
{
}

QWidget* DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    if (mReadonly)
        return nullptr;
    QDateEdit* w = new QDateEdit(parent);
    w->setAlignment(Qt::AlignHCenter);
    w->setDisplayFormat(QLocale().dateFormat(QLocale::ShortFormat));
    w->setCalendarPopup(true);
    return w;
}

void DateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateEdit *w = static_cast<QDateEdit*>(editor);
    w->setDateTime(index.data(Qt::EditRole).toDateTime());
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateEdit *w = static_cast<QDateEdit*>(editor);
    gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->dateTime()));
}

void DateDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void DateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::AlignCenter;
    if (mExpiredRed)
    {
        if (QDate::currentDate().daysTo(index.data(Qt::DisplayRole).toDate()) < 0)
            painter->fillRect(opt.rect, gSettings->ErrorBase);
    }
    QStyledItemDelegate::paint(painter, opt, index);
}

QString DateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return locale.toString(value.toDate(), QLocale::ShortFormat);
}
