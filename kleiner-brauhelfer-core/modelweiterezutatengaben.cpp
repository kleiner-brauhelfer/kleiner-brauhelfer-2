#include "modelweiterezutatengaben.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelWeitereZutatenGaben::ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("ZugabeDatum");
    mVirtualField.append("EntnahmeDatum");
    mVirtualField.append("Abfuellbereit");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelWeitereZutatenGaben::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColZugabeDatum:
    {
        QDateTime braudatum = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColBraudatum).toDateTime();
        if (braudatum.isValid())
            return braudatum.addDays(data(idx.row(), ColZugabeNach).toInt());
        return QDateTime();
    }
    case ColEntnahmeDatum:
    {
        QDateTime zugabedatum = data(idx.row(), ColZugabeDatum).toDateTime();
        if (zugabedatum.isValid())
            return zugabedatum.addDays(data(idx.row(), ColZugabedauer).toInt() / 1440);
        return QDateTime();
    }
    case ColAbfuellbereit:
    {
        if (data(idx.row(), ColZeitpunkt).toInt() == EWZ_Zeitpunkt_Gaerung)
        {
            int Zugabestatus = data(idx.row(), ColZugabestatus).toInt();
            int Entnahmeindex = data(idx.row(), ColEntnahmeindex).toInt();
            if (Zugabestatus == EWZ_Zugabestatus_nichtZugegeben)
              return false;
            else if (Zugabestatus == EWZ_Zugabestatus_Zugegeben && Entnahmeindex == EWZ_Entnahmeindex_MitEntnahme)
                return false;
        }
        return true;
    }
    default:
        return QVariant();
    }
}

bool ModelWeitereZutatenGaben::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            if (data(idx.row(), ColTyp).toInt() == EWZ_Typ_Hopfen)
            {
                QSqlTableModel::setData(index(idx.row(), ColEinheit), EWZ_Einheit_g);
                QSqlTableModel::setData(index(idx.row(), ColTyp), EWZ_Typ_Hopfen);
                QSqlTableModel::setData(index(idx.row(), ColAusbeute), 0);
                QSqlTableModel::setData(index(idx.row(), ColFarbe), 0);
                QSqlTableModel::setData(index(idx.row(), ColZeitpunkt), EWZ_Zeitpunkt_Gaerung);
            }
            else
            {
                int row = bh->modelWeitereZutaten()->getRowWithValue(ModelWeitereZutaten::ColBeschreibung, value);
                if (row >= 0)
                {
                    QSqlTableModel::setData(index(idx.row(), ColEinheit), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColEinheiten));
                    QSqlTableModel::setData(index(idx.row(), ColTyp), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColTyp));
                    QSqlTableModel::setData(index(idx.row(), ColAusbeute), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColAusbeute));
                    QSqlTableModel::setData(index(idx.row(), ColFarbe), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColEBC));
                }
            }
            return true;
        }
        return false;
    }
    case ColMenge:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMenge).toDouble();
            QSqlTableModel::setData(index(idx.row(), Colerg_Menge), value.toDouble() * mengeSoll);
            return true;
        }
        return false;
    }
    case Colerg_Menge:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMenge).toDouble();
            QSqlTableModel::setData(index(idx.row(), ColMenge), value.toDouble() / mengeSoll);
            return true;
        }
        return false;
    }
    case ColTyp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            if (data(idx).toInt() == EWZ_Typ_Hopfen)
            {
                QSqlTableModel::setData(index(idx.row(), ColEinheit), EWZ_Einheit_g);
                QSqlTableModel::setData(index(idx.row(), ColTyp), EWZ_Typ_Hopfen);
                QSqlTableModel::setData(index(idx.row(), ColAusbeute), 0);
                QSqlTableModel::setData(index(idx.row(), ColFarbe), 0);
                QSqlTableModel::setData(index(idx.row(), ColZeitpunkt), EWZ_Zeitpunkt_Gaerung);
            }
            return true;
        }
        return false;
    }
    case ColZeitpunkt:
    {
        int prevZeitpunkt = idx.data().toInt();
        if (QSqlTableModel::setData(idx, value))
        {
            int zeitpunkt = value.toInt();
            if (prevZeitpunkt == EWZ_Zeitpunkt_Gaerung && zeitpunkt != EWZ_Zeitpunkt_Gaerung)
            {
                QModelIndex idx2 = index(idx.row(), ColZugabedauer);
                QSqlTableModel::setData(idx2, idx2.data().toInt() / 1440);
            }
            else if (prevZeitpunkt != EWZ_Zeitpunkt_Gaerung && zeitpunkt == EWZ_Zeitpunkt_Gaerung)
            {
                QModelIndex idx2 = index(idx.row(), ColZugabedauer);
                QSqlTableModel::setData(idx2, idx2.data().toInt() * 1440);
            }
            if (zeitpunkt != EWZ_Zeitpunkt_Gaerung)
            {
                QSqlTableModel::setData(index(idx.row(), ColZugabeNach), 0);
            }
            return true;
        }
        return false;
    }
    case ColZugabeDatum:
    {
        QDateTime braudatum = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColBraudatum).toDateTime();
        if (braudatum.isValid())
            return QSqlTableModel::setData(index(idx.row(), ColZugabeNach), braudatum.daysTo(value.toDateTime()));
        return false;
    }
    case ColEntnahmeDatum:
    {
        QDateTime zugabedatum = data(idx.row(), ColZugabeDatum).toDateTime();
        if (zugabedatum.isValid())
            return QSqlTableModel::setData(index(idx.row(), ColZugabedauer), zugabedatum.daysTo(value.toDateTime()) * 1440);
        return false;
    }
    default:
        return false;
    }
}

