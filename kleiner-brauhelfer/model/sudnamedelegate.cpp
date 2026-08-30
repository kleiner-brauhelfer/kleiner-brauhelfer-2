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
        color = gSettings->colorMerkliste;
    }
    else
    {
        switch(static_cast<Brauhelfer::SudStatus>(index.sibling(index.row(), ModelSud::ColStatus).data().toInt()))
        {
        case Brauhelfer::SudStatus::Rezept:
            color = gSettings->colorRezept;
            break;
        case Brauhelfer::SudStatus::Gebraut:
            color = gSettings->colorGebraut;
            break;
        case Brauhelfer::SudStatus::Abgefuellt:
            color = gSettings->colorAbgefuellt;
            break;
        case Brauhelfer::SudStatus::Verbraucht:
            color = gSettings->colorAusgetrunken;
            break;
        }
    }
    if (color.isValid())
    {
        painter->fillRect(option.rect.x()+1, option.rect.y()+1, 8, option.rect.height()-2, color);

        QColor colorFade = color;
        colorFade.setAlphaF(gSettings->theme() == Qt::ColorScheme::Light ? 0.3 : 0.6);
        QLinearGradient gradient = QLinearGradient(option.rect.right(), 0, option.rect.left(), 0);
        gradient.setColorAt(1.0, colorFade);
        gradient.setColorAt(0.0, Qt::transparent);
        painter->fillRect(option.rect, gradient);
    }

    QStyleOptionViewItem opt(option);
    opt.rect.setLeft(10);
    QStyledItemDelegate::paint(painter, opt, index);
}
