// clazy:excludeall=skipped-base-method
#include "modelmaischplan.h"
#include "brauhelfer.h"
#include "biercalc.h"

ModelMaischplan::ModelMaischplan(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    mUpdating(false)
{
    mVirtualField.append(QStringLiteral("MengeWasser"));
    mVirtualField.append(QStringLiteral("MengeMalz"));
    mVirtualField.append(QStringLiteral("MengeMaische"));

    // connect update sud to swap (move up & down)
}

QVariant ModelMaischplan::dataExt(const QModelIndex &idx) const
{
    double total;
    QVariant sudId;
    switch(idx.column())
    {
    case ColMengeWasser:
        sudId = data(idx.row(), ColSudID);
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
        return data(idx.row(), ColAnteilWasser).toDouble() / 100 * total;
    case ColMengeMalz:
        sudId = data(idx.row(), ColSudID);
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_S_Gesamt).toDouble();
        return data(idx.row(), ColAnteilMalz).toDouble() / 100 * total;
    case ColMengeMaische:
        sudId = data(idx.row(), ColSudID);
        total = BierCalc::MalzVerdraengung * data(idx.row(), ColTotalMengeMalz).toDouble() + data(idx.row(), ColTotalMengeWasser).toDouble();
        return data(idx.row(), ColAnteilMaische).toDouble() / 100 * total;
    default:
        return QVariant();
    }
}

bool ModelMaischplan::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    bool ret = false;
    double total;
    double Tm = 0, m1 = 0, c1 = 0, T1 = 0, m2 = 0, c2 = 0, T2 = 0, m = 0;
    QVariant sudId = data(idx.row(), ColSudID);
    Brauhelfer::RastTyp rastTyp = static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt());
    switch(idx.column())
    {
    case ColMengeWasser:
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
        ret = setData(idx.row(), ColAnteilWasser, value.toDouble() / total * 100);
        break;
    case ColMengeMalz:
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_S_Gesamt).toDouble();
        ret = setData(idx.row(), ColAnteilMalz, value.toDouble() / total * 100);
        break;
    case ColMengeMaische:
        total = BierCalc::MalzVerdraengung * data(idx.row(), ColTotalMengeMalz).toDouble() + data(idx.row(), ColTotalMengeWasser).toDouble();
        ret = setData(idx.row(), ColAnteilMaische, value.toDouble() / total * 100);
        break;


    case ColAnteilWasser:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTempMaische).toDouble(),
                                                    data(idx.row(), ColTotalMengeMalz).toDouble(),
                                                    data(idx.row(), ColTempMalz).toDouble(),
                                                    data(idx.row(), ColTotalMengeWasser).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), Tm);
                break;
            default:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m1, c1);
                m2 = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
                c2 = BierCalc::cWasser;
                Tm = data(idx.row(), ColTempMaische).toDouble();
                T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), T2);
            }
            ret = true;
        }
        break;
    case ColAnteilMalz:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTempMaische).toDouble(),
                                                    data(idx.row(), ColTotalMengeMalz).toDouble(),
                                                    data(idx.row(), ColTempMalz).toDouble(),
                                                    data(idx.row(), ColTotalMengeWasser).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), Tm);
                break;
            default:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m1, c1);
                m2 = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
                c2 = BierCalc::cWasser;
                Tm = data(idx.row(), ColTempMaische).toDouble();
                T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), T2);
                break;
            }
            ret = true;
        }
        break;
    case ColTempWasser:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Einmaischen:
                break;
            case Brauhelfer::RastTyp::Aufheizen:
                break;
            case Brauhelfer::RastTyp::Zubruehen:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m1, c1);
                c2 = BierCalc::cWasser;
                T2 = value.toDouble();
                Tm = data(idx.row(), ColTempMaische).toDouble();
                m2 = BierCalc::mischungstemperaturM2(Tm, m1, c1, T1, c2, T2);
                setData(index(idx.row(), ColMengeMaische), m2);
                break;
            case Brauhelfer::RastTyp::Zuschuetten:
                break;
            case Brauhelfer::RastTyp::Dekoktion:
                break;
            }
            ret = true;
        }
        break;

    case ColTempMalz:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTempMaische).toDouble(),
                                                    data(idx.row(), ColTotalMengeMalz).toDouble(),
                                                    value.toDouble(),
                                                    data(idx.row(), ColTotalMengeWasser).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), Tm);
                break;
            case Brauhelfer::RastTyp::Aufheizen:
                break;
            case Brauhelfer::RastTyp::Zubruehen:
                break;
            case Brauhelfer::RastTyp::Zuschuetten:
                break;
            case Brauhelfer::RastTyp::Dekoktion:
                break;
            }
            ret = true;
        }
        break;

    case ColTempMaische:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(value.toDouble(),
                                                    data(idx.row(), ColTotalMengeMalz).toDouble(),
                                                    data(idx.row(), ColTempMalz).toDouble(),
                                                    data(idx.row(), ColTotalMengeWasser).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), Tm);
                break;
            case Brauhelfer::RastTyp::Aufheizen:
                break;
            case Brauhelfer::RastTyp::Zubruehen:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m1, c1);
                c2 = BierCalc::cWasser;
                m2 = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
                Tm = value.toDouble();
                T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                QSqlTableModel::setData(index(idx.row(), ColTempWasser), T2);
                break;

            case Brauhelfer::RastTyp::Zuschuetten:
                break;

            case Brauhelfer::RastTyp::Dekoktion:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m, c1);
                c2 = c1;
                T2 = data(idx.row(), ColTempExtra2).toDouble();
                Tm = value.toDouble();
                m = BierCalc::mischungstemperaturM2(Tm, 1, c1, T1, c2, T2);
                if (m != 0)
                {
                    m = m / (1 + m) * 100;
                    if (m >= 0)
                        QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), m);
                }
                break;
            }
            ret = true;
        }
        break;

    case ColAnteilMaische:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (rastTyp)
            {
            case Brauhelfer::RastTyp::Dekoktion:
                T1 = getPreviousTempMaische(sudId, idx.row());
                getMaischeValues(sudId, idx.row(), m, c1);
                m2 = value.toDouble() * m;
                m1 = m - m2;
                c2 = c1;
                Tm = data(idx.row(), ColTempMaische).toDouble();
                T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                QSqlTableModel::setData(index(idx.row(), ColTempExtra2), T2);
                break;
            default:
                break;
            }
            ret = true;
        }
        break;

    default:
        break;
    }
    if (ret)
        updateSud(sudId);
    return ret;
}

