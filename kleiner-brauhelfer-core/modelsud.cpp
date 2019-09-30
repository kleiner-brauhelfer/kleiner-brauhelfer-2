#include "modelsud.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include "modelausruestung.h"
#include "modelnachgaerverlauf.h"
#include <math.h>

ModelSud::ModelSud(Brauhelfer *bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh),
    mUpdating(false),
    mSkipUpdateOnOtherModelChanged(false),
    swWzMaischenRecipe(QVector<double>()),
    swWzKochenRecipe(QVector<double>()),
    swWzGaerungRecipe(QVector<double>()),
    swWzGaerungCurrent(QVector<double>())
{
    connect(this, SIGNAL(modelReset()), this, SLOT(onModelReset()));
    connect(this, SIGNAL(rowsInserted(const QModelIndex&,int,int)), this, SLOT(onModelReset()));
    connect(this, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), this, SLOT(onModelReset()));
    connect(this, SIGNAL(rowChanged(const QModelIndex&)), this, SLOT(onRowChanged(const QModelIndex&)));
    mVirtualField.append("SWIst");
    mVirtualField.append("SREIst");
    mVirtualField.append("MengeIst");
    mVirtualField.append("IbuIst");
    mVirtualField.append("FarbeIst");
    mVirtualField.append("CO2Ist");
    mVirtualField.append("Spundungsdruck");
    mVirtualField.append("Gruenschlauchzeitpunkt");
    mVirtualField.append("SpeiseNoetig");
    mVirtualField.append("SpeiseAnteil");
    mVirtualField.append("ZuckerAnteil");
    mVirtualField.append("Woche");
    mVirtualField.append("ReifezeitDelta");
    mVirtualField.append("AbfuellenBereitZutaten");
    mVirtualField.append("MengeSollKochbeginn");
    mVirtualField.append("MengeSollKochende");
    mVirtualField.append("WuerzemengeAnstellenTotal");
    mVirtualField.append("SW_Malz");
    mVirtualField.append("SW_WZ_Maischen");
    mVirtualField.append("SW_WZ_Kochen");
    mVirtualField.append("SW_WZ_Gaerung");
    mVirtualField.append("SWSollKochbeginn");
    mVirtualField.append("SWSollKochbeginnMitWz");
    mVirtualField.append("SWSollKochende");
    mVirtualField.append("SWSollAnstellen");
    mVirtualField.append("Verdampfungsziffer");
    mVirtualField.append("sEVG");
    mVirtualField.append("tEVG");
    mVirtualField.append("AnlageVerdampfungsziffer");
    mVirtualField.append("AnlageSudhausausbeute");
    mVirtualField.append("RestalkalitaetFaktor");
    mVirtualField.append("FaktorHauptgussEmpfehlung");
    mVirtualField.append("BewertungMittel");
}

