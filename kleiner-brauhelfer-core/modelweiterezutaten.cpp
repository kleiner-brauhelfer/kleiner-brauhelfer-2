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
    case ColMengeGramm:
    {
        double menge = data(idx.row(), ColMenge).toDouble();
        switch (data(idx.row(), ColEinheiten).toInt())
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
    case ColInGebrauch:
    {
        ProxyModel model;
        model.setSourceModel(bh->modelWeitereZutatenGaben());
        QVariant name = data(idx.row(), ColBeschreibung);
        for (int r = 0; r < model.rowCount(); ++r)
        {
            if (model.data(r, ModelWeitereZutatenGaben::ColName) == name)
            {
                QVariant sudId = model.data(r, ModelWeitereZutatenGaben::ColSudID);
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

bool ModelWeitereZutaten::setDataExt(const QModelIndex &idx, const QVariant &value)
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
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
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
    case ColEinheiten:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant name = data(idx.row(), ColBeschreibung);
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelWeitereZutatenGaben::ColSudID) == sudId && model->data(j, ModelWeitereZutatenGaben::ColName) == name)
                        model->setData(j, ModelWeitereZutatenGaben::ColEinheit, value);
                }
            }
            return true;
        }
        return false;
    }
    case ColTyp:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant name = data(idx.row(), ColBeschreibung);
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelWeitereZutatenGaben::ColSudID) == sudId && model->data(j, ModelWeitereZutatenGaben::ColName) == name)
                        model->setData(j, ModelWeitereZutatenGaben::ColTyp, value);
                }
            }
            return true;
        }
        return false;
    }
    case ColAusbeute:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant name = data(idx.row(), ColBeschreibung);
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelWeitereZutatenGaben::ColSudID) == sudId && model->data(j, ModelWeitereZutatenGaben::ColName) == name)
                        model->setData(j, ModelWeitereZutatenGaben::ColAusbeute, value);
                }
            }
            return true;
        }
        return false;
    }
    case ColEBC:
    {
        if (QSqlTableModel::setData(idx, value))
        {
            QVariant name = data(idx.row(), ColBeschreibung);
            ProxyModelSud modelSud;
            modelSud.setSourceModel(bh->modelSud());
            modelSud.setFilterStatus(ProxyModelSud::Rezept);
            for (int r = 0; r < modelSud.rowCount(); ++r)
            {
                QVariant sudId = modelSud.data(r, ModelSud::ColID);
                SqlTableModel* model = bh->modelWeitereZutatenGaben();
                for (int j = 0; j < model->rowCount(); ++j)
                {
                    if (model->data(j, ModelWeitereZutatenGaben::ColSudID) == sudId && model->data(j, ModelWeitereZutatenGaben::ColName) == name)
                        model->setData(j, ModelWeitereZutatenGaben::ColFarbe, value);
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

void ModelWeitereZutaten::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColBeschreibung] = getUniqueName(index(0, ColBeschreibung), values[ColBeschreibung], true);
    if (!values.contains(ColMenge))
        values.insert(ColMenge, 0);
    if (!values.contains(ColEinheiten))
     values.insert(ColEinheiten, 0);
    if (!values.contains(ColTyp))
        values.insert(ColTyp, 0);
    if (!values.contains(ColAusbeute))
        values.insert(ColAusbeute, 0);
    if (!values.contains(ColEBC))
        values.insert(ColEBC, 0);
    if (!values.contains(ColPreis))
        values.insert(ColPreis, 0);
    if (!values.contains(ColEingelagert))
        values.insert(ColEingelagert, QDate::currentDate());
    if (!values.contains(ColMindesthaltbar))
        values.insert(ColMindesthaltbar, QDate::currentDate().addYears(1));
}
