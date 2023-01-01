// clazy:excludeall=skipped-base-method
#include "modelhopfengaben.h"
#include <math.h>
#include "brauhelfer.h"
#include "proxymodelsud.h"

ModelHopfengaben::ModelHopfengaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("IBUAnteil");
    mVirtualField.append("Ausbeute");
    connect(bh->modelSud(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(onSudDataChanged(QModelIndex)));
}

QVariant ModelHopfengaben::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColIBUAnteil:
    {
        double menge = data(idx.row(), Colerg_Menge).toDouble();
        double alpha = data(idx.row(), ColAlpha).toDouble();
        double ausbeute = data(idx.row(), ColAusbeute).toDouble();
        double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMengeSollAnstellen).toDouble();
        return menge * alpha * ausbeute / (10 * mengeSoll);
    }
    case ColAusbeute:
    {
        // https://www.maischemalzundmehr.de/index.php?inhaltmitte=toolsiburechne
        int rowSudId = bh->modelSud()->getRowWithValue(ModelSud::ColID, data(idx.row(), ColSudID));
        double kochzeit = bh->modelSud()->data(rowSudId, ModelSud::ColKochdauer).toDouble();
        double isozeit = bh->modelSud()->data(rowSudId, ModelSud::ColNachisomerisierungszeit).toDouble();
        double t = data(idx.row(), ColZeit).toDouble();
        double sw_beginn = bh->modelSud()->data(rowSudId, ModelSud::ColSWSollKochbeginn).toDouble();
        double sw_ende = bh->modelSud()->data(rowSudId, ModelSud::ColSWSollKochende).toDouble();
        double sw;
        if (t > 0)
            sw = (sw_ende - sw_beginn) / kochzeit * (kochzeit - t/2)  + sw_beginn;
        else
            sw = sw_ende;
        double Tiso = 80.0;
        double ausbeute = 100 * BierCalc::tinseth(t + isozeit * 0.046 * exp(0.031 * Tiso), sw);
        if (data(idx.row(), ColPellets).toBool())
            ausbeute *= 1.1;
        if (kochzeit - t > 15)
            ausbeute *= 1.1;
        return ausbeute;
    }
    default:
        return QVariant();
    }
}

bool ModelHopfengaben::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            int row = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, value);
            if (row >= 0)
            {
                QSqlTableModel::setData(index(idx.row(), ColAlpha), bh->modelHopfen()->data(row, ModelHopfen::ColAlpha));
                QSqlTableModel::setData(index(idx.row(), ColPellets), bh->modelHopfen()->data(row, ModelHopfen::ColPellets));
                QModelIndex idx2 = index(idx.row(), ColProzent);
                setData(idx2, idx2.data());
            }
            return true;
        }
        return false;
    }
    case ColProzent:
    {
        double fVal = value.toDouble();
        if (fVal < 0.0)
            fVal = 0.0;
        if (fVal > 100.0)
            fVal = 100.0;
        if (QSqlTableModel::setData(idx, fVal))
        {
            QVariant sudId = data(idx.row(), ColSudID);
            double mengeSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColMengeSollAnstellen).toDouble();
            double ibuSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColIBU).toDouble();
            Brauhelfer::BerechnungsartHopfen berechnungsart = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->modelSud()->dataSud(sudId, ModelSud::ColberechnungsArtHopfen).toInt());
            switch (berechnungsart)
            {
            case Brauhelfer::BerechnungsartHopfen::Keine:
            {
                break;
            }
            case Brauhelfer::BerechnungsartHopfen::Gewicht:
            {
                double summe = 0.0;
                for (int r = 0; r < rowCount(); ++r)
                {
                    if (data(r, ColSudID) == sudId)
                    {
                        double alpha = index(r, ColAlpha).data().toDouble();
                        double ausbeute = index(r, ColAusbeute).data().toDouble();
                        double prozent = index(r, ColProzent).data().toDouble();
                        summe += prozent * alpha * ausbeute;
                    }
                }
                double factor = ibuSoll * 10 * mengeSoll / summe;
                for (int r = 0; r < rowCount(); ++r)
                {
                    if (data(r, ColSudID) == sudId)
                    {
                        double menge = factor * index(r, ColProzent).data().toDouble();
                        QSqlTableModel::setData(index(r, Colerg_Menge), menge);
                    }
                }
                break;
            }
            case Brauhelfer::BerechnungsartHopfen::IBU:
            {
                double alpha = data(idx.row(), ColAlpha).toDouble();
                double ausbeute = data(idx.row(), ColAusbeute).toDouble();
                if (alpha != 0.0 && ausbeute != 0.0)
                {
                    double anteil = ibuSoll * fVal / 100 ;
                    double menge = (anteil * mengeSoll * 10) / (alpha * ausbeute);
                    QSqlTableModel::setData(index(idx.row(), Colerg_Menge), menge);
                }
                else
                {
                    QSqlTableModel::setData(idx, 0.0);
                }
                break;
            }
            }
            return true;
        }
        return false;
    }
    case Colerg_Menge:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant sudId = data(idx.row(), ColSudID);
            Brauhelfer::BerechnungsartHopfen berechnungsart = static_cast<Brauhelfer::BerechnungsartHopfen>(bh->modelSud()->dataSud(sudId, ModelSud::ColberechnungsArtHopfen).toInt());
            switch (berechnungsart)
            {
            case Brauhelfer::BerechnungsartHopfen::Keine:
            {
                break;
            }
            case Brauhelfer::BerechnungsartHopfen::Gewicht:
            {
                double summe = 0.0;
                for (int r = 0; r < rowCount(); ++r)
                {
                    if (index(r, ColSudID).data() == sudId)
                    {
                        summe += index(r, Colerg_Menge).data().toDouble();
                    }
                }
                double p = value.toDouble() / summe * 100;
                QSqlTableModel::setData(index(idx.row(), ColProzent), p);
                break;
            }
            case Brauhelfer::BerechnungsartHopfen::IBU:
            {
                double mengeSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColMengeSollAnstellen).toDouble();
                double ibuSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColIBU).toDouble();
                double alpha = data(idx.row(), ColAlpha).toDouble();
                double ausbeute = data(idx.row(), ColAusbeute).toDouble();
                double p = (10 * alpha * ausbeute * value.toDouble()) / (ibuSoll * mengeSoll);
                QSqlTableModel::setData(index(idx.row(), ColProzent), p);
                break;
            }
            }
            return true;
        }
        return false;
    }
    case ColZeit:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QModelIndex idx2 = index(idx.row(), ColProzent);
            setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    case ColAlpha:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QModelIndex idx2 = index(idx.row(), ColProzent);
            setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    case ColPellets:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QModelIndex idx2 = index(idx.row(), ColProzent);
            setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    case ColZeitpunkt:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            int dauer = 0;
            switch (static_cast<Brauhelfer::HopfenZeitpunkt>(value.toInt()))
            {
            case Brauhelfer::HopfenZeitpunkt::Kochen:
            case Brauhelfer::HopfenZeitpunkt::KochenAlt:
                if (data(idx.row(), ColZeit).toInt() >= 0)
                    return true;
                break;
            case Brauhelfer::HopfenZeitpunkt::Vorderwuerze:
            case Brauhelfer::HopfenZeitpunkt::Kochbeginn:
                dauer = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColKochdauer).toInt();
                break;
            case Brauhelfer::HopfenZeitpunkt::Kochende:
                dauer = 0;
                break;
            case Brauhelfer::HopfenZeitpunkt::Ausschlagen:
                dauer = -bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColNachisomerisierungszeit).toInt();
                break;
            }
            QSqlTableModel::setData(index(idx.row(), ColZeit), dauer);
            QModelIndex idx2 = index(idx.row(), ColProzent);
            setData(idx2, idx2.data());
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

