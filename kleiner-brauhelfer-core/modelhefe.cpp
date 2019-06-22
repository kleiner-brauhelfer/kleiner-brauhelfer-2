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

QVariant ModelHefe::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Eingelagert")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Mindesthaltbar")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "InGebrauch")
    {
        bool found = false;
        QString name = data(index.row(), "Beschreibung").toString();
        ProxyModelSud modelSud;
        modelSud.setSourceModel(bh->modelSud());
        modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
        for (int i = 0; i < modelSud.rowCount(); ++i)
        {
            ProxyModel modelHefegaben;
            modelHefegaben.setSourceModel(bh->modelHefegaben());
            modelHefegaben.setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
            modelHefegaben.setFilterRegExp(QString("^%1$").arg(modelSud.data(i, "ID").toInt()));
            for (int j = 0; j < modelHefegaben.rowCount(); ++j)
            {
                if (modelHefegaben.data(j, "Name").toString() == name)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        return found;
    }
    return QVariant();
}

bool ModelHefe::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Beschreibung")
    {
        QString name = getUniqueName(index, value);
        QString prevValue = data(index).toString();
        if (QSqlTableModel::setData(index, name))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelHefegaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == prevValue)
                        model->setData(j, "Name", name);
                }
            }
            return true;
        }
    }
    if (field == "Eingelagert")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Mindesthaltbar")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Menge")
    {
        double prevValue = data(index.row(), "Menge").toDouble();
        if (QSqlTableModel::setData(index, value))
        {
            if (value.toDouble() > 0.0)
            {
                if (prevValue == 0.0)
                {
                    setData(this->index(index.row(), fieldIndex("Eingelagert")), QDate::currentDate());
                    setData(this->index(index.row(), fieldIndex("Mindesthaltbar")), QDate::currentDate().addYears(1));
                }
            }
            return true;
        }
    }
    return false;
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
        values["Beschreibung"] = getUniqueName(index(0, fieldIndex("Beschreibung")), values["Beschreibung"], true);
}