void ModelSud::createConnections()
{
    connect(bh->modelMalzschuettung(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
    connect(bh->modelHopfengaben(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
    connect(bh->modelHefegaben(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
    connect(bh->modelWeitereZutatenGaben(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
    connect(bh->modelAusruestung(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onAnlageDataChanged(const QModelIndex&)));
}

void ModelSud::onModelReset()
{
    int rows = rowCount();
    swWzMaischenRecipe = QVector<double>(rows);
    swWzKochenRecipe = QVector<double>(rows);
    swWzGaerungRecipe = QVector<double>(rows);
    swWzGaerungCurrent = QVector<double>(rows);
    for (int r = 0; r < rows; ++r)
        updateSwWeitereZutaten(r);
    emit modified();
}

void ModelSud::onRowChanged(const QModelIndex &index)
{
    update(index.row());
}

void ModelSud::onOtherModelRowChanged(const QModelIndex &index)
{
    if (mSkipUpdateOnOtherModelChanged)
        return;
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    int sudId = model->data(index.row(), "SudID").toInt();
    int row = getRowWithValue("ID", sudId);
    update(row);
}

void ModelSud::onAnlageDataChanged(const QModelIndex &index)
{
    QVariant anlage = bh->modelAusruestung()->data(index.row(), "Name");
    for (int r = 0; r < rowCount(); ++r)
    {
        if (data(r, "Anlage") == anlage)
            update(r);
    }
}

QVariant ModelSud::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Braudatum")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Abfuelldatum")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Erstellt")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Gespeichert")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "SWIst")
    {
        return SWIst(index);
    }
    if (field == "SREIst")
    {
        return SREIst(index);
    }
    if (field == "MengeIst")
    {
        switch (data(index.row(), "Status").toInt())
        {
        default:
        case Sud_Status_Rezept:
            return data(index.row(), "Menge").toDouble();
        case Sud_Status_Gebraut:
            return data(index.row(), "WuerzemengeAnstellen").toDouble();
        case Sud_Status_Abgefuellt:
        case Sud_Status_Verbraucht:
            return data(index.row(), "erg_AbgefuellteBiermenge").toDouble();
        }
    }
    if (field == "IbuIst")
    {
        double mengeIst = data(index.row(), "MengeIst").toDouble();
        if (mengeIst <= 0.0)
            return 0.0;
        double mengeFaktor = data(index.row(), "Menge").toDouble() / mengeIst;
        return data(index.row(), "IBU").toDouble() * mengeFaktor;
    }
    if (field == "FarbeIst")
    {
        double mengeIst = data(index.row(), "MengeIst").toDouble();
        if (mengeIst <= 0.0)
            return 0.0;
        double mengeFaktor = data(index.row(), "Menge").toDouble() / mengeIst;
        return data(index.row(), "erg_Farbe").toDouble() * mengeFaktor;
    }
    if (field == "CO2Ist")
    {
        return CO2Ist(index);
    }
    if (field == "Spundungsdruck")
    {
        return Spundungsdruck(index);
    }
    if (field == "Gruenschlauchzeitpunkt")
    {
        return Gruenschlauchzeitpunkt(index);
    }
    if (field == "SpeiseNoetig")
    {
        return SpeiseNoetig(index);
    }
    if (field == "SpeiseAnteil")
    {
        return SpeiseAnteil(index);
    }
    if (field == "ZuckerAnteil")
    {
        return ZuckerAnteil(index);
    }
    if (field == "Woche")
    {
        if (data(index.row(), "Status").toInt() >= Sud_Status_Abgefuellt)
        {
            QDateTime dt = bh->modelNachgaerverlauf()->getLastDateTime(data(index.row(), "ID").toInt());
            if (!dt.isValid())
                dt = data(index.row(), "Abfuelldatum").toDateTime();
            if (dt.isValid())
                return dt.daysTo(QDateTime::currentDateTime()) / 7 + 1;
        }
        return 0;
    }
    if (field == "ReifezeitDelta")
    {
        return ReifezeitDelta(index);
    }
    if (field == "AbfuellenBereitZutaten")
    {
        return AbfuellenBereitZutaten(index);
    }
    if (field == "MengeSollKochbeginn")
    {
        return MengeSollKochbeginn(index);
    }
    if (field == "MengeSollKochende")
    {
        return MengeSollKochende(index);
    }
    if (field == "WuerzemengeAnstellenTotal")
    {
        return data(index.row(), "WuerzemengeAnstellen").toDouble() + data(index.row(), "Speisemenge").toDouble();
    }
    if (field == "SW_Malz")
    {
        return data(index.row(), "SW").toDouble() - swWzMaischenRecipe[index.row()] - swWzKochenRecipe[index.row()] - swWzGaerungRecipe[index.row()];
    }
    if (field == "SW_WZ_Maischen")
    {
        return swWzMaischenRecipe[index.row()];
    }
    if (field == "SW_WZ_Kochen")
    {
        return swWzKochenRecipe[index.row()];
    }
    if (field == "SW_WZ_Gaerung")
    {
        return swWzGaerungRecipe[index.row()];
    }
    if (field == "SWSollKochbeginn")
    {
        return SWSollKochbeginn(index);
    }
    if (field == "SWSollKochbeginnMitWz")
    {
        return SWSollKochbeginnMitWz(index);
    }
    if (field == "SWSollKochende")
    {
        return SWSollKochende(index);
    }
    if (field == "SWSollAnstellen")
    {
        return SWSollAnstellen(index);
    }
    if (field == "Verdampfungsziffer")
    {
        return Verdampfungsziffer(index);
    }
    if (field == "sEVG")
    {
        double sw = data(index.row(), "SWIst").toDouble();
        double sre = data(index.row(), "SREIst").toDouble();
        return BierCalc::vergaerungsgrad(sw, sre);
    }
    if (field == "tEVG")
    {
        double sw = data(index.row(), "SWIst").toDouble();
        double sre = data(index.row(), "SREIst").toDouble();
        double tre = BierCalc::toTRE(sw, sre);
        return BierCalc::vergaerungsgrad(sw, tre);
    }
    if (field == "AnlageVerdampfungsziffer")
    {
        return dataAnlage(index.row(), "Verdampfungsziffer");
    }
    if (field == "AnlageSudhausausbeute")
    {
        return dataAnlage(index.row(), "Sudhausausbeute");
    }
    if (field == "RestalkalitaetFaktor")
    {
        return RestalkalitaetFaktor(index);
    }
    if (field == "FaktorHauptgussEmpfehlung")
    {
        return FaktorHauptgussEmpfehlung(index);
    }
    if (field == "BewertungMittel")
    {
        int sudId = index.sibling(index.row(), fieldIndex("ID")).data().toInt();
        return bh->modelBewertungen()->mean(sudId);
    }
    return QVariant();
}

bool ModelSud::setDataExt(const QModelIndex &index, const QVariant &value)
{
    bool ret;
    mSkipUpdateOnOtherModelChanged = true;
    ret = setDataExt_impl(index, value);
    mSkipUpdateOnOtherModelChanged = false;
    return ret;
}

bool ModelSud::setDataExt_impl(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Braudatum")
    {
        if (QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate)))
        {
            setData(index.row(), "Abfuelldatum", value);
            return true;
        }
        return false;
    }
    if (field == "Abfuelldatum")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Erstellt")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Gespeichert")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
    }
    if (field == "Menge")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "WuerzemengeVorHopfenseihen", data(index.row(), "MengeSollKochbeginn"));
            return true;
        }
        return false;
    }
    if (field == "WuerzemengeVorHopfenseihen")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "WuerzemengeKochende", value);
            return true;
        }
        return false;
    }
    if (field == "WuerzemengeKochende")
    {
        if (QSqlTableModel::setData(index, value))
        {
            double m = value.toDouble() + data(index.row(), "Menge").toDouble() - data(index.row(), "MengeSollKochende").toDouble();
            setData(index.row(), "WuerzemengeAnstellenTotal", m);
            return true;
        }
        return false;
    }
    if (field == "WuerzemengeAnstellenTotal")
    {
        double v = value.toDouble() - data(index.row(), "Speisemenge").toDouble();
        return setData(index.row(), "WuerzemengeAnstellen", v);
    }
    if (field == "WuerzemengeAnstellen")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "JungbiermengeAbfuellen", value);
            return true;
        }
        return false;
    }
    if (field == "Speisemenge")
    {
        if (data(index.row(), "Status").toInt() == Sud_Status_Rezept)
        {
            double v = data(index.row(), "WuerzemengeAnstellenTotal").toDouble() - value.toDouble();
            if (QSqlTableModel::setData(index, value))
            {
                QSqlTableModel::setData(this->index(index.row(), fieldIndex("WuerzemengeAnstellen")), v);
                return true;
            }
            return false;
        }
        else
        {
            return QSqlTableModel::setData(index, value);
        }
    }
    if (field == "erg_AbgefuellteBiermenge")
    {
        double speise = data(index.row(), "SpeiseAnteil").toDouble() / 1000;
        double jungbiermenge = data(index.row(), "JungbiermengeAbfuellen").toDouble();
        if (QSqlTableModel::setData(index, value))
        {
            if (jungbiermenge > 0.0)
                QSqlTableModel::setData(this->index(index.row(), fieldIndex("JungbiermengeAbfuellen")), value.toDouble() / (1 + speise / jungbiermenge));
            else
                QSqlTableModel::setData(this->index(index.row(), fieldIndex("JungbiermengeAbfuellen")), value.toDouble() - speise);
            return true;
        }
        return false;
    }
    if (field == "SW")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "SWKochende", value);
            return true;
        }
        return false;
    }
    if (field == "SWKochende")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "SWAnstellen", value);
            return true;
        }
        return false;
    }
    if (field == "SWAnstellen")
    {
        if (QSqlTableModel::setData(index, value))
        {
            setData(index.row(), "SWSchnellgaerprobe", value);
            setData(index.row(), "SWJungbier", value);
            return true;
        }
        return false;
    }
    if (field == "KochdauerNachBitterhopfung")
    {
        if (QSqlTableModel::setData(index, value))
        {
            updateKochdauer(index.row(), value);
            return true;
        }
        return false;
    }
    return false;
}

