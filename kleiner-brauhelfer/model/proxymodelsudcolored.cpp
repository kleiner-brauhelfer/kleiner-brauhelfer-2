#include "proxymodelsudcolored.h"
#include "settings.h"
#include "modelsud.h"
#include "database_defs.h"

extern Settings* gSettings;

ProxyModelSudColored::ProxyModelSudColored(QObject* parent) :
    ProxyModelSud(parent)
{
}

QVariant ProxyModelSudColored::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole)
    {
        if (index.sibling(index.row(), ModelSud::ColMerklistenID).data().toBool())
            return gSettings->MekrlisteBackground;
        switch(index.sibling(index.row(), ModelSud::ColStatus).data().toInt())
        {
        default:
        case Sud_Status_Rezept:
            return gSettings->NichtGebrautBackground;
        case Sud_Status_Gebraut:
            return gSettings->GebrautBackground;
        case Sud_Status_Abgefuellt:
            return gSettings->AbgefuelltBackground;
        case Sud_Status_Verbraucht:
            return gSettings->VerbrauchtBackground;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == ModelSud::ColWoche)
        {
            if (index.sibling(index.row(), ModelSud::ColStatus).data().toInt() == Sud_Status_Abgefuellt)
            {
                int woche = ProxyModelSud::data(index, role).toInt();
                int tage = index.sibling(index.row(), ModelSud::ColReifezeitDelta).data().toInt();
                if (tage > 0)
                    return tr("%1. Woche, reif in %2 Tage").arg(woche).arg(tage);
                else
                    return tr("%1. Woche").arg(woche);
            }
            return "";
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == ModelSud::ColWoche)
        {
            return Qt::AlignCenter;
        }
    }
    return ProxyModelSud::data(index, role);
}
