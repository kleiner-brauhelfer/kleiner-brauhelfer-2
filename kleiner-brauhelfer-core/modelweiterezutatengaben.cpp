#include "modelweiterezutatengaben.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include <QDateTime>
#include <cmath>

ModelWeitereZutatenGaben::ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("ZugabeDatum");
    mVirtualField.append("EntnahmeDatum");
    mVirtualField.append("Abfuellbereit");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelWeitereZutatenGaben::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "ZugabeDatum")
    {
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
            return braudatum.addDays(data(index.row(), "ZugabeNach").toInt());
        return QDateTime();
    }
    if (field == "EntnahmeDatum")
    {
        QDateTime zugabedatum = data(index.row(), "ZugabeDatum").toDateTime();
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
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Einheit")), EWZ_Einheit_g);
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Typ")), EWZ_Typ_Hopfen);
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Ausbeute")), 0);
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Farbe")), 0);
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Zeitpunkt")), EWZ_Zeitpunkt_Gaerung);
            }
            else
            {
                int row = bh->modelWeitereZutaten()->getRowWithValue("Beschreibung", value);
                if (row >= 0)
                {
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Einheit")), bh->modelWeitereZutaten()->data(row, "Einheiten"));
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Typ")), bh->modelWeitereZutaten()->data(row, "Typ"));
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Ausbeute")), bh->modelWeitereZutaten()->data(row, "Ausbeute"));
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Farbe")), bh->modelWeitereZutaten()->data(row, "EBC"));
                }
            }
            return true;
        }
    }
    if (field == "Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int sudId = index.sibling(index.row(), fieldIndex("SudID")).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("erg_Menge")), value.toDouble() * mengeSoll);
            return true;
        }
    }
    if (field == "erg_Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int sudId = index.sibling(index.row(), fieldIndex("SudID")).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Menge")), value.toDouble() / mengeSoll);
            return true;
        }
    }
    if (field == "Typ")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (data(index).toInt() == EWZ_Typ_Hopfen)
            {
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Zeitpunkt")), EWZ_Zeitpunkt_Gaerung);
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
                QModelIndex index2 = index.sibling(index.row(), fieldIndex("Zugabedauer"));
                QSqlTableModel::setData(index2, index2.data().toInt() / 1440);
            }
            else if (prev != EWZ_Zeitpunkt_Gaerung && val == EWZ_Zeitpunkt_Gaerung)
            {
                QModelIndex index2 = index.sibling(index.row(), fieldIndex("Zugabedauer"));
                QSqlTableModel::setData(index2, index2.data().toInt() * 1440);
            }
            if (val != EWZ_Zeitpunkt_Gaerung)
            {
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("ZugabeNach")), 0);
            }
            return true;
        }
    }
    if (field == "ZugabeDatum")
    {
        QVariant sudId = data(index.row(), "SudID");
        QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
        if (braudatum.isValid())
            return QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("ZugabeNach")), braudatum.daysTo(value.toDateTime()));
    }
    if (field == "EntnahmeDatum")
    {
        QDateTime zugabedatum = data(index.row(), "ZugabeDatum").toDateTime();
        if (zugabedatum.isValid())
        {
            qint64 tage = zugabedatum.daysTo(value.toDateTime());
            return QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Zugabedauer")), tage*1440);
        }
    }
    return false;
}

void ModelWeitereZutatenGaben::onSudDataChanged(const QModelIndex &idx)
{
    QString field = bh->modelSud()->fieldName(idx.column());
    if (field == "Menge")
    {
        int sudId = bh->modelSud()->data(idx.row(), "ID").toInt();
        int colSudId = fieldIndex("SudID");
        int colMenge = fieldIndex("Menge");
        mSignalModifiedBlocked = true;
        for (int i = 0; i < rowCount(); ++i)
        {
            if (index(i, colSudId).data().toInt() == sudId)
            {
                QModelIndex index2 = index(i, colMenge);
                setData(index2, data(index2));
            }
        }
        mSignalModifiedBlocked = false;
    }
    else if (field == "Status")
    {
        int status = idx.data().toInt();
        int sudId = bh->modelSud()->data(idx.row(), "ID").toInt();
        int colSudId = fieldIndex("SudID");
        int colStatus = fieldIndex("Zugabestatus");
        int colZugabeNach = fieldIndex("ZugabeNach");
        mSignalModifiedBlocked = true;
        if (status == Sud_Status_Rezept)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(index(row, colSudId)).toInt() == sudId)
                    setData(index(row, colStatus), EWZ_Zugabestatus_nichtZugegeben);
            }
        }
        else if (status == Sud_Status_Gebraut)
        {
            for (int row = 0; row < rowCount(); ++row)
            {
                if (data(index(row, colSudId)).toInt() == sudId &&
                    data(index(row, colZugabeNach)).toInt() == 0)
                    setData(index(row, colStatus), EWZ_Zugabestatus_Zugegeben);
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelWeitereZutatenGaben::defaultValues(QVariantMap &values) const
{
    if (values.contains("SudID"))
    {
        QVariant sudId = values.value("SudID");
        if (!values.contains("ZugabeNach"))
        {
            if (bh->modelSud()->getValueFromSameRow("ID", sudId, "Status").toInt() != Sud_Status_Rezept)
            {
                QDateTime braudatum = bh->modelSud()->getValueFromSameRow("ID", sudId, "Braudatum").toDateTime();
                if (braudatum.isValid())
                    values.insert("ZugabeNach", braudatum.daysTo(QDateTime::currentDateTime()));
            }
        }
    }
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
    if (!values.contains("ZugabeNach"))
        values.insert("ZugabeNach", 0);
}
