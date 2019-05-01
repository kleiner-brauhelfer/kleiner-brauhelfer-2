#include "proxymodelsudcolored.h"
#include "settings.h"

extern Settings* gSettings;

ProxyModelSudColored::ProxyModelSudColored(QObject* parent) :
    ProxyModelSud(parent),
    mColBierWurdeGebraut(-1),
    mColBierWurdeAbgefuellt(-1),
    mColBierWurdeVerbraucht(-1),
    mColMerklistenID(-1),
    mColWoche(-1),
    mColReifeZeitDelta(-1)
{
}

void ProxyModelSudColored::setSourceModel(QAbstractItemModel *sourceModel)
{
    ProxyModelSud::setSourceModel(sourceModel);
    mColBierWurdeGebraut = fieldIndex("BierWurdeGebraut");
    mColBierWurdeAbgefuellt = fieldIndex("BierWurdeAbgefuellt");
    mColBierWurdeVerbraucht = fieldIndex("BierWurdeVerbraucht");
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
        else if (index.siblingAtColumn(mColBierWurdeVerbraucht).data().toBool())
            return gSettings->VerbrauchtBackground;
        else if (index.siblingAtColumn(mColBierWurdeAbgefuellt).data().toBool())
            return gSettings->AbgefuelltBackground;
        else if (index.siblingAtColumn(mColBierWurdeGebraut).data().toBool())
            return gSettings->GebrautBackground;
        else
            return gSettings->NichtGebrautBackground;
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == mColWoche)
        {
            if (index.siblingAtColumn(mColBierWurdeVerbraucht).data().toBool())
            {
                return "";
            }
            if (index.siblingAtColumn(mColBierWurdeAbgefuellt).data().toBool())
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
