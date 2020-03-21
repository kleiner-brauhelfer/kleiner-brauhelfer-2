#include "modelhopfengaben.h"
#include "brauhelfer.h"

ModelHopfengaben::ModelHopfengaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("IBUAnteil");
    mVirtualField.append("Ausbeute");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
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
        double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMenge).toDouble();
        return menge * alpha * ausbeute / (10 * mengeSoll);
    }
    case ColAusbeute:
    {
        QVariant sudId = data(idx.row(), ColSudID);
        double sw = bh->modelSud()->dataSud(sudId, ModelSud::ColSWSollKochende).toDouble();
        double isozeit = bh->modelSud()->dataSud(sudId, ModelSud::ColNachisomerisierungszeit).toDouble();
        double zeit = data(idx.row(), ColZeit).toDouble();
        double ausbeute = BierCalc::hopfenAusbeute(zeit + isozeit, sw);
        if (data(idx.row(), ColPellets).toBool())
            ausbeute *= 1.1;
        if (data(idx.row(), ColVorderwuerze).toBool())
            ausbeute *= 0.9;
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
            int row = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColBeschreibung, value);
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
            double mengeSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColMenge).toDouble();
            double ibuSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColIBU).toDouble();
            switch (bh->modelSud()->dataSud(sudId, ModelSud::ColberechnungsArtHopfen).toInt())
            {
            case Hopfen_Berechnung_Keine:
            {
                break;
            }
            case Hopfen_Berechnung_Gewicht:
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
            case Hopfen_Berechnung_IBU:
            {
                double alpha = data(idx.row(), ColAlpha).toDouble();
                double ausbeute = data(idx.row(), ColAusbeute).toDouble();
                if (alpha != 0.0 && ausbeute != 0.0)
                {
                    double anteil = ibuSoll * fVal / 100 ;
                    double menge = (anteil * mengeSoll * 10) / (alpha * ausbeute);
                    QSqlTableModel::setData(index(idx.row(), Colerg_Menge), menge);
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
            switch (bh->modelSud()->dataSud(sudId, ModelSud::ColberechnungsArtHopfen).toInt())
            {
            case Hopfen_Berechnung_Keine:
            {
                break;
            }
            case Hopfen_Berechnung_Gewicht:
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
            case Hopfen_Berechnung_IBU:
            {
                double mengeSoll = bh->modelSud()->dataSud(sudId, ModelSud::ColMenge).toDouble();
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
    case ColVorderwuerze:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            if (value.toBool())
            {
                int dauer = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColKochdauerNachBitterhopfung).toInt();
                QSqlTableModel::setData(index(idx.row(), ColZeit), dauer);
            }
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
    case ModelSud::ColSW:
    case ModelSud::ColIBU:
    case ModelSud::ColberechnungsArtHopfen:
    case ModelSud::ColhighGravityFaktor:
    case ModelSud::ColKochdauerNachBitterhopfung:
    case ModelSud::ColNachisomerisierungszeit:
    {
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        int colUpdate = idx.column() == ModelSud::ColberechnungsArtHopfen ? Colerg_Menge : ColProzent;
        for (int r = 0; r < rowCount(); ++r)
        {
            if (data(r, ColSudID) == sudId)
            {
                if (idx.column() == ModelSud::ColKochdauerNachBitterhopfung)
                {
                    int max = idx.data().toInt();
                    if (data(r, ColVorderwuerze).toBool())
                    {
                        QSqlTableModel::setData(index(r, ColZeit), max);
                    }
                    else
                    {
                        QModelIndex idx2 = index(r, ColZeit);
                        if (idx2.data().toInt() > max)
                            QSqlTableModel::setData(idx2, max);
                    }
                }
                else if (idx.column() == ModelSud::ColNachisomerisierungszeit)
                {
                    int min = -1 * idx.data().toInt();
                    if (data(r, ColSudID) == sudId)
                    {
                        QModelIndex idx2 = index(r, ColZeit);
                        if (idx2.data().toInt() < min)
                            QSqlTableModel::setData(idx2, min);
                    }
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

void ModelHopfengaben::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColName))
        values.insert(ColName, bh->modelHopfen()->data(0, ModelHopfen::ColBeschreibung));
    if (!values.contains(ColProzent))
        values.insert(ColProzent, 0);
    if (!values.contains(ColZeit))
        values.insert(ColZeit, 0);
    if (!values.contains(ColVorderwuerze))
        values.insert(ColVorderwuerze, 0);
}
