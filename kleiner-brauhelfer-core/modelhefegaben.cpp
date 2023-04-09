// clazy:excludeall=skipped-base-method
#include "modelhefegaben.h"
#include "brauhelfer.h"
#include "proxymodelsud.h"
#include <QDateTime>
#include <cmath>

ModelHefegaben::ModelHefegaben(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append(QStringLiteral("ZugabeDatum"));
    mVirtualField.append(QStringLiteral("Abfuellbereit"));
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelHefegaben::onSudDataChanged);
}

QVariant ModelHefegaben::dataExt(const QModelIndex &idx) const
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
    case ColAbfuellbereit:
    {
        return data(idx.row(), ColZugegeben).toBool();
    }
    default:
        return QVariant();
    }
}

bool ModelHefegaben::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColZugabeDatum:
    {
        QDateTime braudatum = bh->modelSud()->dataSud(data(idx.row(), ColSudID), ModelSud::ColBraudatum).toDateTime();
        if (braudatum.isValid())
            return QSqlTableModel::setData(index(idx.row(), ColZugabeNach), braudatum.daysTo(value.toDateTime()));
        return false;
    }
    default:
        return false;
    }
}

void ModelHefegaben::onSudDataChanged(const QModelIndex &idx)
{
    if (idx.column() == ModelSud::ColStatus)
    {
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(idx.data().toInt());
        QVariant sudId = bh->modelSud()->data(idx.row(), ModelSud::ColID);
        mSignalModifiedBlocked = true;
        if (status == Brauhelfer::SudStatus::Rezept)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId)
                    QSqlTableModel::setData(index(row, ColZugegeben), false);
            }
        }
        else if (status == Brauhelfer::SudStatus::Gebraut)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(row, ColSudID) == sudId && data(row, ColZugabeNach).toInt() == 0)
                    QSqlTableModel::setData(index(row, ColZugegeben), true);
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelHefegaben::update(const QVariant &name, int col, const QVariant &value)
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

int ModelHefegaben::import(int row)
{
    QMap<int, QVariant> values({{ModelHefe::ColName, data(row, ColName)}});
    return bh->modelHefe()->append(values);
}

void ModelHefegaben::defaultValues(QMap<int, QVariant> &values) const
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
    if (!values.contains(ColName))
        values.insert(ColName, bh->modelHefe()->data(0, ModelHefe::ColName));
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 1);
    if (!values.contains(ColZugegeben))
        values.insert(ColZugegeben, 0);
    if (!values.contains(ColZugabeNach))
        values.insert(ColZugabeNach, 0);
}
