#include "modelweiterezutatengaben.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelWeitereZutatenGaben::ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("Zeitpunkt_von");
    mVirtualField.append("Zeitpunkt_bis");
    mVirtualField.append("Abfuellbereit");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelWeitereZutatenGaben::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Zeitpunkt_von")
    {
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
        {
            int tage = data(index.row(), "ZugegebenNach").toInt();
            return braudatum.addDays(tage);
        }
        return QDateTime();
    }
    if (field == "Zeitpunkt_bis")
    {
        QDateTime zugabedatum = data(index.row(), "Zeitpunkt_von").toDateTime();
        if (zugabedatum.isValid())
        {
            int tage = data(index.row(), "Zugabedauer").toInt() / 1440;
            return zugabedatum.addDays(tage);
        }
        return QDateTime();
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
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
        {
            qint64 tage = braudatum.daysTo(value.toDateTime());
            return QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("ZugegebenNach")), tage);
        }
    }
    if (field == "Zeitpunkt_bis")
    {
        QDateTime zugabedatum = data(index.row(), "Zeitpunkt_von").toDateTime();
        if (zugabedatum.isValid())
        {
            qint64 tage = zugabedatum.daysTo(value.toDateTime());
            return QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("Zugabedauer")), tage*1440);
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
    else if (field == "BierWurdeGebraut")
    {
        if (!index.data().toBool())
        {
            int sudId = bh->modelSud()->data(index.row(), "ID").toInt();

            int colSudId = fieldIndex("SudID");
            int colStatus = fieldIndex("Zugabestatus");

            for (int i = 0; i < rowCount(); ++i)
            {
                if (this->index(i, colSudId).data().toInt() == sudId)
                {
                    QModelIndex index2 = this->index(i, colStatus);
                    setData(index2, EWZ_Zugabestatus_nichtZugegeben);
                }
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
