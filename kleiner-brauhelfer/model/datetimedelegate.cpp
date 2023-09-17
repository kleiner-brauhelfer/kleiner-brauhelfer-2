#include "datetimedelegate.h"
#include <QDateTimeEdit>
#include <QPainter>
#include "settings.h"
#include "commands/undostack.h"

extern Settings* gSettings;

DateTimeDelegate::DateTimeDelegate(bool expiredRed, bool readonly, QObject *parent) :
    QStyledItemDelegate(parent),
    mExpiredRed(expiredRed),
    mReadonly(readonly)
{
}

QWidget* DateTimeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    if (mReadonly)
        return nullptr;
    QDateTimeEdit* w = new QDateTimeEdit(parent);
    w->setAlignment(Qt::AlignHCenter);
    w->setDisplayFormat(QLocale().dateTimeFormat(QLocale::ShortFormat));
    w->setCalendarPopup(true);
    return w;
}

void DateTimeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QDateTimeEdit *w = static_cast<QDateTimeEdit*>(editor);
    w->setDateTime(index.data(Qt::EditRole).toDateTime());
}

void DateTimeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDateTimeEdit *w = static_cast<QDateTimeEdit*>(editor);
    gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->dateTime()));
}

void DateTimeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void DateTimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

QString DateTimeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return locale.toString(value.toDateTime(), QLocale::ShortFormat);
}
