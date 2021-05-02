#include "proxymodelsudcolored.h"
#include "settings.h"
#include "brauhelfer.h"

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
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(index.sibling(index.row(), ModelSud::ColStatus).data().toInt());
        switch(status)
        {
        case Brauhelfer::SudStatus::Rezept:
            return gSettings->NichtGebrautBackground;
        case Brauhelfer::SudStatus::Gebraut:
            return gSettings->GebrautBackground;
        case Brauhelfer::SudStatus::Abgefuellt:
            return gSettings->AbgefuelltBackground;
        case Brauhelfer::SudStatus::Verbraucht:
            return gSettings->VerbrauchtBackground;
        }
    }
    else if (role == Qt::DisplayRole)
    {
        if (index.column() == ModelSud::ColWoche)
        {
            Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(index.sibling(index.row(), ModelSud::ColStatus).data().toInt());
            if (status == Brauhelfer::SudStatus::Abgefuellt)
            {
                int woche = ProxyModelSud::data(index, role).toInt();
                int tage = index.sibling(index.row(), ModelSud::ColReifezeitDelta).data().toInt();
                if (tage > 0)
                    return tr("%1. Woche, reif in %2 Tagen").arg(woche).arg(tage);
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
