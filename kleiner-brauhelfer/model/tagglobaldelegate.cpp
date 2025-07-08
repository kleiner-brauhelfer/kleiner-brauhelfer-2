#include "tagglobaldelegate.h"
#include <QCheckBox>
#include "commands/undostack.h"
#include "modeltags.h"
#include "sudobject.h"

TagGlobalDelegate::TagGlobalDelegate(SudObject *sud, QObject *parent) :
    CheckBoxDelegate(parent),
    mSud(sud)
{
}

QWidget* TagGlobalDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (mSud)
        return CheckBoxDelegate::createEditor(parent, option, index);
    return nullptr;
}

void TagGlobalDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *w = static_cast<QCheckBox*>(editor);
    QVariant value = w->checkState() == Qt::Checked ? -1 : mSud->id();
    gUndoStack->push(new SetModelDataCommand(model, index.row(), ModelTags::ColSudID, value));
}
