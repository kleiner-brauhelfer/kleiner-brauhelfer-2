#include "modelhopfen.h"
#include "brauhelfer.h"
#include <QDate>
#include "proxymodelsud.h"

ModelHopfen::ModelHopfen(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("InGebrauch");
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
        QVariant name = data(idx.row(), ColBeschreibung);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelHopfengaben::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelHopfengaben::ColSudID);
                if (bh->modelSud()->dataSud(sudId, ModelSud::ColStatus) == Sud_Status_Rezept)
                    return true;
            }
        }
        return false;
    }
    default:
        return QVariant();
    }
}

bool ModelHopfen::setDataExt(const QModelIndex &idx, const QVariant &value)
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
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelHopfengaben::ColSudID) == sudId && model->data(j, ModelHopfengaben::ColName) == prevName)
                        model->setData(j, ModelHopfengaben::ColName, name);
                }
                model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelWeitereZutatenGaben::ColSudID) == sudId && model->data(j, ModelWeitereZutatenGaben::ColName) == prevName)
                        model->setData(j, ModelWeitereZutatenGaben::ColName, name);
                }
            }
            return true;
        }
        return false;
    }
    case ColAlpha:
    {
        QVariant name = data(idx.row(), ColBeschreibung);
        if (QSqlTableModel::setData(idx, value))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelHopfengaben::ColSudID) == sudId && model->data(j, ModelHopfengaben::ColName) == name)
                        model->setData(j, ModelHopfengaben::ColAlpha, value);
                }
            }
            return true;
        }
        return false;
    }
    case ColPellets:
    {
        QVariant name = data(idx.row(), ColBeschreibung);
        if (QSqlTableModel::setData(idx, value))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelHopfengaben::ColSudID) == sudId && model->data(j, ModelHopfengaben::ColName) == name)
                        model->setData(j, ModelHopfengaben::ColPellets, value);
                }
            }
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

void ModelHopfen::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Alpha"))
        values.insert("Alpha", 0);
    if (!values.contains("Pellets"))
        values.insert("Pellets", 1);
    if (!values.contains("Typ"))
        values.insert("Typ", 0);
    if (!values.contains("Menge"))
        values.insert("Menge", 0);
    if (!values.contains("Preis"))
        values.insert("Preis", 0);
    if (!values.contains("Eingelagert"))
        values.insert("Eingelagert", QDate::currentDate());
    if (!values.contains("Mindesthaltbar"))
        values.insert("Mindesthaltbar", QDate::currentDate().addYears(1));
    if (values.contains("Beschreibung"))
        values["Beschreibung"] = getUniqueName(index(0, ColBeschreibung), values["Beschreibung"], true);
}
