#include "modelmalz.h"
#include "brauhelfer.h"
#include <QDate>
#include "proxymodelsud.h"

ModelMalz::ModelMalz(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("InGebrauch");
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
        ProxyModel model;
        model.setSourceModel(bh->modelMalzschuettung());
        QVariant name = data(idx.row(), ColBeschreibung);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelMalzschuettung::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelMalzschuettung::ColSudID);
                Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->dataSud(sudId, ModelSud::ColStatus).toInt());
                if (status == Brauhelfer::SudStatus::Rezept)
                    return true;
            }
        }
        return false;
    }
    default:
        return QVariant();
    }
}

bool ModelMalz::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColBeschreibung:
    {
        QString name = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, name))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelMalzschuettung();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelMalzschuettung::ColSudID) == sudId && model->data(j, ModelMalzschuettung::ColName) == prevName)
                        model->setData(j, ModelMalzschuettung::ColName, name);
                }
            }
            return true;
        }
        return false;
    }
    case ColFarbe:
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant name = data(idx.row(), ColBeschreibung);
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelMalzschuettung();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelMalzschuettung::ColSudID) == sudId && model->data(j, ModelMalzschuettung::ColName) == name)
                        model->setData(j, ModelMalzschuettung::ColFarbe, value);
                }
            }
            return true;
        }
        return false;
    case ColEingelagert:
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
    case ColMindesthaltbar:
        return QSqlTableModel::setData(idx, value.toDateTime().toString(Qt::ISODate));
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

void ModelMalz::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColBeschreibung] = getUniqueName(index(0, ColBeschreibung), values[ColBeschreibung], true);
    if (!values.contains(ColFarbe))
        values.insert(ColFarbe, 0);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