Qt::ItemFlags ModelSud::flags(const QModelIndex &index) const
{
    Qt::ItemFlags itemFlags = SqlTableModel::flags(index);
    QString field = fieldName(index.column());
    if (field == "WuerzemengeAnstellenTotal")
        itemFlags |= Qt::ItemIsEditable;
    return itemFlags;
}

QVariant ModelSud::dataAnlage(int row, const QString& fieldName) const
{
    QVariant anlage = data(row, "Anlage");
    return bh->modelAusruestung()->getValueFromSameRow("Name", anlage, fieldName);
}

QVariant ModelSud::dataWasser(int row, const QString& fieldName) const
{
    QVariant profil = data(row, "Wasserprofil");
    return bh->modelWasser()->getValueFromSameRow("Name", profil, fieldName);
}

void ModelSud::update(int row)
{
    if (row < 0 || row >= rowCount())
        return;

    if (mUpdating)
        return;
    mUpdating = true;
    mSignalModifiedBlocked = true;

    double menge, sw;

    updateSwWeitereZutaten(row);

    int status = data(row, "Status").toInt();
    if (status == Sud_Status_Rezept)
    {
        // recipe
        double mengeRecipe = data(row, "Menge").toDouble();
        double swRecipe = data(row, "SW").toDouble();
        double hgf = 1 + data(row, "highGravityFaktor").toInt() / 100.0;

        // erg_S_Gesamt
        sw = swRecipe - swWzMaischenRecipe[row] - swWzKochenRecipe[row] - swWzGaerungRecipe[row];
        double ausb = dataAnlage(row, "Sudhausausbeute").toDouble();
        double schuet = BierCalc::schuettung(sw * hgf, mengeRecipe / hgf, ausb, true);
        setData(row, "erg_S_Gesamt", schuet);

        // erg_Farbe
        updateFarbe(row);

        // erg_WHauptguss
        double fac = data(row, "FaktorHauptguss").toDouble();
        double hg = schuet * fac;
        setData(row, "erg_WHauptguss", hg);

        // erg_WNachguss
        menge = data(row, "MengeSollKochbeginn").toDouble();
        double KorrekturWasser = dataAnlage(row, "KorrekturWasser").toDouble();
        double ng = menge + schuet * 0.96 - hg + KorrekturWasser;
        setData(row, "erg_WNachguss", ng);

        // erg_W_Gesamt
        setData(row, "erg_W_Gesamt", hg + ng);

        // erg_Sudhausausbeute
        sw = data(row, "SWKochende").toDouble() - swWzMaischenRecipe[row] - swWzKochenRecipe[row];
        menge = data(row, "WuerzemengeKochende").toDouble();
        setData(row, "erg_Sudhausausbeute", BierCalc::sudhausausbeute(sw, menge, schuet, true));

        // erg_EffektiveAusbeute
        sw = data(row, "SWAnstellen").toDouble() * hgf - swWzMaischenRecipe[row] - swWzKochenRecipe[row];
        menge = data(row, "WuerzemengeAnstellenTotal").toDouble() / hgf;
        setData(row, "erg_EffektiveAusbeute", BierCalc::sudhausausbeute(sw , menge, schuet, true));

        // erg_Preis
        updatePreis(row);
    }
    if (status == Sud_Status_Gebraut)
    {
        // erg_Alkohol
        double sre = data(row, "SREIst").toDouble();
        sw = data(row, "SWAnstellen").toDouble() + swWzGaerungCurrent[row];
        menge = data(row, "WuerzemengeAnstellen").toDouble();
        if (menge > 0.0)
            sw += (data(row, "ZuckerAnteil").toDouble() / 10) / menge;
        setData(row, "erg_Alkohol", BierCalc::alkohol(sw, sre));

        // erg_AbgefuellteBiermenge
        double jungbiermenge = data(row, "JungbiermengeAbfuellen").toDouble();
        double speise = data(row, "SpeiseAnteil").toDouble() / 1000;
        setData(row, "erg_AbgefuellteBiermenge", jungbiermenge + speise);

        // erg_Preis
        updatePreis(row);
    }

    setData(row, "Gespeichert", QDateTime::currentDateTime());

    mSignalModifiedBlocked = false;
    mUpdating = false;
}

