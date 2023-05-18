// clazy:excludeall=skipped-base-method
#include "modelrasten.h"
#include "brauhelfer.h"
#include "biercalc.h"

ModelRasten::ModelRasten(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    mUpdating(false)
{
    mVirtualField.append(QStringLiteral("Menge"));
}

QVariant ModelRasten::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColMenge:
    {
        double V = 0.0;
        QVariant sudId = data(idx.row(), ColSudID);
        switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
        case Brauhelfer::RastTyp::Infusion:
            V = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
            break;
        case Brauhelfer::RastTyp::Temperatur:
        case Brauhelfer::RastTyp::Dekoktion:
            V = BierCalc::MalzVerdraengung * bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble() + getPreviousMenge(sudId, idx.row());
            break;
        }
        return data(idx.row(), ModelRasten::ColMengenfaktor).toDouble() * V;
    }
    default:
        return QVariant();
    }
}

bool ModelRasten::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    bool ret = false;
    double fVal, m1, c1, m2, c2, T1, T2, Tm, V = 0.0, temp = 0.0;
    QVariant sudId = data(idx.row(), ColSudID);
    switch(idx.column())
    {
    case ColMenge:
        switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
        case Brauhelfer::RastTyp::Infusion:
            V = bh->modelSud()->dataSud(sudId, ModelSud::Colerg_WHauptguss).toDouble();
            break;
        case Brauhelfer::RastTyp::Temperatur:
        case Brauhelfer::RastTyp::Dekoktion:
            V = BierCalc::MalzVerdraengung * bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble() + getPreviousMenge(sudId, idx.row());
            break;
        }
        if (V != 0.0)
        {
            temp = value.toDouble() / V;
            if (temp >= 0)
                ret = setData(idx.row(), ColMengenfaktor, temp);
        }
        break;
    case ColMengenfaktor:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTemp).toDouble(),
                                                    bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble(),
                                                    data(idx.row(), ColParam3).toDouble(),
                                                    data(idx.row(), ColMenge).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColParam1), Tm);
                break;
            case Brauhelfer::RastTyp::Infusion:
                T1 = getPreviousTemp(sudId, idx.row());
                if (T1 != std::numeric_limits<double>::infinity())
                {
                    getMaischeValues(sudId, idx.row(), m1, c1);
                    m2 = data(idx.row(), ColMenge).toDouble() * BierCalc::dichteWasser(20);
                    c2 = BierCalc::cWasser;
                    Tm = data(idx.row(), ColTemp).toDouble();
                    T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                    QSqlTableModel::setData(index(idx.row(), ColParam1), T2);
                }
                break;
            case Brauhelfer::RastTyp::Dekoktion:
                T1 = getPreviousTemp(sudId, idx.row());
                if (T1 != std::numeric_limits<double>::infinity())
                {
                    fVal = value.toDouble();
                    if (fVal < 0)
                        fVal = 0;
                    getMaischeValues(sudId, idx.row(), temp, c1);
                    m2 = fVal * temp;
                    m1 = temp - m2;
                    c2 = c1;
                    Tm = data(idx.row(), ColTemp).toDouble();
                    T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                    QSqlTableModel::setData(index(idx.row(), ColParam1), T2);
                }
                break;
            default:
                break;
            }
            ret = true;
        }
        break;
    case ColTemp:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTemp).toDouble(),
                                                    bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble(),
                                                    data(idx.row(), ColParam3).toDouble(),
                                                    data(idx.row(), ColMenge).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColParam1), Tm);
                break;
            case Brauhelfer::RastTyp::Infusion:
                T1 = getPreviousTemp(sudId, idx.row());
                if (T1 != std::numeric_limits<double>::infinity())
                {
                    getMaischeValues(sudId, idx.row(), m1, c1);
                    c2 = BierCalc::cWasser;
                    m2 = data(idx.row(), ColMenge).toDouble() * BierCalc::dichteWasser(20);
                    Tm = value.toDouble();
                    T2 = BierCalc::mischungstemperaturT2(Tm, m1, c1, T1, m2, c2);
                    QSqlTableModel::setData(index(idx.row(), ColParam1), T2);
                }
                else
                {
                    QSqlTableModel::setData(index(idx.row(), ColParam1), value.toDouble());
                }
                break;
            case Brauhelfer::RastTyp::Dekoktion:
                T1 = getPreviousTemp(sudId, idx.row());
                if (T1 != std::numeric_limits<double>::infinity())
                {
                    getMaischeValues(sudId, idx.row(), temp, c1);
                    c2 = c1;
                    T2 = data(idx.row(), ColParam1).toDouble();
                    Tm = value.toDouble();
                    temp = BierCalc::mischungstemperaturM2(Tm, 1, c1, T1, c2, T2);
                    if (temp != 0)
                    {
                        temp = temp / (1 + temp);
                        if (temp >= 0)
                            QSqlTableModel::setData(index(idx.row(), ColMengenfaktor), temp);
                    }
                }
                else
                {
                    QSqlTableModel::setData(index(idx.row(), ColParam1), value.toDouble());
                }
                break;
            default:
                break;
            }
            ret = true;
        }
        break;
    case ColParam1:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
            {
            case Brauhelfer::RastTyp::Infusion:
                T1 = getPreviousTemp(sudId, idx.row());
                if (T1 != std::numeric_limits<double>::infinity())
                {
                    getMaischeValues(sudId, idx.row(), m1, c1);
                    c2 = BierCalc::cWasser;
                    T2 = value.toDouble();
                    Tm = data(idx.row(), ColTemp).toDouble();
                    m2 = BierCalc::mischungstemperaturM2(Tm, m1, c1, T1, c2, T2);
                    setData(index(idx.row(), ColMenge), m2);
                }
                else
                {
                    QSqlTableModel::setData(index(idx.row(), ColTemp), value.toDouble());
                }
                break;
            default:
                break;
            }
            ret = true;
        }
        break;
    case ColParam3:
        if (QSqlTableModel::setData(idx, value))
        {
            switch (static_cast<Brauhelfer::RastTyp>(data(idx.row(), ColTyp).toInt()))
            {
            case Brauhelfer::RastTyp::Einmaischen:
                Tm = BierCalc::einmaischetemperatur(data(idx.row(), ColTemp).toDouble(),
                                                    bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble(),
                                                    data(idx.row(), ColParam3).toDouble(),
                                                    data(idx.row(), ColMenge).toDouble());
                QSqlTableModel::setData(index(idx.row(), ColParam1), Tm);
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
        update(sudId);
    return ret;
}

void ModelRasten::getMaischeValues(const QVariant &sudId, int fromRow, double& m, double &c)
{
    double m_malz = bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::Colerg_S_Gesamt).toDouble();
    double m_wasser = getPreviousMenge(sudId, fromRow) * BierCalc::dichteWasser(20);
    m = m_malz + m_wasser;
    if (m != 0)
        c = (m_malz * BierCalc::cMalz + m_wasser * BierCalc::cWasser) / m;
}

