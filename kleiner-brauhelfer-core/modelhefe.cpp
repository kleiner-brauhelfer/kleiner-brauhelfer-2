// clazy:excludeall=skipped-base-method
#include "modelhefe.h"
#include "proxymodel.h"
#include "proxymodelsud.h"
#include "brauhelfer.h"
#include <QDate>

ModelHefe::ModelHefe(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    inGebrauch(QVector<bool>())
{
    mVirtualField.append(QStringLiteral("InGebrauch"));

    connect(this, &SqlTableModel::modelReset, this, &ModelHefe::resetInGebrauch);
    connect(this, &SqlTableModel::rowsInserted, this, &ModelHefe::resetInGebrauch);
    connect(this, &SqlTableModel::rowsRemoved, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::modelReset, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsInserted, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsRemoved, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelHefe::onSudDataChanged);
    connect(bh->modelHefegaben(), &SqlTableModel::modelReset, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelHefegaben(), &SqlTableModel::rowsInserted, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelHefegaben(), &SqlTableModel::rowsRemoved, this, &ModelHefe::resetInGebrauch);
    connect(bh->modelHefegaben(), &SqlTableModel::dataChanged, this, &ModelHefe::onHefegabenDataChanged);
}

QVariant ModelHefe::dataExt(const QModelIndex &idx) const
{
    switch(idx.column())
    {
    case ColEingelagert:
    {
        return QDateTime::fromString(QSqlTableModel::data(idx).toString(), Qt::ISODate);
    }
    case ColMindesthaltbar:
    {
        return QDateTime::fromString(QSqlTableModel::data(idx).toString(), Qt::ISODate);
    }
    case ColInGebrauch:
    {
        if (inGebrauch.size() == 0)
            updateInGebrauch();
        return inGebrauch[idx.row()];
    }
    default:
        return QVariant();
    }
}

bool ModelHefe::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString newName = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, newName))
        {
            bh->modelHefegaben()->update(prevName, ModelHefegaben::ColName, newName);
            return true;
        }
        return false;
    }
    case ColEingelagert:
    {
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
    }
    case ColMindesthaltbar:
    {
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
    }
    case ColMenge:
    {
        double prevValue = data(idx).toDouble();
        if (QSqlTableModel::setData(idx, value))
        {
            if (value.toDouble() > 0.0 && prevValue == 0.0)
            {
                setData(idx.row(), ColEingelagert, QDate::currentDate());
                setData(idx.row(), ColMindesthaltbar, QDate::currentDate().addYears(1));
            }
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

void ModelHefe::resetInGebrauch()
{
    inGebrauch.clear();
}

void ModelHefe::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelSud::ColStatus >= topLeft.column() && ModelSud::ColStatus <= bottomRight.column())
        resetInGebrauch();
}

void ModelHefe::onHefegabenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelHefegaben::ColSudID >= topLeft.column() && ModelHefegaben::ColSudID <= bottomRight.column())
        resetInGebrauch();
    if (ModelHefegaben::ColName >= topLeft.column() && ModelHefegaben::ColName <= bottomRight.column())
        resetInGebrauch();
}

void ModelHefe::updateInGebrauch() const
{
    QList<int> sudIds;
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept);
    for (int row = 0; row < modelSud.rowCount(); row++)
    {
        sudIds.append(modelSud.data(row, ModelSud::ColID).toInt());
    }

    QList<QString> hefegaben;
    ProxyModel modelHefegaben;
    modelHefegaben.setSourceModel(bh->modelHefegaben());
    for (int row = 0; row < modelHefegaben.rowCount(); row++)
    {
        int sudId = modelHefegaben.data(row, ModelHefegaben::ColSudID).toInt();
        if (sudIds.contains(sudId))
            hefegaben.append(modelHefegaben.data(row, ModelHefegaben::ColName).toString());
    }

    if (inGebrauch.size() != rowCount())
        inGebrauch = QVector<bool>(rowCount());
    for (int row = 0; row < rowCount(); row++)
    {
        QString name = data(row,ColName).toString();
        inGebrauch[row] = hefegaben.contains(name);
    }
}

QStringList ModelHefe::inGebrauchListe(const QString &name) const
{
    QStringList list;
    ProxyModel model;
    model.setSourceModel(bh->modelHefegaben());
    for (int r = 0; r < model.rowCount(); ++r)
    {
        if (model.data(r, ModelHefegaben::ColName) == name)
        {
            QVariant sudId = model.data(r, ModelHefegaben::ColSudID);
            Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt());
            if (status == Brauhelfer::SudStatus::Rezept)
            {
                QString sudName = bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::ColSudname).toString();
                if (!list.contains(sudName))
                    list.append(sudName);
            }
        }
    }
    return list;
}

void ModelHefe::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColTypOGUG))
        values.insert(ColTypOGUG, 0);
    if (!values.contains(ColTypTrFl))
        values.insert(ColTypTrFl, 0);
    if (!values.contains(ColWuerzemenge))
        values.insert(ColWuerzemenge, 20);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
