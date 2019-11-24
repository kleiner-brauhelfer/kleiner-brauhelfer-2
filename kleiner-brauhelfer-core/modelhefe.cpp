#include "modelhefe.h"
#include "brauhelfer.h"
#include <QDate>
#include "proxymodelsud.h"

ModelHefe::ModelHefe(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("InGebrauch");
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
        ProxyModel model;
        model.setSourceModel(bh->modelHefegaben());
        QVariant name = data(idx.row(), ColBeschreibung);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelHefegaben::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelHefegaben::ColSudID);
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

bool ModelHefe::setDataExt(const QModelIndex &idx, const QVariant &value)
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
                SqlTableModel* model = bh->modelHefegaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelHefegaben::ColSudID) == sudId && model->data(j, ModelHefegaben::ColName) == prevName)
                        model->setData(j, ModelHefegaben::ColName, name);
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

void ModelHefe::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Menge"))
        values.insert("Menge", 0);
    if (!values.contains("Wuerzemenge"))
        values.insert("Wuerzemenge", 0);
    if (!values.contains("Preis"))
        values.insert("Preis", 0);
    if (!values.contains("Eingelagert"))
        values.insert("Eingelagert", QDate::currentDate());
    if (!values.contains("Mindesthaltbar"))
        values.insert("Mindesthaltbar", QDate::currentDate().addYears(1));
    if (values.contains("Beschreibung"))
        values["Beschreibung"] = getUniqueName(index(0, ColBeschreibung), values["Beschreibung"], true);
}