double ModelRasten::getPreviousTemp(const QVariant &sudId, int fromRow) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelRasten*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    fromRow = model.mapRowFromSource(fromRow);
    if (fromRow > 0)
        return model.data(fromRow - 1, ColTemp).toDouble();
    return std::numeric_limits<double>::infinity();
}

double ModelRasten::getPreviousMenge(const QVariant &sudId, int fromRow) const
{
    ProxyModel model;
    model.setSourceModel(const_cast<ModelRasten*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    if (fromRow >= 0)
        fromRow = model.mapRowFromSource(fromRow);
    else
        fromRow = model.rowCount();
    double menge = 0.0;
    for (int r = 0; r < fromRow; r++)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(r, ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
        case Brauhelfer::RastTyp::Infusion:
            menge += model.data(r, ColMenge).toDouble();
            break;
        default:
            break;
        }
    }
    return menge;
}

void ModelRasten::update(const QVariant &sudId)
{
    if (mUpdating)
        return;
    mUpdating = true;
    ProxyModel model;
    model.setSourceModel(this);
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    mSignalModifiedBlocked = true;
    for (int r = 0; r < model.rowCount(); r++)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(r, ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
        case Brauhelfer::RastTyp::Temperatur:
            break;
        case Brauhelfer::RastTyp::Infusion:
        case Brauhelfer::RastTyp::Dekoktion:
            model.setData(r, ColTemp, model.data(r, ColTemp));
            break;
        }
    }
    mSignalModifiedBlocked = false;
    mUpdating = false;
}

void ModelRasten::defaultValues(QMap<int, QVariant> &values) const
{
    if (!values.contains(ColTyp))
        values.insert(ColTyp, static_cast<int>(Brauhelfer::RastTyp::Temperatur));
    if (!values.contains(ColMengenfaktor))
        values.insert(ColMengenfaktor, 1);
}