void ModelSud::updateSwWeitereZutaten(int row)
{
    QRegExp sudReg = QRegExp(QString("^%1$").arg(data(row, "ID").toInt()));

    swWzMaischenRecipe[row] = 0.0;
    swWzKochenRecipe[row] = 0.0;
    swWzGaerungRecipe[row] = 0.0;
    swWzGaerungCurrent[row] = 0.0;

    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    modelWeitereZutatenGaben.setFilterRegExp(sudReg);
    for (int i = 0; i < modelWeitereZutatenGaben.rowCount(); ++i)
    {
        double ausbeute = modelWeitereZutatenGaben.data(i, "Ausbeute").toDouble();
        if (ausbeute > 0.0)
        {
            double sw = modelWeitereZutatenGaben.data(i, "Menge").toDouble() * ausbeute / 1000;
            switch (modelWeitereZutatenGaben.data(i, "Zeitpunkt").toInt())
            {
            case EWZ_Zeitpunkt_Gaerung:
                swWzGaerungRecipe[row] += sw ;
                if (modelWeitereZutatenGaben.data(i, "Zugabestatus").toInt() != EWZ_Zugabestatus_nichtZugegeben)
                    swWzGaerungCurrent[row] += sw;
                break;
            case EWZ_Zeitpunkt_Kochen:
                swWzKochenRecipe[row] += sw;
                break;
            case EWZ_Zeitpunkt_Maischen:
                swWzMaischenRecipe[row] += sw;
                break;
            }
        }
    }
}

