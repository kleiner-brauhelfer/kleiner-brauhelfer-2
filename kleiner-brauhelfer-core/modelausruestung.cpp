#include "modelausruestung.h"
#include "brauhelfer.h"
#include <qmath.h>
#include "proxymodelsud.h"

ModelAusruestung::ModelAusruestung(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("InGebrauch");
    mVirtualField.append("Maischebottich_Volumen");
    mVirtualField.append("Maischebottich_MaxFuellvolumen");
    mVirtualField.append("Sudpfanne_Volumen");
    mVirtualField.append("Sudpfanne_MaxFuellvolumen");
    mVirtualField.append("Vermoegen");
    mVirtualField.append("AnzahlSude");
}

QVariant ModelAusruestung::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "InGebrauch")
    {
        bool found = false;
        QString name = data(index.row(), "Name").toString();
        ProxyModelSud modelSud;
        modelSud.setSourceModel(bh->modelSud());
        modelSud.setFilterStatus(ProxyModelSud::NichtGebraut);
        for (int i = 0; i < modelSud.rowCount(); ++i)
        {
            if (modelSud.data(i, "AuswahlBrauanlageName").toString() == name)
            {
                found = true;
                break;
            }
        }
        return found;
    }
    if (field == "Maischebottich_Volumen")
    {
        double r = data(index.row(), "Maischebottich_Durchmesser").toDouble() / 2;
        double h = data(index.row(), "Maischebottich_Hoehe").toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    if (field == "Maischebottich_MaxFuellvolumen")
    {
        double r = data(index.row(), "Maischebottich_Durchmesser").toDouble() / 2;
        double h = data(index.row(), "Maischebottich_MaxFuellhoehe").toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    if (field == "Sudpfanne_Volumen")
    {
        double r = data(index.row(), "Sudpfanne_Durchmesser").toDouble() / 2;
        double h = data(index.row(), "Sudpfanne_Hoehe").toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    if (field == "Sudpfanne_MaxFuellvolumen")
    {
        double r = data(index.row(), "Sudpfanne_Durchmesser").toDouble() / 2;
        double h = data(index.row(), "Sudpfanne_MaxFuellhoehe").toDouble();
        return pow(r, 2) * M_PI * h / 1000;
    }
    if (field == "Vermoegen")
    {
        double V1 = data(index.row(), "Maischebottich_MaxFuellvolumen").toDouble();
        double V2 = data(index.row(), "Sudpfanne_MaxFuellvolumen").toDouble();
        return (V1 > V2) ? V2 : V1;
    }
    if (field == "AnzahlSude")
    {
        int n = 0;
        QString name = data(index.row(), "Name").toString();
        int colName = bh->modelSud()->fieldIndex("AuswahlBrauanlageName");
        for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
        {
            if (bh->modelSud()->index(row, colName).data().toString() == name)
                ++n;
        }
        return n;
    }
    return QVariant();
}

bool ModelAusruestung::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Name")
    {
        QString name = getUniqueName(index, value);
        QString prevValue = data(index).toString();
        if (QSqlTableModel::setData(index, name))
        {
            int colName = bh->modelSud()->fieldIndex("AuswahlBrauanlageName");
            for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
            {
                QModelIndex index = bh->modelSud()->index(row, colName);
                if (bh->modelSud()->data(index).toString() == prevValue)
                    bh->modelSud()->setData(index, name);
            }
            return true;
        }
    }
    return false;
}

QString ModelAusruestung::getUniqueName(const QModelIndex &index, const QVariant &value)
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
}

bool ModelAusruestung::removeRows(int row, int count, const QModelIndex &parent)
{
    if (SqlTableModel::removeRows(row, count, parent))
    {
        for (int i = 0; i < count; ++i)
        {
            int id = data(row + i, "AnlagenID").toInt();
            int colId = bh->modelGeraete()->fieldIndex("AusruestungAnlagenID");
            for (int i = 0; i < bh->modelGeraete()->rowCount(); ++i)
            {
                if (bh->modelGeraete()->index(i, colId).data().toInt() == id)
                    bh->modelGeraete()->removeRows(i);
            }
        }
        return true;
    }
    return false;
}

void ModelAusruestung::defaultValues(QVariantMap &values) const
{
    if (!values.contains("AnlagenID"))
        values.insert("AnlagenID", (int)time(nullptr) + rand());
    if (!values.contains("Sudhausausbeute"))
        values.insert("Sudhausausbeute", 60.0);
}

QString ModelAusruestung::name(int id) const
{
    for (int i = 0; i < rowCount(); ++i)
        if (data(i, "AnlagenID").toInt() == id)
            return data(i, "Name").toString();
    return QString();
}

int ModelAusruestung::id(const QString& name) const
{
    for (int i = 0; i < rowCount(); ++i)
        if (data(i, "Name").toString() == name)
            return data(i, "AnlagenID").toInt();
    return 0;
}
