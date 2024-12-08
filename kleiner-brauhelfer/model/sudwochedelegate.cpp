#include "sudwochedelegate.h"
#include "brauhelfer.h"

SudWocheDelegate::SudWocheDelegate(QObject *parent) :
    TextDelegate(false, Qt::AlignCenter, parent)
{
}

void SudWocheDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    TextDelegate::initStyleOption(option, index);
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(index.sibling(index.row(), ModelSud::ColStatus).data().toInt());
    if (status == Brauhelfer::SudStatus::Abgefuellt)
    {
        int woche = index.data().toInt();
        int tage = index.sibling(index.row(), ModelSud::ColReifezeitDelta).data().toInt();
        if (tage > 0)
            option->text = tr("%1. Woche, reif in %2 Tagen").arg(woche).arg(tage);
        else
            option->text = tr("%1. Woche").arg(woche);
    }
    else
    {
        option->text = "";
    }
}