void ModelSud::updateFarbe(int row)
{
    QRegExp sudReg = QRegExp(QString("^%1$").arg(data(row, "ID").toInt()));
    double ebc = 0.0;
    double d = 0.0;
    double gs = 0.0;

    ProxyModel modelMalzschuettung;
    modelMalzschuettung.setSourceModel(bh->modelMalzschuettung());
    modelMalzschuettung.setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
    modelMalzschuettung.setFilterRegExp(sudReg);
    int colFarbe = bh->modelMalzschuettung()->fieldIndex("Farbe");
    int colMenge = bh->modelMalzschuettung()->fieldIndex("erg_Menge");
    for (int i = 0; i < modelMalzschuettung.rowCount(); ++i)
    {
        double farbe = modelMalzschuettung.index(i, colFarbe).data().toDouble();
        double menge = modelMalzschuettung.index(i, colMenge).data().toDouble();
        d += menge * farbe;
        gs += menge;
    }

    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    modelWeitereZutatenGaben.setFilterRegExp(sudReg);
    colFarbe = bh->modelWeitereZutatenGaben()->fieldIndex("Farbe");
    colMenge = bh->modelWeitereZutatenGaben()->fieldIndex("erg_Menge");
    for (int i = 0; i < modelWeitereZutatenGaben.rowCount(); ++i)
    {
        double farbe = modelWeitereZutatenGaben.index(i, colFarbe).data().toDouble();
        if (farbe > 0.0)
        {
            double menge = modelWeitereZutatenGaben.index(i, colMenge).data().toDouble() / 1000;
            d += menge * farbe;
            gs += menge;
        }
    }
    if (gs > 0.0)
    {
        double sw = data(row, "SW").toDouble() - swWzKochenRecipe[row] - swWzGaerungRecipe[row];
        ebc = (d / gs) * sw / 10 + 2;
        ebc += dataAnlage(row, "KorrekturFarbe").toDouble();
    }
    setData(row, "erg_Farbe", ebc);
}

