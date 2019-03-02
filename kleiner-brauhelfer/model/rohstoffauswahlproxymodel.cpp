#include "rohstoffauswahlproxymodel.h"
#include "settings.h"

extern Settings* gSettings;

RohstoffAuswahlProxyModel::RohstoffAuswahlProxyModel(QObject* parent) :
    ProxyModel(parent),
    mColMenge(-1)
{
}

QVariant RohstoffAuswahlProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole)
    {
        double menge = index.siblingAtColumn(mColMenge).data().toDouble();
        if (menge == 0.0)
            return gSettings->ErrorText;
        else if (menge < mIndexMengeBenoetigt.data().toDouble())
            return gSettings->WarningText;
    }
    return ProxyModel::data(index, role);
}

void RohstoffAuswahlProxyModel::setColumnMenge(int col)
{
    mColMenge = col;
}

void RohstoffAuswahlProxyModel::setIndexMengeBenoetigt(const QModelIndex &index)
{
    mIndexMengeBenoetigt = index;
}
