#include "modelweiterezutatengaben.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelWeitereZutatenGaben::ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Extrakt");
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
    case ColExtrakt:
    {
        double ausbeute = data(idx.row(), ColAusbeute).toDouble();
        if (ausbeute > 0.0)
        {
            double menge = data(idx.row(), ColMenge).toDouble();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(idx.row(), ColEinheit).toInt());
            if (einheit != Brauhelfer::Einheit::Stk)
                menge /= 1000;
            return menge * ausbeute;
        }
        return 0;
    }
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
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(data(idx.row(), ColZeitpunkt).toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
        {
            Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(data(idx.row(), ColZugabestatus).toInt());
            Brauhelfer::ZusatzEntnahmeindex entnahmeindex = static_cast<Brauhelfer::ZusatzEntnahmeindex>(data(idx.row(), ColEntnahmeindex).toInt());
            if (status == Brauhelfer::ZusatzStatus::NichtZugegeben)
              return false;
            else if (status == Brauhelfer::ZusatzStatus::Zugegeben && entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme)
                return false;
        }
        return true;
    }
    default:
        return QVariant();
    }
}

int ModelWeitereZutatenGaben::import(int row)
{
    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(row, ColTyp).toInt());
    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
    {
        QMap<int, QVariant> values({{ModelHopfen::ColName, data(row, ColName)}});
        return bh->modelHopfen()->append(values);
    }
    else
    {
        QMap<int, QVariant> values({{ModelWeitereZutaten::ColName, data(row, ColName)},
                                    {ModelWeitereZutaten::ColEinheit, data(row, ColEinheit)},
                                    {ModelWeitereZutaten::ColTyp, data(row, ColTyp)},
                                    {ModelWeitereZutaten::ColAusbeute, data(row, ColAusbeute)},
                                    {ModelWeitereZutaten::ColFarbe, data(row, ColFarbe)}});
        return bh->modelWeitereZutaten()->append(values);
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
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(idx.row(), ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            {
                QSqlTableModel::setData(index(idx.row(), ColEinheit), static_cast<int>(Brauhelfer::Einheit::g));
                QSqlTableModel::setData(index(idx.row(), ColTyp), static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen));
                QSqlTableModel::setData(index(idx.row(), ColAusbeute), 0);
                QSqlTableModel::setData(index(idx.row(), ColFarbe), 0);
                QSqlTableModel::setData(index(idx.row(), ColZeitpunkt), static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung));
            }
            else
            {
                int row = bh->modelWeitereZutaten()->getRowWithValue(ModelWeitereZutaten::ColName, value);
                if (row >= 0)
                {
                    QSqlTableModel::setData(index(idx.row(), ColEinheit), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColEinheit));
                    QSqlTableModel::setData(index(idx.row(), ColTyp), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColTyp));
                    QSqlTableModel::setData(index(idx.row(), ColAusbeute), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColAusbeute));
                    QSqlTableModel::setData(index(idx.row(), ColFarbe), bh->modelWeitereZutaten()->data(row, ModelWeitereZutaten::ColFarbe));
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
            double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMengeSoll).toDouble();
            QSqlTableModel::setData(index(idx.row(), Colerg_Menge), value.toDouble() * mengeSoll);
            return true;
        }
        return false;
    }
    case Colerg_Menge:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            double mengeSoll = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColMengeSoll).toDouble();
            QSqlTableModel::setData(index(idx.row(), ColMenge), value.toDouble() / mengeSoll);
            return true;
        }
        return false;
    }
    case ColTyp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(data(idx).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            {
                QSqlTableModel::setData(index(idx.row(), ColEinheit), static_cast<int>(Brauhelfer::Einheit::g));
                QSqlTableModel::setData(index(idx.row(), ColTyp), static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen));
                QSqlTableModel::setData(index(idx.row(), ColAusbeute), 0);
                QSqlTableModel::setData(index(idx.row(), ColFarbe), 0);
                QSqlTableModel::setData(index(idx.row(), ColZeitpunkt), static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung));
            }
            return true;
        }
        return false;
    }
    case ColZeitpunkt:
    {
        Brauhelfer::ZusatzZeitpunkt prevZeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(idx.data().toInt());
        if (QSqlTableModel::setData(idx, value))
        {
            Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(value.toInt());
            if (prevZeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung && zeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QModelIndex idx2 = index(idx.row(), ColZugabedauer);
                QSqlTableModel::setData(idx2, idx2.data().toInt() / 1440);
            }
            else if (prevZeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung && zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QModelIndex idx2 = index(idx.row(), ColZugabedauer);
                QSqlTableModel::setData(idx2, idx2.data().toInt() * 1440);
            }
            if (zeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                QSqlTableModel::setData(index(idx.row(), ColZugabeNach), 0);
            }
            return true;
        }
        return false;
    }
    case ColExtrakt:
    {
        double ausbeute = data(idx.row(), ColAusbeute).toDouble();
        if (ausbeute > 0.0)
        {
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(idx.row(), ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Stk)
                return setData(index(idx.row(), ColMenge), value.toDouble() / ausbeute);
            else
                return setData(index(idx.row(), ColMenge), value.toDouble() / ausbeute * 1000);
        }
        return true;
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
    if (idx.column() == ModelSud::ColMenge || idx.column() == ModelSud::ColMengeSoll)
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
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(idx.data().toInt());
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        if (status == Brauhelfer::SudStatus::Rezept)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId)
                    QSqlTableModel::setData(index(row, ColZugabestatus), static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben));
            }
        }
        else if (status == Brauhelfer::SudStatus::Gebraut)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId && data(row, ColZugabeNach).toInt() == 0)
                    QSqlTableModel::setData(index(row, ColZugabestatus), static_cast<int>(Brauhelfer::ZusatzStatus::Zugegeben));
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
            Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt());
            if (status != Brauhelfer::SudStatus::Rezept)
            {
                QDateTime braudatum = bh->modelSud()->dataSud(sudId, ModelSud::ColBraudatum).toDateTime();
                if (braudatum.isValid())
                    values.insert(ColZugabeNach, braudatum.daysTo(QDateTime::currentDateTime()));
            }
        }
    }
    if (values.contains(ColTyp))
    {
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(values.value(ColTyp).toInt());
        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        {
            if (!values.contains(ColName))
                values.insert(ColName, bh->modelHopfen()->data(0, ModelHopfen::ColName));
        }
        else
        {
            if (!values.contains(ColName))
                values.insert(ColName, bh->modelWeitereZutaten()->data(0, ModelWeitereZutaten::ColName));
        }
    }
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColZugabeNach))
        values.insert(ColZugabeNach, 0);
}