void ModelSud::updatePreis(int row)
{
    QRegExp sudReg = QRegExp(QString("^%1$").arg(data(row, "ID").toInt()));
    double summe = 0.0;

    double kostenSchuettung = 0.0;
    ProxyModel modelMalzschuettung;
    modelMalzschuettung.setSourceModel(bh->modelMalzschuettung());
    modelMalzschuettung.setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
    modelMalzschuettung.setFilterRegExp(sudReg);
    for (int o = 0; o < modelMalzschuettung.rowCount(); ++o)
    {
        QVariant name = modelMalzschuettung.data(o, "Name");
        double menge = modelMalzschuettung.data(o, "erg_Menge").toDouble();
        double preis = bh->modelMalz()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
        kostenSchuettung += preis * menge;
    }
    summe += kostenSchuettung;

    double kostenHopfen = 0.0;
    ProxyModel modelHopfengaben;
    modelHopfengaben.setSourceModel(bh->modelHopfengaben());
    modelHopfengaben.setFilterKeyColumn(bh->modelHopfengaben()->fieldIndex("SudID"));
    modelHopfengaben.setFilterRegExp(sudReg);
    for (int o = 0; o < modelHopfengaben.rowCount(); ++o)
    {
        QVariant name = modelHopfengaben.data(o, "Name");
        double menge = modelHopfengaben.data(o, "erg_Menge").toDouble();
        double preis = bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
        kostenHopfen += preis * menge / 1000;
    }
    summe += kostenHopfen;

    double kostenHefe = 0.0;
    ProxyModel modelHefegaben;
    modelHefegaben.setSourceModel(bh->modelHefegaben());
    modelHefegaben.setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
    modelHefegaben.setFilterRegExp(sudReg);
    for (int o = 0; o < modelHefegaben.rowCount(); ++o)
    {
        QVariant name = modelHefegaben.data(o, "Name");
        int menge = modelHefegaben.data(o, "Menge").toInt();
        double preis = bh->modelHefe()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
        kostenHefe += preis * menge;
    }
    summe += kostenHefe;

    //Kosten der Weiteren Zutaten
    double kostenWeitereZutaten = 0.0;
    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    modelWeitereZutatenGaben.setFilterRegExp(sudReg);
    for (int o = 0; o < modelWeitereZutatenGaben.rowCount(); ++o)
    {
        QVariant name = modelWeitereZutatenGaben.data(o, "Name");
        double menge = modelWeitereZutatenGaben.data(o, "erg_Menge").toDouble();
        int typ = modelWeitereZutatenGaben.data(o, "Typ").toInt();
        double preis;
        if (typ == EWZ_Typ_Hopfen)
            preis = bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
        else
            preis = bh->modelWeitereZutaten()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
        kostenWeitereZutaten += preis * menge / 1000;
    }
    summe += kostenWeitereZutaten;

    double kostenSonstiges = data(row, "KostenWasserStrom").toDouble();
    summe += kostenSonstiges;

    double kostenAnlage = dataAnlage(row, "Kosten").toDouble();
    summe += kostenAnlage;

    setData(row, "erg_Preis", summe / data(row, "MengeIst").toDouble());
}

void ModelSud::updateKochdauer(int row, const QVariant &value)
{
    int id = data(row, "ID").toInt();
    double T = value.toDouble();
    SqlTableModel* model = bh->modelHopfengaben();
    int colSudId = model->fieldIndex("SudID");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->data(model->index(i, colSudId)).toInt() == id)
        {
            if (model->data(i, "Vorderwuerze").toBool())
            {
                model->setData(i, "Zeit", value);
            }
            else
            {
               if (model->data(i, "Zeit").toDouble() >= T)
                   model->setData(i, "Zeit", value);
            }
        }
    }
}

QVariant ModelSud::SWIst(const QModelIndex &index) const
{
    return data(index.row(), "SWAnstellen").toDouble() + swWzGaerungCurrent[index.row()];
}

QVariant ModelSud::SREIst(const QModelIndex &index) const
{
    if (data(index.row(), "SchnellgaerprobeAktiv").toBool())
        return data(index.row(), "SWSchnellgaerprobe").toDouble();
    else
        return data(index.row(), "SWJungbier").toDouble();
}

QVariant ModelSud::CO2Ist(const QModelIndex &index) const
{
    return bh->modelNachgaerverlauf()->getLastCO2(data(index.row(), "ID").toInt());
}

QVariant ModelSud::Spundungsdruck(const QModelIndex &index) const
{
    double co2 = data(index.row(), "CO2").toDouble();
    double T = data(index.row(), "TemperaturJungbier").toDouble();
    return BierCalc::spundungsdruck(co2, T);
}

QVariant ModelSud::Gruenschlauchzeitpunkt(const QModelIndex &index) const
{
    double co2Soll = data(index.row(), "CO2").toDouble();
    double sw = data(index.row(), "SWIst").toDouble();
    double T = data(index.row(), "TemperaturJungbier").toDouble();
    double sre = data(index.row(), "SWSchnellgaerprobe").toDouble();
    return BierCalc::gruenschlauchzeitpunkt(co2Soll, sw, sre, T);
}

QVariant ModelSud::SpeiseNoetig(const QModelIndex &index) const
{
    double co2Soll = data(index.row(), "CO2").toDouble();
    double sw = data(index.row(), "SWIst").toDouble();
    double sreJungbier = data(index.row(), "SWJungbier").toDouble();
    double T = data(index.row(), "TemperaturJungbier").toDouble();
    double sreSchnellgaerprobe = data(index.row(), "SREIst").toDouble();
    double jungbiermenge = data(index.row(), "JungbiermengeAbfuellen").toDouble();
    return BierCalc::speise(co2Soll, sw, sreSchnellgaerprobe, sreJungbier, T) * jungbiermenge * 1000;
}

