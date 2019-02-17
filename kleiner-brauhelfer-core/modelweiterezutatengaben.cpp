#include "modelweiterezutatengaben.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelWeitereZutatenGaben::ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Zeitpunkt_von_ist");
    mVirtualField.append("Zeitpunkt_bis_ist");
    mVirtualField.append("Abfuellbereit");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelWeitereZutatenGaben::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Zeitpunkt_von")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Zeitpunkt_von_ist")
    {
        if (data(index.row(), "Zugabestatus").toInt() == EWZ_Zugabestatus_nichtZugegeben)
            return "";
        return data(index.row(), "Zeitpunkt_von");
    }
    if (field == "Zeitpunkt_bis")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Zeitpunkt_bis_ist")
    {
        if (data(index.row(), "Zugabestatus").toInt() != EWZ_Zugabestatus_Entnommen)
            return "";
        if (data(index.row(), "Entnahmeindex").toInt() == EWZ_Entnahmeindex_KeineEntnahme)
            return "";
        return data(index.row(), "Zeitpunkt_bis");
    }
    if (field == "Typ")
    {
        int typ = QSqlTableModel::data(index).toInt();
        if (typ < 0)
            typ = EWZ_Typ_Hopfen;
        return typ;
    }
    if (field == "Abfuellbereit")
    {
        if (data(index.row(), "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
        {
            int Zugabestatus = data(index.row(), "Zugabestatus").toInt();
            int Entnahmeindex = data(index.row(), "Entnahmeindex").toInt();
            if (Zugabestatus == EWZ_Zugabestatus_nichtZugegeben)
              return false;
            if (Zugabestatus == EWZ_Zugabestatus_Zugegeben && Entnahmeindex == EWZ_Entnahmeindex_MitEntnahme)
                return false;
        }
        return true;
    }
    return QVariant();
}

bool ModelWeitereZutatenGaben::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Name")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int typ = data(index.row(), "Typ").toInt();
            if (typ == EWZ_Typ_Hopfen)
            {
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Einheit")), EWZ_Einheit_g);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Typ")), EWZ_Typ_Hopfen);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Ausbeute")), 0);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Farbe")), 0);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Zeitpunkt")), EWZ_Zeitpunkt_Gaerung);
            }
            else
            {
                int row = bh->modelWeitereZutaten()->getRowWithValue("Beschreibung", value);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Einheit")), bh->modelWeitereZutaten()->data(row, "Einheiten"));
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Typ")), bh->modelWeitereZutaten()->data(row, "Typ"));
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Ausbeute")), bh->modelWeitereZutaten()->data(row, "Ausbeute"));
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Farbe")), bh->modelWeitereZutaten()->data(row, "EBC"));
            }
            return true;
        }
    }
    if (field == "Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int sudId = index.siblingAtColumn(fieldIndex("SudID")).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("erg_Menge")), value.toDouble() * mengeSoll);
            return true;
        }
    }
    if (field == "erg_Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int sudId = index.siblingAtColumn(fieldIndex("SudID")).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Menge")), value.toDouble() / mengeSoll);
            return true;
        }
    }
    if (field == "Typ")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (data(index).toInt() == EWZ_Typ_Hopfen)
            {
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Zeitpunkt")), EWZ_Zeitpunkt_Gaerung);
            }
            return true;
        }
    }
    if (field == "Zeitpunkt")
    {
        int prev = index.data().toInt();
        if (QSqlTableModel::setData(index, value))
        {
            int val = value.toInt();
            if (prev == EWZ_Zeitpunkt_Gaerung && val != EWZ_Zeitpunkt_Gaerung)
            {
                QModelIndex index2 = index.siblingAtColumn(fieldIndex("Zugabedauer"));
                QSqlTableModel::setData(index2, index2.data().toInt() / 1440);
            }
            else if (prev != EWZ_Zeitpunkt_Gaerung && val == EWZ_Zeitpunkt_Gaerung)
            {
                QModelIndex index2 = index.siblingAtColumn(fieldIndex("Zugabedauer"));
                QSqlTableModel::setData(index2, index2.data().toInt() * 1440);
            }
            return true;
        }
    }
    if (field == "Zeitpunkt_von")
    {
        QDateTime dt = value.toDateTime();
        if (QSqlTableModel::setData(index, dt.toString(Qt::ISODate)))
        {
            QModelIndex index2 = index.siblingAtColumn(fieldIndex("Zeitpunkt_bis"));
            dt = dt.addDays(ceil(data(index.row(), "Zugabedauer").toInt() / 1440.0));
            QSqlTableModel::setData(index2, dt.toString(Qt::ISODate));
            return true;
        }
    }
    if (field == "Zeitpunkt_bis")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Zugabestatus")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (value.toInt() == EWZ_Zugabestatus_Entnommen)
            {
                QModelIndex index2 = this->index(index.row(), fieldIndex("Zugabedauer"));
                int day = data(index.row(), "Zeitpunkt_von").toDateTime().daysTo(data(index.row(), "Zeitpunkt_bis").toDateTime());
                QSqlTableModel::setData(index2, day * 1440);
            }
            return true;
        }
    }
    return false;
}

void ModelWeitereZutatenGaben::onSudDataChanged(const QModelIndex &index)
{
    QString field = bh->modelSud()->fieldName(index.column());
    if (field == "Menge")
    {
        int sudId = bh->modelSud()->data(index.row(), "ID").toInt();
        int colSudId = fieldIndex("SudID");
        int colMenge = fieldIndex("Menge");
        for (int i = 0; i < rowCount(); ++i)
        {
            if (this->index(i, colSudId).data().toInt() == sudId)
            {
                QModelIndex index2 = this->index(i, colMenge);
                setData(index2, data(index2));
            }
        }
    }
}

void ModelWeitereZutatenGaben::defaultValues(QVariantMap &values) const
{
    if (values.contains("Typ") && values.value("Typ").toInt() == EWZ_Typ_Hopfen)
    {
        if (!values.contains("Name"))
            values.insert("Name", bh->modelHopfen()->data(0, "Beschreibung"));
    }
    else
    {
        if (!values.contains("Name"))
            values.insert("Name", bh->modelWeitereZutaten()->data(0, "Beschreibung"));
    }
    if (!values.contains("Menge"))
        values.insert("Menge", 0);
    if (!values.contains("Zeitpunkt_von"))
        values.insert("Zeitpunkt_von", QDate::currentDate());
}
