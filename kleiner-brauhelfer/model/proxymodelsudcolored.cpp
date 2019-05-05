#include "proxymodelsudcolored.h"
#include "settings.h"
#include "database_defs.h"

extern Settings* gSettings;

ProxyModelSudColored::ProxyModelSudColored(QObject* parent) :
    ProxyModelSud(parent),
    mColStatus(-1),
    mColMerklistenID(-1),
    mColWoche(-1),
    mColReifeZeitDelta(-1)
{
}

void ProxyModelSudColored::setSourceModel(QAbstractItemModel *sourceModel)
{
    ProxyModelSud::setSourceModel(sourceModel);
    mColStatus = fieldIndex("Status");
    mColMerklistenID = fieldIndex("MerklistenID");
    mColWoche = fieldIndex("Woche");
    mColReifeZeitDelta = fieldIndex("ReifezeitDelta");
}

QVariant ProxyModelSudColored::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole)
    {
        if (index.siblingAtColumn(mColMerklistenID).data().toBool())
            return gSettings->MekrlisteBackground;
        switch(index.siblingAtColumn(mColStatus).data().toInt())
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
        if (index.column() == mColWoche)
        {
            if (index.siblingAtColumn(mColStatus).data().toInt() == Sud_Status_Abgefuellt)
            {
                int woche = ProxyModelSud::data(index, role).toInt();
                int tage = index.siblingAtColumn(mColReifeZeitDelta).data().toInt();
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
        if (index.column() == mColWoche)
        {
            return Qt::AlignCenter;
        }
    }
    return ProxyModelSud::data(index, role);
}