QVariant ModelSud::SpeiseAnteil(const QModelIndex &index) const
{
    if (data(index.row(), "Spunden").toBool())
        return 0.0;
    double speiseVerfuegbar = data(index.row(), "Speisemenge").toDouble() * 1000;
    double speise = SpeiseNoetig(index).toDouble();
    if (speise > speiseVerfuegbar)
        speise = speiseVerfuegbar;
    return speise;
}

QVariant ModelSud::ZuckerAnteil(const QModelIndex &index) const
{
    if (data(index.row(), "Spunden").toBool())
        return 0.0;
    double speiseVerfuegbar = data(index.row(), "Speisemenge").toDouble() * 1000;
    double sw = data(index.row(), "SWIst").toDouble();
    double sre = data(index.row(), "SREIst").toDouble();
    double speise = SpeiseNoetig(index).toDouble() - speiseVerfuegbar;
    if (speise <= 0.0)
        return 0.0;
    return BierCalc::speiseToZucker(sw, sre, speise);
}

QVariant ModelSud::ReifezeitDelta(const QModelIndex &index) const
{
    if (data(index.row(), "Status").toInt() >= Sud_Status_Abgefuellt)
    {
        QDateTime dt = bh->modelNachgaerverlauf()->getLastDateTime(data(index.row(), "ID").toInt());
        if (!dt.isValid())
            dt = data(index.row(), "Abfuelldatum").toDateTime();
        if (dt.isValid())
        {
            qint64 tageReifung = dt.daysTo(QDateTime::currentDateTime());
            int tageReifungSoll = data(index.row(), "Reifezeit").toInt() * 7;
            return tageReifungSoll - tageReifung;
        }
    }
    return 0;
}

QVariant ModelSud::AbfuellenBereitZutaten(const QModelIndex &index) const
{
    ProxyModel modelHefegaben;
    modelHefegaben.setSourceModel(bh->modelHefegaben());
    modelHefegaben.setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
    modelHefegaben.setFilterRegExp(QString("^%1$").arg(data(index.row(), "ID").toInt()));
    for (int i = 0; i < modelHefegaben.rowCount(); ++i)
    {
        if (!modelHefegaben.data(i, "Abfuellbereit").toBool())
            return false;
    }

    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    modelWeitereZutatenGaben.setFilterRegExp(QString("^%1$").arg(data(index.row(), "ID").toInt()));
    for (int i = 0; i < modelWeitereZutatenGaben.rowCount(); ++i)
    {
        if (!modelWeitereZutatenGaben.data(i, "Abfuellbereit").toBool())
            return false;
    }
    return true;
}

QVariant ModelSud::MengeSollKochbeginn(const QModelIndex &index) const
{
    double mengeSollKochEnde = data(index.row(), "MengeSollKochende").toDouble();
    double kochdauer = data(index.row(), "KochdauerNachBitterhopfung").toDouble();
    double verdampfungsziffer = dataAnlage(index.row(), "Verdampfungsziffer").toDouble();
    return mengeSollKochEnde * (1 + (verdampfungsziffer * kochdauer / (60 * 100)));
}

QVariant ModelSud::MengeSollKochende(const QModelIndex &index) const
{
    double mengeSoll = data(index.row(), "Menge").toDouble();
    double hgf = 1 + data(index.row(), "highGravityFaktor").toInt() / 100.0;
    return mengeSoll / hgf;
}

QVariant ModelSud::SWSollKochbeginn(const QModelIndex &index) const
{
    double sw = data(index.row(), "SW").toDouble() - swWzKochenRecipe[index.row()] - swWzGaerungRecipe[index.row()];
    double hgf = 1 + data(index.row(), "highGravityFaktor").toInt() / 100.0;
    double kochdauer = data(index.row(), "KochdauerNachBitterhopfung").toDouble();
    double verdampfungsziffer = dataAnlage(index.row(), "Verdampfungsziffer").toDouble();
    return sw * hgf / (1 + (verdampfungsziffer * kochdauer / (60 * 100)));
}

QVariant ModelSud::SWSollKochbeginnMitWz(const QModelIndex &index) const
{
    double sw = data(index.row(), "SWSollKochende").toDouble();
    double kochdauer = data(index.row(), "KochdauerNachBitterhopfung").toDouble();
    double verdampfungsziffer = dataAnlage(index.row(), "Verdampfungsziffer").toDouble();
    return sw / (1 + (verdampfungsziffer * kochdauer / (60 * 100)));
}

QVariant ModelSud::SWSollKochende(const QModelIndex &index) const
{
    double sw = data(index.row(), "SWSollAnstellen").toDouble();
    double hgf = 1 + data(index.row(), "highGravityFaktor").toInt() / 100.0;
    return sw * hgf;
}

