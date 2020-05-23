#include "phmalzdelegate.h"
#include "dialogs/dlgphmalz.h"
#include "modelmalz.h"

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
    w->setPh(index.model()->data(index, Qt::EditRole).toDouble());
}

void PhMalzDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    DlgPhMalz *w = static_cast<DlgPhMalz*>(editor);
    model->setData(index, w->pH(), Qt::EditRole);
}

void PhMalzDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(editor)
    Q_UNUSED(option)
    Q_UNUSED(index)
}
