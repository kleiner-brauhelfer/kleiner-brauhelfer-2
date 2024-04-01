// clazy:excludeall=skipped-base-method
#include "modelweiterezutaten.h"
#include "proxymodel.h"
#include "proxymodelsud.h"
#include "brauhelfer.h"
#include <QDate>

ModelWeitereZutaten::ModelWeitereZutaten(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh),
    inGebrauch(QVector<bool>())
{
    mVirtualField.append(QStringLiteral("MengeNormiert"));
    mVirtualField.append(QStringLiteral("InGebrauch"));

    connect(this, &SqlTableModel::modelReset, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(this, &SqlTableModel::rowsInserted, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(this, &SqlTableModel::rowsRemoved, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::modelReset, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsInserted, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::rowsRemoved, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelSud(), &SqlTableModel::dataChanged, this, &ModelWeitereZutaten::onSudDataChanged);
    connect(bh->modelWeitereZutatenGaben(), &SqlTableModel::modelReset, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelWeitereZutatenGaben(), &SqlTableModel::rowsInserted, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelWeitereZutatenGaben(), &SqlTableModel::rowsRemoved, this, &ModelWeitereZutaten::resetInGebrauch);
    connect(bh->modelWeitereZutatenGaben(), &SqlTableModel::dataChanged, this, &ModelWeitereZutaten::onWeitereZutatenGabenDataChanged);
}

QVariant ModelWeitereZutaten::dataExt(const QModelIndex &idx) const
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
    case ColMengeNormiert:
    {
        double menge = data(idx.row(), ColMenge).toDouble();
        Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(data(idx.row(), ColEinheit).toInt());
        switch (einheit)
        {
        case Brauhelfer::Einheit::Kg:
            return menge * 1000;
        case Brauhelfer::Einheit::g:
            return menge;
        case Brauhelfer::Einheit::mg:
            return menge / 1000;
        case Brauhelfer::Einheit::Stk:
            return menge;
        case Brauhelfer::Einheit::l:
            return menge * 1000;
        case Brauhelfer::Einheit::ml:
            return menge;
        }
        return 0;
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

bool ModelWeitereZutaten::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString newName = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, newName))
        {
            bh->modelWeitereZutatenGaben()->update(prevName, ModelWeitereZutatenGaben::ColName, newName);
            return true;
        }
        return false;
    }
    case ColEinheit:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelWeitereZutatenGaben()->update(data(idx.row(), ColName), ModelWeitereZutatenGaben::ColEinheit, value);
            return true;
        }
        return false;
    }
    case ColTyp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelWeitereZutatenGaben()->update(data(idx.row(), ColName), ModelWeitereZutatenGaben::ColTyp, value);
            return true;
        }
        return false;
    }
    case ColAusbeute:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelWeitereZutatenGaben()->update(data(idx.row(), ColName), ModelWeitereZutatenGaben::ColAusbeute, value);
            return true;
        }
        return false;
    }
    case ColFarbe:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelWeitereZutatenGaben()->update(data(idx.row(), ColName), ModelWeitereZutatenGaben::ColFarbe, value);
            return true;
        }
        return false;
    }
    case ColUnvergaerbar:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            bh->modelWeitereZutatenGaben()->update(data(idx.row(), ColName), ModelWeitereZutatenGaben::ColUnvergaerbar, value);
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

void ModelWeitereZutaten::resetInGebrauch()
{
    inGebrauch.clear();
}

void ModelWeitereZutaten::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelSud::ColStatus >= topLeft.column() && ModelSud::ColStatus <= bottomRight.column())
        resetInGebrauch();
}

void ModelWeitereZutaten::onWeitereZutatenGabenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    Q_UNUSED(roles)
    if (ModelWeitereZutatenGaben::ColSudID >= topLeft.column() && ModelWeitereZutatenGaben::ColSudID <= bottomRight.column())
        resetInGebrauch();
    if (ModelWeitereZutatenGaben::ColName >= topLeft.column() && ModelWeitereZutatenGaben::ColName <= bottomRight.column())
        resetInGebrauch();
}

void ModelWeitereZutaten::updateInGebrauch() const
{
    QList<int> sudIds;
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept);
    for (int row = 0; row < modelSud.rowCount(); row++)
    {
        sudIds.append(modelSud.data(row, ModelSud::ColID).toInt());
    }

    QList<QString> weitereZutatenGaben;
    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    for (int row = 0; row < modelWeitereZutatenGaben.rowCount(); row++)
    {
        int sudId = modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColSudID).toInt();
        if (sudIds.contains(sudId))
            weitereZutatenGaben.append(modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColName).toString());
    }

    if (inGebrauch.size() != rowCount())
        inGebrauch = QVector<bool>(rowCount());
    for (int row = 0; row < rowCount(); row++)
    {
        QString name = data(row,ColName).toString();
        inGebrauch[row] = weitereZutatenGaben.contains(name);
    }
}

QStringList ModelWeitereZutaten::inGebrauchListe(const QString &name) const
{
    QStringList list;
    ProxyModel model;
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    for (int r = 0; r < model.rowCount(); ++r)
    {
        if (model.data(r, ModelWeitereZutatenGaben::ColName) == name)
        {
            QVariant sudId = model.data(r, ModelWeitereZutatenGaben::ColSudID);
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

void ModelWeitereZutaten::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColEinheit))
     values.insert(ColEinheit, static_cast<int>(Brauhelfer::Einheit::Kg));
    if (!values.contains(ColTyp))
        values.insert(ColTyp, 0);
    if (!values.contains(ColAusbeute))
        values.insert(ColAusbeute, 0);
    if (!values.contains(ColFarbe))
        values.insert(ColFarbe, 0);
    if (!values.contains(ColUnvergaerbar))
        values.insert(ColUnvergaerbar, 0);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
