#include "modelhopfengaben.h"
#include "brauhelfer.h"

ModelHopfengaben::ModelHopfengaben(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
    mVirtualField.append("IBUAnteil");
    mVirtualField.append("Ausbeute");
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&)));
}

QVariant ModelHopfengaben::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "IBUAnteil")
    {
        int sudId = data(index.row(), "SudID").toInt();
        double menge = data(index.row(), "erg_Menge").toDouble();
        double alpha = data(index.row(), "Alpha").toDouble();
        double ausbeute = data(index.row(), "Ausbeute").toDouble();
        double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
        return menge * alpha * ausbeute / (10 * mengeSoll);
    }
    else if (field == "Ausbeute")
    {
        int sudId = data(index.row(), "SudID").toInt();
        double sw = bh->modelSud()->getValueFromSameRow("ID", sudId, "SWSollKochende").toDouble();
        double isozeit = bh->modelSud()->getValueFromSameRow("ID", sudId, "Nachisomerisierungszeit").toDouble();
        double zeit = data(index.row(), "Zeit").toDouble();
        double ausbeute = BierCalc::hopfenAusbeute(zeit + isozeit, sw);
        if (data(index.row(), "Pellets").toBool())
            ausbeute *= 1.1;
        if (data(index.row(), "Vorderwuerze").toBool())
            ausbeute *= 0.9;
        if (ausbeute < 0.1)
            ausbeute = 0.1;
        return ausbeute;
    }
    return QVariant();
}

bool ModelHopfengaben::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Name")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int row = bh->modelHopfen()->getRowWithValue("Beschreibung", value);
            if (row >= 0)
            {
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Alpha")), bh->modelHopfen()->data(row, "Alpha"));
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Pellets")), bh->modelHopfen()->data(row, "Pellets"));
                QModelIndex index2 = index.sibling(index.row(), fieldIndex("Prozent"));
                setData(index2, data(index2));
            }
            return true;
        }
    }
    else if (field == "Prozent")
    {
        double fVal = value.toDouble();
        if (fVal < 0.0)
            fVal = 0.0;
        if (fVal > 100.0)
            fVal = 100.0;
        if (QSqlTableModel::setData(index, fVal))
        {
            int colSudId = fieldIndex("SudID");
            int sudId = index.sibling(index.row(), colSudId).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            double ibuSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "IBU").toDouble();
            switch (bh->modelSud()->getValueFromSameRow("ID", sudId, "berechnungsArtHopfen").toInt())
            {
            case Hopfen_Berechnung_Gewicht:
                {
                    double summe = 0.0;
                    int colAlpha = fieldIndex("Alpha");
                    int colProzent = fieldIndex("Prozent");
                    int colAusbeute = fieldIndex("Ausbeute");
                    int colMenge = fieldIndex("erg_Menge");
                    for (int i = 0; i < rowCount(); ++i)
                    {
                        if (this->index(i, colSudId).data().toInt() == sudId)
                        {
                            double alpha = this->index(i, colAlpha).data().toDouble();
                            double ausbeute = this->index(i, colAusbeute).data().toDouble();
                            double prozent = this->index(i, colProzent).data().toDouble();
                            summe += prozent * alpha * ausbeute;
                        }
                    }
                    double factor = ibuSoll * 10 * mengeSoll / summe;
                    for (int i = 0; i < rowCount(); ++i)
                    {
                        if (this->index(i, colSudId).data().toInt() == sudId)
                        {
                            double menge = factor * this->index(i, colProzent).data().toDouble();
                            QSqlTableModel::setData(this->index(i, colMenge), menge);
                        }
                    }
                }
                break;
            case Hopfen_Berechnung_IBU:
                {
                    double anteil = ibuSoll * fVal / 100 ;
                    double alpha = data(index.row(), "Alpha").toDouble();
                    double ausbeute = data(index.row(), "Ausbeute").toDouble();
                    double menge;
                    if (alpha == 0.0 || ausbeute == 0.0)
                        menge = 0.0;
                    else
                        menge = (anteil * mengeSoll * 10) / (alpha * ausbeute);
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("erg_Menge")), menge);
                }
                break;
            }
            return true;
        }
    }
    else if (field == "erg_Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int colSudId = fieldIndex("SudID");
            int sudId = index.sibling(index.row(), colSudId).data().toInt();
            double mengeSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "Menge").toDouble();
            double ibuSoll = bh->modelSud()->getValueFromSameRow("ID", sudId, "IBU").toDouble();
            switch (bh->modelSud()->getValueFromSameRow("ID", sudId, "berechnungsArtHopfen").toInt())
            {
            case Hopfen_Berechnung_Gewicht:
                {
                    double summe = 0.0;
                    int colMenge = fieldIndex("erg_Menge");
                    for (int i = 0; i < rowCount(); ++i)
                    {
                        if (this->index(i, colSudId).data().toInt() == sudId)
                        {
                            summe += this->index(i, colMenge).data().toDouble();
                        }
                    }
                    double p = value.toDouble() / summe * 100;
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Prozent")), p);
                 }
                break;
            case Hopfen_Berechnung_IBU:
                {
                    double alpha = data(index.row(), "Alpha").toDouble();
                    double ausbeute = data(index.row(), "Ausbeute").toDouble();
                    double p = (10 * alpha * ausbeute * value.toDouble()) / (ibuSoll * mengeSoll);
                    QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Prozent")), p);
                }
                break;
            }
            return true;
        }
    }
    else if (field == "Zeit")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QModelIndex index2 = index.sibling(index.row(), fieldIndex("Prozent"));
            setData(index2, data(index2));
            return true;
        }
    }
    else if (field == "Alpha")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QModelIndex index2 = index.sibling(index.row(), fieldIndex("Prozent"));
            setData(index2, data(index2));
            return true;
        }
    }
    else if (field == "Pellets")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QModelIndex index2 = index.sibling(index.row(), fieldIndex("Prozent"));
            setData(index2, data(index2));
            return true;
        }
    }
    else if (field == "Vorderwuerze")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (value.toBool())
            {
                int sudId = data(index.row(), "SudID").toInt();
                double dauer = bh->modelSud()->getValueFromSameRow("ID", sudId, "KochdauerNachBitterhopfung").toDouble();
                QSqlTableModel::setData(index.sibling(index.row(), fieldIndex("Zeit")), dauer);
            }
            QModelIndex index2 = index.sibling(index.row(), fieldIndex("Prozent"));
            setData(index2, data(index2));
            return true;
        }
    }
    return false;
}

