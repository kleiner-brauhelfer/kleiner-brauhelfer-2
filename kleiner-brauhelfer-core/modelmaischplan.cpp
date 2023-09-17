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
    mVirtualField.append(QStringLiteral("TotalMengeWasser"));
    mVirtualField.append(QStringLiteral("MengeMalz"));
    mVirtualField.append(QStringLiteral("TotalMengeMalz"));
    mVirtualField.append(QStringLiteral("MengeMaische"));
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelMaischplan::onSudDataChanged);
    connect(this, &SqlTableModel::rowsSwapped, this, &ModelMaischplan::onRowsSwapped);
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
    case ColTotalMengeWasser:
        sudId = data(idx.row(), ColSudID);
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
        return getTotalAnteil(sudId, idx.row(), ColAnteilWasser) / 100 * total;
    case ColMengeMalz:
        sudId = data(idx.row(), ColSudID);
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_S_Gesamt).toDouble();
        return data(idx.row(), ColAnteilMalz).toDouble() / 100 * total;
    case ColTotalMengeMalz:
        sudId = data(idx.row(), ColSudID);
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_S_Gesamt).toDouble();
        return getTotalAnteil(sudId, idx.row(), ColAnteilMalz) / 100 * total;;
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
    QVariant sudId = data(idx.row(), ColSudID);
    switch(idx.column())
    {
    case ColSudID:
        if (QSqlTableModel::setData(idx, value))
        {
            updateSud(value);
            ret = true;
        }
        break;
    case ColMengeWasser:
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
        ret = setData(idx.row(), ColAnteilWasser, value.toDouble() / total * 100);
        break;
    case ColTotalMengeWasser:
        qWarning(Brauhelfer::loggingCategory) << "ModelMaischplan::setData(ColTotalMengeWasser) not implemented";
        return true;
    case ColMengeMalz:
        total = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_S_Gesamt).toDouble();
        ret = setData(idx.row(), ColAnteilMalz, value.toDouble() / total * 100);
        break;
    case ColTotalMengeMalz:
        qWarning(Brauhelfer::loggingCategory) << "ModelMaischplan::setData(ColTotalMengeMalz) not implemented";
        return true;
    case ColMengeMaische:
        total = BierCalc::MalzVerdraengung * data(idx.row(), ColTotalMengeMalz).toDouble() + data(idx.row(), ColTotalMengeWasser).toDouble();
        ret = setData(idx.row(), ColAnteilMaische, value.toDouble() / total * 100);
        break;
    case ColTyp:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (static_cast<Brauhelfer::RastTyp>(value.toInt()))
            {
            case Brauhelfer::RastTyp::Einmaischen:
                QSqlTableModel::setData(index(idx.row(), ColAnteilWasser), 100);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMalz), 100);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), 0);
                QSqlTableModel::setData(index(idx.row(), ColTempRast), getPreviousValueTemperature(idx.row()));
                QSqlTableModel::setData(index(idx.row(), ColDauerRast), 5);
                QSqlTableModel::setData(index(idx.row(), ColTempMalz), 21);
                break;
            case Brauhelfer::RastTyp::Aufheizen:
                QSqlTableModel::setData(index(idx.row(), ColAnteilWasser), 0);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMalz), 0);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), 0);
                QSqlTableModel::setData(index(idx.row(), ColTempRast), getPreviousValueTemperature(idx.row()));
                QSqlTableModel::setData(index(idx.row(), ColDauerRast), 15);
                QSqlTableModel::setData(index(idx.row(), ColTempMalz), 0);
                break;
            case Brauhelfer::RastTyp::Zubruehen:
                QSqlTableModel::setData(index(idx.row(), ColAnteilWasser), 33.33);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMalz), 0);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), 0);
                QSqlTableModel::setData(index(idx.row(), ColTempRast), getPreviousValueTemperature(idx.row()));
                QSqlTableModel::setData(index(idx.row(), ColDauerRast), 15);
                QSqlTableModel::setData(index(idx.row(), ColTempMalz), 0);
                break;
            case Brauhelfer::RastTyp::Zuschuetten:
                QSqlTableModel::setData(index(idx.row(), ColAnteilWasser), 20);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMalz), 20);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), 0);
                QSqlTableModel::setData(index(idx.row(), ColTempRast), getPreviousValueTemperature(idx.row()));
                QSqlTableModel::setData(index(idx.row(), ColDauerRast), 15);
                QSqlTableModel::setData(index(idx.row(), ColTempMalz), 21);
                break;
            case Brauhelfer::RastTyp::Dekoktion:
                QSqlTableModel::setData(index(idx.row(), ColAnteilWasser), 0);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMalz), 0);
                QSqlTableModel::setData(index(idx.row(), ColDauerRast), 15);
                QSqlTableModel::setData(index(idx.row(), ColTempMalz), 0);
                QSqlTableModel::setData(index(idx.row(), ColTempExtra1), 95);
                QSqlTableModel::setData(index(idx.row(), ColDauerExtra1), 15);
                QSqlTableModel::setData(index(idx.row(), ColTempExtra2), 0);
                QSqlTableModel::setData(index(idx.row(), ColDauerExtra2), 0);
                setData(index(idx.row(), ColAnteilMaische), 50);
                break;
            }
            ret = true;
        }
        break;
    case ColTempRast:
        if (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()) == Brauhelfer::RastTyp::Dekoktion)
        {
            if (QSqlTableModel::setData(idx, value))
            {
                double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
                getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
                double T_rast = data(idx.row(), ColTempRast).toDouble();
              #if 0 // -> ColTempExtra1
                double m_maische = data(idx.row(), ColAnteilMaische).toDouble() / 100 * m_maische0;
                double T_maische = BierCalc::mischungstemperaturT2(T_rast, m_maische0 - m_maische, c_maische0, T_maische0, m_maische, c_maische0);
                QSqlTableModel::setData(index(idx.row(), ColTempExtra1), T_maische);
              #else // -> ColAnteilMaische
                double T_maische = data(idx.row(), ColTempExtra1).toDouble();
                double anteil = (T_rast - T_maische0) / (T_maische - T_maische0);
                QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), anteil * 100);
              #endif
                ret = true;
            }
            break;
        }
        [[fallthrough]];
    case ColAnteilWasser:
    case ColAnteilMalz:
        if (QSqlTableModel::setData(idx, value))
        {
            double m_wasser = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
            double m_malz = data(idx.row(), ColMengeMalz).toDouble();
            if (m_wasser > 0 || m_malz > 0)
            {
                double T_rast = data(idx.row(), ColTempRast).toDouble();
                double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
                getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
                if (m_wasser > 0)
                {
                    double T_malz = data(idx.row(), ColTempMalz).toDouble();
                    double m_zugabe = m_malz + m_wasser;
                    double c_zugabe = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m_zugabe;
                    double T_zugabe = BierCalc::mischungstemperaturT2(T_rast, m_maische0, c_maische0, T_maische0, m_zugabe, c_zugabe);
                    double T_wasser = BierCalc::mischungstemperaturT2(T_zugabe, m_malz, BierCalc::cMalz, T_malz, m_wasser, BierCalc::cWasser);
                    QSqlTableModel::setData(index(idx.row(), ColTempWasser), T_wasser);
                }
                else
                {
                    double T_malz = BierCalc::mischungstemperaturT2(T_rast, m_maische0, c_maische0, T_maische0, m_malz, BierCalc::cMalz);
                    QSqlTableModel::setData(index(idx.row(), ColTempMalz), T_malz);
                }
            }
            ret = true;
        }
        break;
    case ColTempWasser:
        if (QSqlTableModel::setData(idx, value))
        {
            double m_wasser = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
            double m_malz = data(idx.row(), ColMengeMalz).toDouble();
            if (m_wasser > 0 || m_malz > 0)
            {
                double T_wasser = data(idx.row(), ColTempWasser).toDouble();
                double T_malz = data(idx.row(), ColTempMalz).toDouble();
                double T_zugabe = BierCalc::mischungstemperaturTm(m_malz, BierCalc::cMalz, T_malz, m_wasser, BierCalc::cWasser, T_wasser);
                double m_zugabe = m_malz + m_wasser;
                double c_zugabe = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m_zugabe;
                double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
                getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
                double T_rast = BierCalc::mischungstemperaturTm(m_maische0, c_maische0, T_maische0, m_zugabe, c_zugabe, T_zugabe);
                QSqlTableModel::setData(index(idx.row(), ColTempRast), T_rast);
            }
            ret = true;
        }
        break;
    case ColTempMalz:
        if (QSqlTableModel::setData(idx, value))
        {
            double m_wasser = data(idx.row(), ColMengeWasser).toDouble() * BierCalc::dichteWasser(20);
            double m_malz = data(idx.row(), ColMengeMalz).toDouble();
            if (m_wasser > 0 || m_malz > 0)
            {
                double T_malz = data(idx.row(), ColTempMalz).toDouble();
                double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
                getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
                if (m_wasser > 0)
                {
                    double T_rast = data(idx.row(), ColTempRast).toDouble();
                    double m_zugabe = m_malz + m_wasser;
                    double c_zugabe = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m_zugabe;
                    double T_zugabe = BierCalc::mischungstemperaturT2(T_rast, m_maische0, c_maische0, T_maische0, m_zugabe, c_zugabe);
                    double T_wasser = BierCalc::mischungstemperaturT2(T_zugabe, m_malz, BierCalc::cMalz, T_malz, m_wasser, BierCalc::cWasser);
                    QSqlTableModel::setData(index(idx.row(), ColTempWasser), T_wasser);
                }
                else
                {
                    double T_rast = BierCalc::mischungstemperaturTm(m_maische0, c_maische0, T_maische0, m_malz, BierCalc::cMalz, T_malz);
                    QSqlTableModel::setData(index(idx.row(), ColTempRast), T_rast);
                }
            }
            ret = true;
        }
        break;
    case ColAnteilMaische:
        if (QSqlTableModel::setData(idx, value))
        {
            double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
            getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
            double m_maische = data(idx.row(), ColAnteilMaische).toDouble() / 100 * m_maische0;
          #if 0 // -> ColTempExtra1
            double T_rast = data(idx.row(), ColTempRast).toDouble();
            double T_maische = BierCalc::mischungstemperaturT2(T_rast, m_maische0 - m_maische, c_maische0, T_maische0, m_maische, c_maische0);
            QSqlTableModel::setData(index(idx.row(), ColTempExtra1), T_maische);
          #else // -> ColTempRast
            double T_maische = data(idx.row(), ColTempExtra1).toDouble();
            double T_rast = BierCalc::mischungstemperaturTm(m_maische0 - m_maische, c_maische0, T_maische0, m_maische, c_maische0, T_maische);
            QSqlTableModel::setData(index(idx.row(), ColTempRast), T_rast);
          #endif
            ret = true;
        }
        break;
    case ColTempExtra1:
        if (QSqlTableModel::setData(idx, value))
        {
            double m_maische0 = 0, c_maische0 = 0, T_maische0 = 0;
            getPreviousMaischeValues(idx.row(), m_maische0, c_maische0, T_maische0);
            double T_maische = data(idx.row(), ColTempExtra1).toDouble();
          #if 0 // -> ColTempRast
            double m_maische = data(idx.row(), ColAnteilMaische).toDouble() / 100 * m_maische0;
            double T_rast = BierCalc::mischungstemperaturTm(m_maische0 - m_maische, c_maische0, T_maische0, m_maische, c_maische0, T_maische);
            QSqlTableModel::setData(index(idx.row(), ColTempRast), T_rast);
          #else // -> ColAnteilMaische
            double T_rast = data(idx.row(), ColTempRast).toDouble();
            double anteil = (T_rast - T_maische0) / (T_maische - T_maische0);
            QSqlTableModel::setData(index(idx.row(), ColAnteilMaische), anteil * 100);
          #endif
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

double ModelMaischplan::getTotalAnteil(const QVariant &sudId, int rowFrom, int colAnteil) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelMaischplan*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    rowFrom = model.mapRowFromSource(rowFrom);
    double anteil = 0.0;
    for (int r = 0; r <= rowFrom; r++)
        anteil += model.data(r, colAnteil).toDouble();
    return anteil;
}

