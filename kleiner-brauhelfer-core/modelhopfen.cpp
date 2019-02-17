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

QVariant ModelHopfen::dataExt(const QModelIndex &index) const
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
            int id = modelSud.data(i, "ID").toInt();
            SqlTableModel* model = bh->modelHopfengaben();
            for (int j = 0; j < model->rowCount(); ++j)
            {
                if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
            SqlTableModel* model2 = bh->modelWeitereZutatenGaben();
            for (int j = 0; j < model2->rowCount(); ++j)
            {
                if (model2->data(j, "SudID").toInt() == id && model2->data(j, "Name").toString() == name)
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

bool ModelHopfen::setDataExt(const QModelIndex &index, const QVariant &value)
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
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == prevValue)
                        model->setData(j, "Name", name);
                }
                SqlTableModel* model2 = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model2->rowCount(); ++j)
                {
                    if (model2->data(j, "SudID").toInt() == id && model2->data(j, "Name").toString() == prevValue)
                        model->setData(j, "Name", name);
                }
            }
            return true;
        }
    }
    if (field == "Alpha")
    {
        QString name = data(index.row(), "Beschreibung").toString();
        if (QSqlTableModel::setData(index, value))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::NichtGebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Alpha", value);
                }
            }
            return true;
        }
    }
    if (field == "Pellets")
    {
        QString name = data(index.row(), "Beschreibung").toString();
        if (QSqlTableModel::setData(index, value))
        {
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::NichtGebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelHopfengaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Pellets", value);
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

QString ModelHopfen::getUniqueName(const QModelIndex &index, const QVariant &value)
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
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
}