void ModelMaischplan::updateSud(const QVariant &sudId)
{
    if (mUpdating)
        return;
    mUpdating = true;
    mSignalModifiedBlocked = true;
    ProxyModel model;
    model.setSourceModel(this);
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    double mengeWasser = 0, mengeMalz = 0;
    for (int r = 0; r < model.rowCount(); r++)
    {
        int rSrc = model.mapRowToSource(r);
        mengeWasser += model.data(r, ColMengeWasser).toDouble();
        mengeMalz += model.data(r, ColMengeMalz).toDouble();
        QSqlTableModel::setData(model.index(rSrc, ColTotalMengeWasser), mengeWasser);
        QSqlTableModel::setData(model.index(rSrc, ColTotalMengeMalz), mengeMalz);
    }
    mSignalModifiedBlocked = false;
    mUpdating = false;
}

double ModelMaischplan::getPreviousTempMaische(const QVariant &sudId, int fromRow) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelMaischplan*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    fromRow = model.mapRowFromSource(fromRow);
    if (fromRow > 0)
        return model.data(fromRow - 1, ColTempMaische).toDouble();
    return model.data(fromRow, ColTempMaische).toDouble();
}

double ModelMaischplan::getPreviousMengeWasser(const QVariant &sudId, int fromRow) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelMaischplan*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    fromRow = model.mapRowFromSource(fromRow);
    if (fromRow > 0)
        return model.data(fromRow - 1, ColTotalMengeWasser).toDouble();
    return 0;
}

void ModelMaischplan::getMaischeValues(const QVariant &sudId, int fromRow, double& m, double &c)
{
    double m_malz = bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble();
    double m_wasser = getPreviousMengeWasser(sudId, fromRow) * BierCalc::dichteWasser(20);
    m = m_malz + m_wasser;
    if (m != 0)
        c = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m;
}


