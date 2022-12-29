// clazy:excludeall=skipped-base-method
#include "modelhopfen.h"
#include "brauhelfer.h"
#include <QDate>

ModelHopfen::ModelHopfen(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("InGebrauch");
    mVirtualField.append("InGebrauchListe");
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
        ProxyModel model;
        model.setSourceModel(bh->modelHopfengaben());
        QVariant name = data(idx.row(), ColName);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelHopfengaben::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelHopfengaben::ColSudID);
                Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt());
                if (status == Brauhelfer::SudStatus::Rezept)
                    return true;
            }
        }
        return false;
    }
    case ColInGebrauchListe:
    {
        QStringList list;
        ProxyModel model;
        model.setSourceModel(bh->modelHopfengaben());
        QVariant name = data(idx.row(), ColName);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelHopfengaben::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelHopfengaben::ColSudID);
                Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt());
                if (status == Brauhelfer::SudStatus::Rezept)
                    list.append(bh->modelSud()->getValueFromSameRow(ModelSud::ColID, sudId, ModelSud::ColSudname).toString());
            }
        }
        return list;
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