double ModelMaischplan::getPreviousValueTemperature(int row) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelMaischplan*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(data(row, ColSudID).toInt()));
    row = model.mapRowFromSource(row);
    if (row > 0)
    {
        return model.data(row - 1, ColTempRast).toDouble();
    }
    return 57;
}

void ModelMaischplan::getPreviousMaischeValues(int row, double& m, double &c, double &T) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelMaischplan*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(data(row, ColSudID).toInt()));
    row = model.mapRowFromSource(row);
    if (row > 0)
    {
        double m_malz = model.data(row - 1, ColTotalMengeMalz).toDouble();
        double m_wasser = model.data(row - 1, ColTotalMengeWasser).toDouble() * BierCalc::dichteWasser(20);
        m = m_malz + m_wasser;
        c = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m;
        T = model.data(row - 1, ColTempRast).toDouble();
    }
    else
    {
        m = 0;
        c = 0;
        T = 0;
    }
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
    for (int r = 0; r < model.rowCount(); r++)
        model.setData(r, ColTempRast, model.data(r, ColTempRast));
    mSignalModifiedBlocked = false;
    mUpdating = false;
}

void ModelMaischplan::onSudDataChanged(const QModelIndex &idx)
{
    switch (idx.column())
    {
    case ModelSud::Colerg_S_Gesamt:
    case ModelSud::Colerg_WHauptguss:
        updateSud(bh->modelSud()->data(idx.row(), ModelSud::ColID));
        break;
    }
}

void ModelMaischplan::onRowsSwapped(int row1, int row2)
{
    QVariant sudId1 = data(row1, ColSudID);
    QVariant sudId2 = data(row2, ColSudID);
    updateSud(sudId1);
    if (sudId2 != sudId1)
        updateSud(sudId2);
}