QVariant ModelSud::SWSollAnstellen(const QModelIndex &index) const
{
    double sw = data(index.row(), "SW").toDouble();
    return sw - swWzGaerungRecipe[index.row()];
}

QVariant ModelSud::Verdampfungsziffer(const QModelIndex &index) const
{
    double V1 = data(index.row(), "WuerzemengeVorHopfenseihen").toDouble();
    double V2 = data(index.row(), "WuerzemengeKochende").toDouble();
    double t = data(index.row(), "KochdauerNachBitterhopfung").toDouble();
    return BierCalc::verdampfungsziffer(V1, V2, t);
}

QVariant ModelSud::RestalkalitaetFaktor(const QModelIndex &index) const
{
    double ist = bh->modelWasser()->getValueFromSameRow("Name", data(index.row(), "Wasserprofil"), "Restalkalitaet").toDouble();
    double soll = data(index.row(), "RestalkalitaetSoll").toDouble();
    double fac = (ist -  soll) * 0.033333333;
    if (fac < 0.0)
        fac = 0.0;
    return fac;
}

QVariant ModelSud::FaktorHauptgussEmpfehlung(const QModelIndex &index) const
{
    double ebc = data(index.row(), "erg_Farbe").toDouble();
    if (ebc < 50)
        return 4.0 - ebc * 0.02;
    else
        return 3.0;
}

bool ModelSud::removeRows(int row, int count, const QModelIndex &parent)
{
    if (SqlTableModel::removeRows(row, count, parent))
    {
        for (int i = 0; i < count; ++i)
        {
            int sudId = data(row + i, "ID").toInt();
            removeRowsFrom(bh->modelRasten(), sudId);
            removeRowsFrom(bh->modelMalzschuettung(), sudId);
            removeRowsFrom(bh->modelHopfengaben(), sudId);
            removeRowsFrom(bh->modelWeitereZutatenGaben(), sudId);
            removeRowsFrom(bh->modelHefegaben(), sudId);
            removeRowsFrom(bh->modelSchnellgaerverlauf(), sudId);
            removeRowsFrom(bh->modelHauptgaerverlauf(), sudId);
            removeRowsFrom(bh->modelNachgaerverlauf(), sudId);
            removeRowsFrom(bh->modelBewertungen(), sudId);
            removeRowsFrom(bh->modelAnhang(), sudId);
            removeRowsFrom(bh->modelFlaschenlabel(), sudId);
            removeRowsFrom(bh->modelFlaschenlabelTags(), sudId);
        }
        return true;
    }
    return false;
}

void ModelSud::removeRowsFrom(SqlTableModel* model, int sudId)
{
    int colId = model->fieldIndex("SudID");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->index(i, colId).data().toInt() == sudId)
            model->removeRows(i);
    }
}

void ModelSud::defaultValues(QVariantMap &values) const
{
    if (!values.contains("ID"))
        values.insert("ID", getNextId());
    if (!values.contains("Erstellt"))
        values.insert("Erstellt", QDateTime::currentDateTime());
    if (!values.contains("Sudnummer"))
        values.insert("Sudnummer", 0);
    if (!values.contains("Menge"))
        values.insert("Menge", 20);
    if (!values.contains("SW"))
        values.insert("SW", 12);
    if (!values.contains("CO2"))
        values.insert("CO2", 5);
    if (!values.contains("IBU"))
        values.insert("IBU", 26);
    if (!values.contains("KochdauerNachBitterhopfung"))
        values.insert("KochdauerNachBitterhopfung", 60);
    if (!values.contains("berechnungsArtHopfen"))
        values.insert("berechnungsArtHopfen", Hopfen_Berechnung_IBU);
    if (!values.contains("TemperaturJungbier"))
        values.insert("TemperaturJungbier", 20.0);
    if (!values.contains("Status"))
        values.insert("Status", Sud_Status_Rezept);
    if (!values.contains("Anlage") && bh->modelAusruestung()->rowCount() == 1)
        values.insert("Anlage", bh->modelAusruestung()->data(0, "Name"));
    if (!values.contains("Wasserprofil") && bh->modelWasser()->rowCount() == 1)
        values.insert("Wasserprofil", bh->modelWasser()->data(0, "Name"));
}

QVariantMap ModelSud::copyValues(int row) const
{
    QVariantMap values = SqlTableModel::copyValues(row);
    values.insert("ID", getNextId());
    return values;
}
