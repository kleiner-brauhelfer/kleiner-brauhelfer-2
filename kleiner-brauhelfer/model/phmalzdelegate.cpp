#include "phmalzdelegate.h"
#include "dialogs/dlgphmalz.h"
#include "modelmalz.h"
#include "commands/undostack.h"

PhMalzDelegate::PhMalzDelegate(QObject *parent) :
    DoubleSpinBoxDelegate(2, 0, 14, 0.1, false, parent)
{
}

QWidget* PhMalzDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    if (mReadonly)
        return nullptr;
    double farbe = index.sibling(index.row(), ModelMalz::ColFarbe).data().toDouble();
    DlgPhMalz* w = new DlgPhMalz(farbe, parent);
    return w;
}

void PhMalzDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DlgPhMalz *w = static_cast<DlgPhMalz*>(editor);
    w->setPh(index.data(Qt::EditRole).toDouble());
}

void PhMalzDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    DlgPhMalz *w = static_cast<DlgPhMalz*>(editor);
    if (w->result() == QDialog::Accepted)
        gUndoStack->push(new SetModelDataCommand(model, index.row(), index.column(), w->pH()));
}

void PhMalzDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(editor)
    Q_UNUSED(option)
    Q_UNUSED(index)
}
