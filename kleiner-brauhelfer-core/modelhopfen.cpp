// clazy:excludeall=skipped-base-method
#include "modelhopfen.h"
#include "proxymodel.h"
#include "proxymodelsud.h"
#include "brauhelfer.h"
#include <QDate>

ModelHopfen::ModelHopfen(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    inGebrauch(QVector<bool>())
{
    mVirtualField.append(QStringLiteral("InGebrauch"));

    connect(this, &SqlTableModel::modelReset, this, &ModelHopfen::resetInGebrauch);
    connect(this, &SqlTableModel::rowsInserted, this, &ModelHopfen::resetInGebrauch);
    connect(this, &SqlTableModel::rowsRemoved, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::modelReset, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsInserted, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsRemoved, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelHopfen::onSudDataChanged);
    connect(bh->modelHopfengaben(), &SqlTableModel::modelReset, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelHopfengaben(), &SqlTableModel::rowsInserted, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelHopfengaben(), &SqlTableModel::rowsRemoved, this, &ModelHopfen::resetInGebrauch);
    connect(bh->modelHopfengaben(), &SqlTableModel::dataChanged, this, &ModelHopfen::onHopfengabenDataChanged);
}

QVariant ModelHopfen::dataExt(const QModelIndex &idx) const
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

bool ModelHopfen::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString newName = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, newName))
        {
            bh->modelHopfengaben()->update(prevName, ModelHopfengaben::ColName, newName);
            bh->modelWeitereZutatenGaben()->update(prevName, ModelWeitereZutatenGaben::ColName, newName);
            return true;
        }
        return false;
    }
    case ColAlpha:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelHopfengaben()->update(data(idx.row(), ColName), ModelHopfengaben::ColAlpha, value);
            return true;
        }
        return false;
    }
    case ColPellets:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelHopfengaben()->update(data(idx.row(), ColName), ModelHopfengaben::ColPellets, value);
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

void ModelHopfen::resetInGebrauch()
{
    inGebrauch.clear();
}

void ModelHopfen::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelSud::ColStatus >= topLeft.column() && ModelSud::ColStatus <= bottomRight.column())
        resetInGebrauch();
}

void ModelHopfen::onHopfengabenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelHopfengaben::ColSudID >= topLeft.column() && ModelHopfengaben::ColSudID <= bottomRight.column())
        resetInGebrauch();
    if (ModelHopfengaben::ColName >= topLeft.column() && ModelHopfengaben::ColName <= bottomRight.column())
        resetInGebrauch();
}

void ModelHopfen::updateInGebrauch() const
{
    QList<int> sudIds;
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept);
    for (int row = 0; row < modelSud.rowCount(); row++)
    {
        sudIds.append(modelSud.data(row, ModelSud::ColID).toInt());
    }

    QList<QString> hopfengaben;
    ProxyModel modelHopfengaben;
    modelHopfengaben.setSourceModel(bh->modelHopfengaben());
    for (int row = 0; row < modelHopfengaben.rowCount(); row++)
    {
        int sudId = modelHopfengaben.data(row, ModelHopfengaben::ColSudID).toInt();
        if (sudIds.contains(sudId))
            hopfengaben.append(modelHopfengaben.data(row, ModelHopfengaben::ColName).toString());
    }

    if (inGebrauch.size() != rowCount())
        inGebrauch = QVector<bool>(rowCount());
    for (int row = 0; row < rowCount(); row++)
    {
        QString name = data(row,ColName).toString();
        inGebrauch[row] = hopfengaben.contains(name);
    }
}

QStringList ModelHopfen::inGebrauchListe(const QString &name) const
{
    QStringList list;
    ProxyModel model;
    model.setSourceModel(bh->modelHopfengaben());
    for (int r = 0; r < model.rowCount(); ++r)
    {
        if (model.data(r, ModelHopfengaben::ColName) == name)
        {
            QVariant sudId = model.data(r, ModelHopfengaben::ColSudID);
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

void ModelHopfen::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
    if (!values.contains(ColAlpha))
        values.insert(ColAlpha, 0);
    if (!values.contains(ColPellets))
        values.insert(ColPellets, 1);
    if (!values.contains(ColTyp))
        values.insert(ColTyp, 0);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
