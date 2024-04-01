// clazy:excludeall=skipped-base-method
#include "modelmalz.h"
#include "proxymodel.h"
#include "proxymodelsud.h"
#include "brauhelfer.h"
#include <QDate>

ModelMalz::ModelMalz(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    inGebrauch(QVector<bool>())
{
    mVirtualField.append(QStringLiteral("InGebrauch"));

    connect(this, &SqlTableModel::modelReset, this, &ModelMalz::resetInGebrauch);
    connect(this, &SqlTableModel::rowsInserted, this, &ModelMalz::resetInGebrauch);
    connect(this, &SqlTableModel::rowsRemoved, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::modelReset, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsInserted, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsRemoved, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelMalz::onSudDataChanged);
    connect(bh->modelMalzschuettung(), &SqlTableModel::modelReset, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelMalzschuettung(), &SqlTableModel::rowsInserted, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelMalzschuettung(), &SqlTableModel::rowsRemoved, this, &ModelMalz::resetInGebrauch);
    connect(bh->modelMalzschuettung(), &SqlTableModel::dataChanged, this, &ModelMalz::onMalzschuettungDataChanged);
}

QVariant ModelMalz::dataExt(const QModelIndex &idx) const
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

bool ModelMalz::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString newName = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, newName))
        {
            bh->modelMalzschuettung()->update(prevName, ModelMalzschuettung::ColName, newName);
            return true;
        }
        return false;
    }
    case ColPotential:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelMalzschuettung()->update(data(idx.row(), ColName), ModelMalzschuettung::ColPotential, value);
            return true;
        }
        return false;
    }
    case ColFarbe:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelMalzschuettung()->update(data(idx.row(), ColName), ModelMalzschuettung::ColFarbe, value);
            return true;
        }
        return false;
    }
    case ColpH:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelMalzschuettung()->update(data(idx.row(), ColName), ModelMalzschuettung::ColpH, value);
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

void ModelMalz::resetInGebrauch()
{
    inGebrauch.clear();
}

void ModelMalz::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelSud::ColStatus >= topLeft.column() && ModelSud::ColStatus <= bottomRight.column())
        resetInGebrauch();
}

void ModelMalz::onMalzschuettungDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelMalzschuettung::ColSudID >= topLeft.column() && ModelMalzschuettung::ColSudID <= bottomRight.column())
        resetInGebrauch();
    if (ModelMalzschuettung::ColName >= topLeft.column() && ModelMalzschuettung::ColName <= bottomRight.column())
        resetInGebrauch();
}

void ModelMalz::updateInGebrauch() const
{
    QList<int> sudIds;
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept);
    for (int row = 0; row < modelSud.rowCount(); row++)
    {
        sudIds.append(modelSud.data(row, ModelSud::ColID).toInt());
    }

    QList<QString> malzschuettungen;
    ProxyModel modelMalzschuettung;
    modelMalzschuettung.setSourceModel(bh->modelMalzschuettung());
    for (int row = 0; row < modelMalzschuettung.rowCount(); row++)
    {
        int sudId = modelMalzschuettung.data(row, ModelMalzschuettung::ColSudID).toInt();
        if (sudIds.contains(sudId))
            malzschuettungen.append(modelMalzschuettung.data(row, ModelMalzschuettung::ColName).toString());
    }

    if (inGebrauch.size() != rowCount())
        inGebrauch = QVector<bool>(rowCount());
    for (int row = 0; row < rowCount(); row++)
    {
        QString name = data(row,ColName).toString();
        inGebrauch[row] = malzschuettungen.contains(name);
    }
}

QStringList ModelMalz::inGebrauchListe(const QString &name) const
{
    QStringList list;
    ProxyModel model;
    model.setSourceModel(bh->modelMalzschuettung());
    for (int r = 0; r < model.rowCount(); ++r)
    {
        if (model.data(r, ModelMalzschuettung::ColName) == name)
        {
            QVariant sudId = model.data(r, ModelMalzschuettung::ColSudID);
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

void ModelMalz::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
    if (!values.contains(ColPotential))
        values.insert(ColPotential, 0);
    if (!values.contains(ColFarbe))
        values.insert(ColFarbe, 0);
    if (!values.contains(ColpH))
        values.insert(ColpH, 0);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
