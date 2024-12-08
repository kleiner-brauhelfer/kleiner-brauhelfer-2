#include "sudnamedelegate.h"
#include <QPainter>
#include "settings.h"
#include "brauhelfer.h"

extern Settings* gSettings;

SudNameDelegate::SudNameDelegate(bool readonly, Qt::Alignment alignment, QObject *parent) :
    TextDelegate(readonly, alignment, parent)
{
}

SudNameDelegate::SudNameDelegate(QObject *parent) :
    TextDelegate(parent)
{
}

void SudNameDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QColor color;
    if (index.sibling(index.row(), ModelSud::ColMerklistenID).data().toBool())
    {
        color = gSettings->MekrlisteBackground;
    }
    else
    {
        switch(static_cast<Brauhelfer::SudStatus>(index.sibling(index.row(), ModelSud::ColStatus).data().toInt()))
        {
        case Brauhelfer::SudStatus::Rezept:
            color = gSettings->NichtGebrautBackground;
            break;
        case Brauhelfer::SudStatus::Gebraut:
            color = gSettings->GebrautBackground;
            break;
        case Brauhelfer::SudStatus::Abgefuellt:
            color = gSettings->AbgefuelltBackground;
            break;
        case Brauhelfer::SudStatus::Verbraucht:
            color = gSettings->VerbrauchtBackground;
            break;
        }
    }
    if (color.isValid())
    {
        QRect rect = QRect(option.rect.x()+1, option.rect.y()+1, 8, option.rect.height()-2);
        painter->fillRect(rect, color);
    }

    QStyleOptionViewItem opt(option);
    opt.rect.setLeft(10);
    QStyledItemDelegate::paint(painter, opt, index);
}
