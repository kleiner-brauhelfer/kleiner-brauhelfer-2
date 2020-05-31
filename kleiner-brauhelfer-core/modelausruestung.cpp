#include "modelausruestung.h"
#include "brauhelfer.h"
#include <qmath.h>
#include "proxymodelsud.h"

ModelAusruestung::ModelAusruestung(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Maischebottich_Volumen");
    mVirtualField.append("Maischebottich_MaxFuellvolumen");
    mVirtualField.append("Sudpfanne_Volumen");
    mVirtualField.append("Sudpfanne_MaxFuellvolumen");
    mVirtualField.append("Vermoegen");
    mVirtualField.append("AnzahlSude");
}

QVariant ModelAusruestung::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColMaischebottich_Volumen:
    {
        double r = data(idx.row(), ColMaischebottich_Durchmesser).toDouble() / 2;
        double h = data(idx.row(), ColMaischebottich_Hoehe).toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    case ColMaischebottich_MaxFuellvolumen:
    {
        double r = data(idx.row(), ColMaischebottich_Durchmesser).toDouble() / 2;
        double h = data(idx.row(), ColMaischebottich_MaxFuellhoehe).toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    case ColSudpfanne_Volumen:
    {
        double r = data(idx.row(), ColSudpfanne_Durchmesser).toDouble() / 2;
        double h = data(idx.row(), ColSudpfanne_Hoehe).toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    case ColSudpfanne_MaxFuellvolumen:
    {
        double r = data(idx.row(), ColSudpfanne_Durchmesser).toDouble() / 2;
        double h = data(idx.row(), ColSudpfanne_MaxFuellhoehe).toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    case ColVermoegen:
    {
        double V1 = data(idx.row(), ColMaischebottich_MaxFuellvolumen).toDouble();
        double V2 = data(idx.row(), ColSudpfanne_MaxFuellvolumen).toDouble();
        return (V1 > V2) ? V2 : V1;
    }
    case ColAnzahlSude:
    {
        ProxyModel modelSud;
        modelSud.setSourceModel(bh->modelSud());
        modelSud.setFilterKeyColumn(ModelSud::ColAnlage);
        modelSud.setFilterRegExp(QString("^%1$").arg(data(idx.row(), ColName).toString()));
        return modelSud.rowCount();
    }
    default:
        return QVariant();
    }
}

bool ModelAusruestung::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString name = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, name))
        {
            for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
            {
                QModelIndex idx2 = bh->modelSud()->index(row, ModelSud::ColAnlage);
                if (bh->modelSud()->data(idx2) == prevName)
                    bh->modelSud()->setData(idx2, name);
            }
            return true;
        }
        return false;
    }
    case ColTyp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            Brauhelfer::AnlageTyp typ = static_cast<Brauhelfer::AnlageTyp>(value.toInt());
            switch (typ)
            {
            case Brauhelfer::AnlageTyp::Standard:
                break;
            case Brauhelfer::AnlageTyp::GrainfatherG30:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 29.5);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 52.5);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 43.9);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 29.5);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 52.5);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 43.9);
                break;
            case Brauhelfer::AnlageTyp::GrainfatherG70:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 44.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 56.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 50.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 44.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 56.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 50.0);
                break;
            case Brauhelfer::AnlageTyp::Braumeister10:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 24.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 33.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 26.5);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 24.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 33.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 26.5);
                break;
            case Brauhelfer::AnlageTyp::Braumeister20:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 34.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 45.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 25.3);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 34.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 45.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 25.3);
                break;
            case Brauhelfer::AnlageTyp::Braumeister50:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 44.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 55.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 36.2);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 44.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 55.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 36.2);
                break;
            case Brauhelfer::AnlageTyp::Braumeister200:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 0.0);
                break;
            case Brauhelfer::AnlageTyp::Braumeister500:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 0.0);
                break;
            case Brauhelfer::AnlageTyp::Braumeister1000:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 0.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 0.0);
                break;
            case Brauhelfer::AnlageTyp::BrauheldPro30:
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Durchmesser), 29.8);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_Hoehe), 47.0);
                QSqlTableModel::setData(index(idx.row(), ColMaischebottich_MaxFuellhoehe), 43.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Durchmesser), 29.8);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_Hoehe), 47.0);
                QSqlTableModel::setData(index(idx.row(), ColSudpfanne_MaxFuellhoehe), 43.0);
                break;
            }
            return true;
        }
        return false;
    }
    case ColMaischebottich_Hoehe:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QModelIndex idx2 = index(idx.row(), ColMaischebottich_MaxFuellhoehe);
            if (idx2.data().toDouble() > value.toDouble())
                QSqlTableModel::setData(idx2, value);
            return true;
        }
        return false;
    }
    case ColSudpfanne_Hoehe:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QModelIndex idx2 = index(idx.row(), ColSudpfanne_MaxFuellhoehe);
            if (idx2.data().toDouble() > value.toDouble())
                QSqlTableModel::setData(idx2, value);
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

bool ModelAusruestung::removeRows(int row, int count, const QModelIndex &parent)
{
    QList<int> ids;
    for (int i = 0; i < count; ++i)
        ids.append(data(row + i, ColID).toInt());
    if (SqlTableModel::removeRows(row, count, parent))
    {
        for (int r = 0; r < bh->modelGeraete()->rowCount(); ++r)
        {
            if (ids.contains(bh->modelGeraete()->data(r, ModelGeraete::ColAusruestungAnlagenID).toInt()))
            {
                int count = bh->modelGeraete()->rowCount();
                bh->modelGeraete()->removeRows(r);
                if (count != bh->modelGeraete()->rowCount())
                    r--;
            }
        }
        return true;
    }
    return false;
}

void ModelAusruestung::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
    if (!values.contains(ColID))
        values[ColID] = getNextId();
    if (!values.contains(ColSudhausausbeute))
        values.insert(ColSudhausausbeute, 60.0);
    if (!values.contains(ColVerdampfungsrate))
        values.insert(ColVerdampfungsrate, 2.0);
}
