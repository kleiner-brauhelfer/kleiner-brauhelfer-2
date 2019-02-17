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
        modelSud.setFilterStatus(ProxyModelSud::NichtGebraut);
        for (int i = 0; i < modelSud.rowCount(); ++i)
        {
            if (modelSud.data(i, "AuswahlHefe").toString() == name)
            {
                found = true;
                break;
            }
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
            modelSud.setFilterStatus(ProxyModelSud::NichtGebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                if (modelSud.data(i, "AuswahlHefe").toString() == prevValue)
                    modelSud.setData(i, "AuswahlHefe", name);
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

QString ModelHefe::getUniqueName(const QModelIndex &index, const QVariant &value)
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
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
}
