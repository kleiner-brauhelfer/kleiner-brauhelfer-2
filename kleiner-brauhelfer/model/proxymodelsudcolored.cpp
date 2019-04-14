#include "proxymodelsudcolored.h"
#include "settings.h"
#include "database_defs.h"

extern Settings* gSettings;

ProxyModelSudColored::ProxyModelSudColored(QObject* parent) :
    ProxyModelSud(parent),
    mColStatus(-1),
    mColMerklistenID(-1)
{
}

void ProxyModelSudColored::setSourceModel(QAbstractItemModel *sourceModel)
{
    ProxyModelSud::setSourceModel(sourceModel);
    mColStatus = fieldIndex("Status");
    mColMerklistenID = fieldIndex("MerklistenID");
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
    return ProxyModelSud::data(index, role);
}
