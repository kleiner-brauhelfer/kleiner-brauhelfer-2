#include "proxymodelsudcolored.h"
#include "settings.h"

extern Settings* gSettings;

ProxyModelSudColored::ProxyModelSudColored(QObject* parent) :
    ProxyModelSud(parent),
    mColBierWurdeGebraut(-1),
    mColBierWurdeAbgefuellt(-1),
    mColBierWurdeVerbraucht(-1),
    mColMerklistenID(-1)
{
}

void ProxyModelSudColored::setSourceModel(QAbstractItemModel *sourceModel)
{
    ProxyModelSud::setSourceModel(sourceModel);
    mColBierWurdeGebraut = fieldIndex("BierWurdeGebraut");
    mColBierWurdeAbgefuellt = fieldIndex("BierWurdeAbgefuellt");
    mColBierWurdeVerbraucht = fieldIndex("BierWurdeVerbraucht");
    mColMerklistenID = fieldIndex("MerklistenID");
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
    return ProxyModelSud::data(index, role);
}