void ModelWeitereZutatenGaben::onSudDataChanged(const QModelIndex &idx)
{
    if (idx.column() == ModelSud::ColMenge)
    {
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        for (int r = 0; r < rowCount(); ++r)
        {
            if (data(r, ColSudID) == sudId)
            {
                QModelIndex idx2 = index(r, ColMenge);
                setData(idx2, data(idx2));
            }
        }
        mSignalModifiedBlocked = false;
    }
    else if (idx.column() == ModelSud::ColStatus)
    {
        int status = idx.data().toInt();
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        if (status == Sud_Status_Rezept)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId)
                    QSqlTableModel::setData(index(row, ColZugabestatus), EWZ_Zugabestatus_nichtZugegeben);
            }
        }
        else if (status == Sud_Status_Gebraut)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId && data(row, ColZugabeNach).toInt() == 0)
                    QSqlTableModel::setData(index(row, ColZugabestatus), EWZ_Zugabestatus_Zugegeben);
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelWeitereZutatenGaben::defaultValues(QMap<int, QVariant> &values) const
{
    if (values.contains(ColSudID))
    {
        QVariant sudId = values.value(ColSudID);
        if (!values.contains(ColZugabeNach))
        {
            if (bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt() != Sud_Status_Rezept)
            {
                QDateTime braudatum = bh->modelSud()->dataSud(sudId, ModelSud::ColBraudatum).toDateTime();
                if (braudatum.isValid())
                    values.insert(ColZugabeNach, braudatum.daysTo(QDateTime::currentDateTime()));
            }
        }
    }
    if (values.contains(ColTyp) && values.value(ColTyp).toInt() == EWZ_Typ_Hopfen)
    {
        if (!values.contains(ColName))
            values.insert(ColName, bh->modelHopfen()->data(0, ModelHopfen::ColBeschreibung));
    }
    else
    {
        if (!values.contains(ColName))
            values.insert(ColName, bh->modelWeitereZutaten()->data(0, ModelWeitereZutaten::ColBeschreibung));
    }
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColZugabeNach))
        values.insert(ColZugabeNach, 0);
}
