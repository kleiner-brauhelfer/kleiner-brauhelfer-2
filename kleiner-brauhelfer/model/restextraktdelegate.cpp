#include "restextraktdelegate.h"
#include "dialogs/dlgrestextrakt.h"
#include "modelschnellgaerverlauf.h"
#include "modelhauptgaerverlauf.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

RestextraktDelegate::RestextraktDelegate(bool hauptgaerung, QObject *parent) :
    DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, false, parent),
    mHauptgaerung(hauptgaerung)
{
    mDecimals = gSettings->GravityDecimals();
}

QWidget* RestextraktDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if (mReadonly)
        return nullptr;
    QDateTime dt;
    double T;
    double re = index.data().toDouble();
    double sw = bh->sud()->getSWIst();
    if (mHauptgaerung)
    {
        dt = index.sibling(index.row(), ModelHauptgaerverlauf::ColZeitstempel).data().toDateTime();
        T = index.sibling(index.row(), ModelHauptgaerverlauf::ColTemp).data().toDouble();
    }
    else
    {
        dt = index.sibling(index.row(), ModelSchnellgaerverlauf::ColZeitstempel).data().toDateTime();
        T = index.sibling(index.row(), ModelSchnellgaerverlauf::ColTemp).data().toDouble();
    }
    DlgRestextrakt* w = new DlgRestextrakt(re, sw, T, dt, parent);
    return w;
}

void RestextraktDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DlgRestextrakt *w = static_cast<DlgRestextrakt*>(editor);
    w->setValue(index.data(Qt::EditRole).toDouble());
    if (mHauptgaerung)
    {
        w->setDatum(index.sibling(index.row(), ModelHauptgaerverlauf::ColZeitstempel).data().toDateTime());
        w->setTemperatur(index.sibling(index.row(), ModelHauptgaerverlauf::ColTemp).data().toDouble());
    }
    else
    {
        w->setDatum(index.sibling(index.row(), ModelSchnellgaerverlauf::ColZeitstempel).data().toDateTime());
        w->setTemperatur(index.sibling(index.row(), ModelSchnellgaerverlauf::ColTemp).data().toDouble());
    }
}

void RestextraktDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    DlgRestextrakt *w = static_cast<DlgRestextrakt*>(editor);
    if (w->result() == QDialog::Accepted)
    {
        model->setData(index,w->value(), Qt::EditRole);
        if (mHauptgaerung)
        {
            model->setData(index.sibling(index.row(), ModelHauptgaerverlauf::ColTemp), w->temperatur(), Qt::EditRole);
            model->setData(index.sibling(index.row(), ModelHauptgaerverlauf::ColZeitstempel), w->datum(), Qt::EditRole);
        }
        else
        {
            model->setData(index.sibling(index.row(), ModelSchnellgaerverlauf::ColTemp), w->temperatur(), Qt::EditRole);
            model->setData(index.sibling(index.row(), ModelSchnellgaerverlauf::ColZeitstempel), w->datum(), Qt::EditRole);
        }
    }
}

QString RestextraktDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
    return locale.toString(BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,value.toDouble()), 'f', gSettings->GravityDecimals());
}

void RestextraktDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(editor)
    Q_UNUSED(option)
    Q_UNUSED(index)
}