void ModelHopfengaben::onSudDataChanged(const QModelIndex &idx)
{
    switch (idx.column())
    {
    case ModelSud::ColMenge:
    case ModelSud::ColMengeSollAnstellen:
    case ModelSud::ColSW:
    case ModelSud::ColIBU:
    case ModelSud::ColberechnungsArtHopfen:
    case ModelSud::ColhighGravityFaktor:
    case ModelSud::ColKochdauer:
    case ModelSud::ColNachisomerisierungszeit:
    {
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        int colUpdate = idx.column() == ModelSud::ColberechnungsArtHopfen ? Colerg_Menge : ColProzent;
        for (int r = 0; r < rowCount(); ++r)
        {
            if (data(r, ColSudID) == sudId)
            {
                if (idx.column() == ModelSud::ColKochdauer)
                {
                    int max = idx.data().toInt();
                    QModelIndex idx2 = index(r, ColZeit);
                    switch (static_cast<Brauhelfer::HopfenZeitpunkt>(data(r, ColZeitpunkt).toInt()))
                    {
                    case Brauhelfer::HopfenZeitpunkt::Vorderwuerze:
                    case Brauhelfer::HopfenZeitpunkt::Kochbeginn:
                        QSqlTableModel::setData(index(r, ColZeit), max);
                        break;
                    default:
                        if (idx2.data().toInt() > max)
                            QSqlTableModel::setData(idx2, max);
                        break;
                    }
                }
                else if (idx.column() == ModelSud::ColNachisomerisierungszeit)
                {
                    int min = -idx.data().toInt();
                    QModelIndex idx2 = index(r, ColZeit);
                    if (idx2.data().toInt() < min)
                        QSqlTableModel::setData(idx2, min);
                }
                QModelIndex idx2 = index(r, colUpdate);
                setData(idx2, data(idx2));
            }
        }
        mSignalModifiedBlocked = false;
        break;
    }
    }
}

void ModelHopfengaben::update(const QVariant &name, int col, const QVariant &value)
{
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept);
    for (int r = 0; r < modelSud.rowCount(); ++r)
    {
        QVariant sudId = modelSud.data(r, ModelSud::ColID);
        for (int j = 0; j < rowCount(); ++j)
        {
            if (data(j, ColSudID) == sudId && data(j, ColName) == name)
                setData(j, col, value);
        }
    }
}

int ModelHopfengaben::import(int row)
{
    QMap<int, QVariant> values({{ModelHopfen::ColName, data(row, ColName)},
                                {ModelHopfen::ColAlpha, data(row, ColAlpha)},
                                {ModelHopfen::ColPellets, data(row, ColPellets)}});
    return bh->modelHopfen()->append(values);
}

void ModelHopfengaben::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColName))
        values.insert(ColName, bh->modelHopfen()->data(0, ModelHopfen::ColName));
    if (!values.contains(ColProzent))
        values.insert(ColProzent, 0);
    if (!values.contains(ColZeit))
        values.insert(ColZeit, 0);
    if (!values.contains(ColZeitpunkt))
        values.insert(ColZeitpunkt, static_cast<int>(Brauhelfer::HopfenZeitpunkt::Kochen));
}