void ModelHopfengaben::onSudDataChanged(const QModelIndex &index)
{
    QString field = bh->modelSud()->fieldName(index.column());
    if (field == "Menge" || field == "SW" || field == "IBU" || field == "berechnungsArtHopfen" ||
        field == "highGravityFaktor" || field == "KochdauerNachBitterhopfung" || field == "Nachisomerisierungszeit")
    {
        int sudId = bh->modelSud()->data(index.row(), "ID").toInt();
        int colSudId = fieldIndex("SudID");
        int colUpdate = fieldIndex(field == "berechnungsArtHopfen" ? "erg_Menge" : "Prozent");
        mSignalModifiedBlocked = true;
        if (field == "KochdauerNachBitterhopfung")
        {
            int max = index.data().toInt();
            int colZeit = fieldIndex("Zeit");
            int colVWH = fieldIndex("Vorderwuerze");
            for (int i = 0; i < rowCount(); ++i)
            {
                if (this->index(i, colSudId).data().toInt() == sudId)
                {
                    if (this->index(i, colVWH).data().toBool())
                    {
                        setData(this->index(i, colZeit), max);
                    }
                    else
                    {
                        QModelIndex index2 = this->index(i, colZeit);
                        int value = index2.data().toInt();
                        if (value > max)
                            setData(index2, max);
                    }
                }
            }
        }
        else if (field == "Nachisomerisierungszeit")
        {
            int min = -1 * index.data().toInt();
            int colZeit = fieldIndex("Zeit");
            for (int i = 0; i < rowCount(); ++i)
            {
                if (this->index(i, colSudId).data().toInt() == sudId)
                {
                    QModelIndex index2 = this->index(i, colZeit);
                    int value = index2.data().toInt();
                    if (value < min)
                        setData(index2, min);
                }
            }
        }
        for (int i = 0; i < rowCount(); ++i)
        {
            if (this->index(i, colSudId).data().toInt() == sudId)
            {
                QModelIndex index2 = this->index(i, colUpdate);
                setData(index2, data(index2));
            }
        }
        mSignalModifiedBlocked = false;
    }
}

void ModelHopfengaben::defaultValues(QVariantMap &values) const
{
    if (!values.contains("Name"))
        values.insert("Name", bh->modelHopfen()->data(0, "Beschreibung"));
    if (!values.contains("Prozent"))
        values.insert("Prozent", 0);
    if (!values.contains("Zeit"))
        values.insert("Zeit", 0);
    if (!values.contains("Vorderwuerze"))
        values.insert("Vorderwuerze", 0);
}
