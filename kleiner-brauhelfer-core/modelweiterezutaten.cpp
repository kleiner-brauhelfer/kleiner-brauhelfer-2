#include "modelweiterezutaten.h"
#include "brauhelfer.h"
#include <QDate>
#include "proxymodelsud.h"

ModelWeitereZutaten::ModelWeitereZutaten(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("MengeGramm");
    mVirtualField.append("InGebrauch");
}

QVariant ModelWeitereZutaten::dataExt(const QModelIndex &index) const
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
    if (field == "MengeGramm")
    {
        double menge = index.sibling(index.row(), fieldIndex("Menge")).data().toDouble();
        switch (index.sibling(index.row(), fieldIndex("Einheiten")).data().toInt())
        {
        case EWZ_Einheit_Kg:
            return menge * 1000;
        case EWZ_Einheit_g:
            return menge;
        case EWZ_Einheit_mg:
            return menge / 1000;
        case EWZ_Einheit_Stk:
            return menge;
        default:
            return menge;
        }
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
            ProxyModel modelWeitereZutatenGaben;
            modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
            modelWeitereZutatenGaben.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
            modelWeitereZutatenGaben.setFilterRegExp(QString("^%1$").arg(modelSud.data(i, "ID").toInt()));
            for (int j = 0; j < modelWeitereZutatenGaben.rowCount(); ++j)
            {
                if (modelWeitereZutatenGaben.data(j, "Name").toString() == name)
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

bool ModelWeitereZutaten::setDataExt(const QModelIndex &index, const QVariant &value)
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
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == prevValue)
                        model->setData(j, "Name", name);
                }
            }
            return true;
        }
    }
    if (field == "Einheiten")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QString name = data(index.row(), "Beschreibung").toString();
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Einheit", value);
                }
            }
            return true;
        }
    }
    if (field == "Typ")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QString name = data(index.row(), "Beschreibung").toString();
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Typ", value);
                }
            }
            return true;
        }
    }
    if (field == "Ausbeute")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QString name = data(index.row(), "Beschreibung").toString();
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Ausbeute", value);
                }
            }
            return true;
        }
    }
    if (field == "EBC")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QString name = data(index.row(), "Beschreibung").toString();
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
            for (int i = 0; i < modelSud.rowCount(); ++i)
            {
                int id = modelSud.data(i, "ID").toInt();
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == name)
                        model->setData(j, "Farbe", value);
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

void ModelWeitereZutaten::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Menge"))
        values.insert("Menge", 0);
    if (!values.contains("Einheiten"))
     values.insert("Einheiten", 0);
    if (!values.contains("Typ"))
        values.insert("Typ", 0);
    if (!values.contains("Ausbeute"))
        values.insert("Ausbeute", 0);
    if (!values.contains("EBC"))
        values.insert("EBC", 0);
    if (!values.contains("Preis"))
        values.insert("Preis", 0);
    if (!values.contains("Eingelagert"))
        values.insert("Eingelagert", QDate::currentDate());
    if (!values.contains("Mindesthaltbar"))
        values.insert("Mindesthaltbar", QDate::currentDate().addYears(1));
    if (values.contains("Beschreibung"))
        values["Beschreibung"] = getUniqueName(index(0, fieldIndex("Beschreibung")), values["Beschreibung"], true);
}
