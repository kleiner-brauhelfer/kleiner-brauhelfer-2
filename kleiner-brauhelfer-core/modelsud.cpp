#include "modelsud.h"
#include "database_defs.h"
#include "brauhelfer.h"
#include "modelausruestung.h"
#include "modelnachgaerverlauf.h"
#include <QSqlQuery>
#include <math.h>

ModelSud::ModelSud(Brauhelfer *bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh),
    updating(false),
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
    mVirtualField.append("ReifezeitDelta");
    mVirtualField.append("AbfuellenBereitZutaten");
    mVirtualField.append("MengeSollKochbeginn");
    mVirtualField.append("MengeSollKochende");
    mVirtualField.append("WuerzemengeAnstellenTotal");
    mVirtualField.append("SWSollLautern");
    mVirtualField.append("SWSollKochbeginn");
    mVirtualField.append("SWSollKochende");
    mVirtualField.append("SWSollAnstellen");
    mVirtualField.append("Verdampfungsziffer");
    mVirtualField.append("sEVG");
    mVirtualField.append("tEVG");
    mVirtualField.append("AnlageVerdampfungsziffer");
    mVirtualField.append("AnlageSudhausausbeute");
    mVirtualField.append("RestalkalitaetFaktor");
    mVirtualField.append("FaktorHauptgussEmpfehlung");
    mVirtualField.append("BewertungMax");
}

void ModelSud::createConnections()
{
    connect(bh->modelMalzschuettung(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
    connect(bh->modelWeitereZutatenGaben(), SIGNAL(rowChanged(const QModelIndex&)),
            this, SLOT(onOtherModelRowChanged(const QModelIndex&)));
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
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    int sudId = model->data(index.row(), "SudID").toInt();
    int row = getRowWithValue("ID", sudId);
    update(row);
}

QVariant ModelSud::dataExt(const QModelIndex &index) const
{
    QString field = fieldName(index.column());
    if (field == "Braudatum")
    {
        return QDateTime::fromString(QSqlTableModel::data(index).toString(), Qt::ISODate);
    }
    if (field == "Anstelldatum")
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
        double menge = 0.0;
        if (data(index.row(), "BierWurdeAbgefuellt").toBool())
            menge = data(index.row(), "erg_AbgefuellteBiermenge").toDouble();
        else if (data(index.row(), "BierWurdeGebraut").toBool())
            menge = data(index.row(), "WuerzemengeAnstellen").toDouble();
        else
            menge = data(index.row(), "Menge").toDouble();
        return menge;
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
    if (field == "SWSollLautern")
    {
        return SWSollLautern(index);
    }
    if (field == "SWSollKochbeginn")
    {
        return SWSollKochbeginn(index);
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
    if (field == "BewertungMax")
    {
        int sudId = index.siblingAtColumn(fieldIndex("ID")).data().toInt();
        return bh->modelBewertungen()->max(sudId);
    }
    return QVariant();
}

bool ModelSud::setDataExt(const QModelIndex &index, const QVariant &value)
{
    QString field = fieldName(index.column());
    if (field == "Braudatum")
    {
        if (QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate)))
        {
            setData(index.row(), "Anstelldatum", value);
            setData(index.row(), "Abfuelldatum", value);
            return true;
        }
        return false;
    }
    if (field == "Anstelldatum")
    {
        return QSqlTableModel::setData(index, value.toDateTime().toString(Qt::ISODate));
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
    if (field == "BierWurdeGebraut")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (!value.toBool())
            {
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("BierWurdeAbgefuellt")), false);
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("BierWurdeVerbraucht")), false);

            }
            return true;
        }
        return false;
    }
    if (field == "BierWurdeAbgefuellt")
    {
        if (QSqlTableModel::setData(index, value))
        {
            if (!value.toBool())
            {
                QSqlTableModel::setData(index.siblingAtColumn(fieldIndex("BierWurdeVerbraucht")), false);
            }
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
        double v = data(index.row(), "WuerzemengeAnstellenTotal").toDouble() - value.toDouble();
        if (QSqlTableModel::setData(index, value))
        {
            QSqlTableModel::setData(this->index(index.row(), fieldIndex("WuerzemengeAnstellen")), v);
            return true;
        }
        return false;
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
    if (field == "AuswahlBrauanlage")
    {
        if (QSqlTableModel::setData(index, value))
        {
            QString name = bh->modelAusruestung()->name(value.toInt());
            QSqlTableModel::setData(this->index(index.row(), fieldIndex("AuswahlBrauanlageName")), name);
            return true;
        }
        return false;
    }
    if (field == "AuswahlBrauanlageName")
    {
        if (QSqlTableModel::setData(index, value))
        {
            int id = bh->modelAusruestung()->id(value.toString());
            QSqlTableModel::setData(this->index(index.row(), fieldIndex("AuswahlBrauanlage")), id);
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
    int anlage = data(row, "AuswahlBrauanlage").toInt();
    SqlTableModel* model = bh->modelAusruestung();
    int col = model->fieldIndex("AnlagenID");
    for (int i = 0; i < model->rowCount(); ++i)
        if (model->data(model->index(i, col)).toInt() == anlage)
            return model->data(i, fieldName);
    return QVariant();
}

void ModelSud::update(int row)
{
    if (row < 0 || row >= rowCount())
        return;

    if (updating)
        return;
    updating = true;

    double menge, sw;

    updateSwWeitereZutaten(row);

    if (!data(row, "BierWurdeGebraut").toBool())
    {
        // recipe
        double mengeRecipe = data(row, "Menge").toDouble();
        double swRecipe = data(row, "SW").toDouble();
        double hgf = 1 + data(row, "highGravityFaktor").toInt() / 100.0;

        // erg_S_Gesammt
        sw = swRecipe - swWzMaischenRecipe[row] - swWzKochenRecipe[row] - swWzGaerungRecipe[row];
        double ausb = dataAnlage(row, "Sudhausausbeute").toDouble();
        double schuet = mengeRecipe * BierCalc::platoToDichte(sw * hgf) * sw / ausb;
        setData(row, "erg_S_Gesammt", schuet);

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

        // erg_W_Gesammt
        setData(row, "erg_W_Gesammt", hg + ng);

        // erg_Sudhausausbeute
        sw = data(row, "SWKochende").toDouble() - swWzMaischenRecipe[row] - swWzKochenRecipe[row];
        menge = data(row, "WuerzemengeKochende").toDouble();
        setData(row, "erg_Sudhausausbeute", BierCalc::sudhausausbeute(sw, menge, schuet));

        // erg_EffektiveAusbeute
        sw = data(row, "SWAnstellen").toDouble() * hgf - swWzMaischenRecipe[row] - swWzKochenRecipe[row];
        menge = (data(row, "WuerzemengeAnstellen").toDouble() + data(row, "Speisemenge").toDouble()) / hgf;
        setData(row, "erg_EffektiveAusbeute", BierCalc::sudhausausbeute(sw , menge, schuet));
    }
    if (!data(row, "BierWurdeAbgefuellt").toBool())
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

    updating = false;
}

void ModelSud::updateSwWeitereZutaten(int row)
{
    int id = data(row, "ID").toInt();
    SqlTableModel* model = bh->modelWeitereZutatenGaben();
    int colSudId = model->fieldIndex("SudID");

    swWzMaischenRecipe[row] = 0.0;
    swWzKochenRecipe[row] = 0.0;
    swWzGaerungRecipe[row] = 0.0;
    swWzGaerungCurrent[row] = 0.0;

    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->data(model->index(i, colSudId)).toInt() == id)
        {
            double ausbeute = model->data(i, "Ausbeute").toDouble();
            if (ausbeute > 0.0)
            {
                double sw = model->data(i, "Menge").toDouble() * ausbeute / 1000;
                switch (model->data(i, "Zeitpunkt").toInt())
                {
                case EWZ_Zeitpunkt_Gaerung:
                    swWzGaerungRecipe[row] += sw ;
                    if (model->data(i, "Zugabestatus").toInt() != EWZ_Zugabestatus_nichtZugegeben)
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
}

void ModelSud::updateFarbe(int row)
{
    int id = data(row, "ID").toInt();
    double ebc = 0.0;
    double d = 0.0;
    double gs = 0.0;
    SqlTableModel* model = bh->modelMalzschuettung();
    int colSudId = model->fieldIndex("SudID");
    int colFarbe = model->fieldIndex("Farbe");
    int colMenge = model->fieldIndex("erg_Menge");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->index(i, colSudId).data().toInt() == id)
        {
            double farbe = model->index(i, colFarbe).data().toDouble();
            double menge = model->index(i, colMenge).data().toDouble();
            d += menge * farbe;
            gs += menge;
        }
    }
    model = bh->modelWeitereZutatenGaben();
    colSudId = model->fieldIndex("SudID");
    int colTyp = model->fieldIndex("Typ");
    colFarbe = model->fieldIndex("Farbe");
    colMenge = model->fieldIndex("erg_Menge");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->index(i, colSudId).data().toInt() == id &&
            model->index(i, colTyp).data().toInt() != EWZ_Typ_Hopfen)
        {
            double farbe = model->index(i, colFarbe).data().toDouble();
            if (farbe > 0.0)
            {
                double menge = model->index(i, colMenge).data().toDouble() / 1000;
                d += menge * farbe;
                gs += menge;
            }
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
    int id = data(row, "ID").toInt();
    int colSudId;
    SqlTableModel *model;
    double summe = 0.0;

    double kostenSchuettung = 0.0;
    model = bh->modelMalzschuettung();
    colSudId = model->fieldIndex("SudID");
    for (int o = 0; o < model->rowCount(); ++o)
    {
        if (model->data(model->index(o, colSudId)).toInt() == id)
        {
            QVariant name = model->data(o, "Name");
            double menge = model->data(o, "erg_Menge").toDouble();
            double preis = bh->modelMalz()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
            kostenSchuettung += preis * menge;
        }
    }
    summe += kostenSchuettung;

    double kostenHopfen = 0.0;
    model = bh->modelHopfengaben();
    colSudId = model->fieldIndex("SudID");
    for (int o = 0; o < model->rowCount(); ++o)
    {
        if (model->data(model->index(o, colSudId)).toInt() == id)
        {
            QVariant name = model->data(o, "Name");
            double menge = model->data(o, "erg_Menge").toDouble();
            double preis = bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
            kostenHopfen += preis * menge / 1000;
        }
    }
    summe += kostenHopfen;

    double kostenHefe = 0.0;
    //model = bh->modelHefegaben();
    //colSudId = model->fieldIndex("SudID");
    //for (int o = 0; o < model->rowCount(); ++o)
    {
        //if (model->data(model->index(o, colSudId)).toInt() == id)
        {
            QVariant name = data(row, "AuswahlHefe");//model->data(o, "Name");
            int menge = data(row, "HefeAnzahlEinheiten").toInt();//model->data(o, "Menge").toInt();
            double preis = bh->modelHefe()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
            kostenHefe += preis * menge;
        }
    }
    summe += kostenHefe;

    //Kosten der Weiteren Zutaten
    double kostenWeitereZutaten = 0.0;
    model = bh->modelWeitereZutatenGaben();
    colSudId = model->fieldIndex("SudID");
    for (int o = 0; o < model->rowCount(); ++o)
    {
        if (model->data(model->index(o, colSudId)).toInt() == id)
        {
            QVariant name = model->data(o, "Name");
            double menge = model->data(o, "erg_Menge").toDouble();
            int typ = model->data(o, "Typ").toInt();
            double preis;
            if (typ == EWZ_Typ_Hopfen)
                preis = bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
            else
                preis = bh->modelWeitereZutaten()->getValueFromSameRow("Beschreibung", name, "Preis").toDouble();
            kostenWeitereZutaten += preis * menge / 1000;
        }
    }
    summe += kostenWeitereZutaten;

    double kostenSonstiges = data(row, "KostenWasserStrom").toDouble();
    summe += kostenSonstiges;

    double kostenAnlage = dataAnlage(row, "Kosten").toDouble();
    summe += kostenAnlage;

    if (data(row, "BierWurdeGebraut").toBool())
        setData(row, "erg_Preis", summe / data(row, "erg_AbgefuellteBiermenge").toDouble());
    else
        setData(row, "erg_Preis", summe / data(row, "Menge").toDouble());
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
    if (data(index.row(), "BierWurdeAbgefuellt").toBool())
    {
        QDateTime dt = bh->modelNachgaerverlauf()->getLastDateTime(data(index.row(), "ID").toInt());
        int tageReifung = (int)dt.daysTo(QDateTime::currentDateTime());
        int tageReifungSoll = data(index.row(), "Reifezeit").toInt() * 7;
        return tageReifungSoll - tageReifung;
    }
    return 0;
}

QVariant ModelSud::AbfuellenBereitZutaten(const QModelIndex &index) const
{
    int id = data(index.row(), "ID").toInt();
    SqlTableModel* model = bh->modelWeitereZutatenGaben();
    int colSudId = model->fieldIndex("SudID");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->data(model->index(i, colSudId)).toInt() == id)
        {
            if (!model->data(i, "Abfuellbereit").toBool())
                return false;
        }
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

QVariant ModelSud::SWSollLautern(const QModelIndex &index) const
{
    double swSoll = data(index.row(), "SW").toDouble();
    double hgf = 1 + data(index.row(), "highGravityFaktor").toInt() / 100.0;
    double sw = (swSoll - swWzKochenRecipe[index.row()] - swWzGaerungRecipe[index.row()]) * hgf;
    double menge = data(index.row(), "Menge").toDouble();
    double mengeKochbeginn = data(index.row(), "MengeSollKochbeginn").toDouble();
    return sw * menge / mengeKochbeginn;
}

QVariant ModelSud::SWSollKochbeginn(const QModelIndex &index) const
{
    double sw = data(index.row(), "SWSollKochende").toDouble();
    double menge = data(index.row(), "Menge").toDouble();
    double mengeKochbeginn = data(index.row(), "MengeSollKochbeginn").toDouble();
    return sw * menge / mengeKochbeginn;
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
    double ist = bh->modelWasser()->data(0, "Restalkalitaet").toDouble();
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

int ModelSud::getNextId() const
{
    int maxId = 0;
    int colSudId = fieldIndex("ID");
    for (int i = 0; i < rowCount(); ++i)
    {
        int sudId = index(i, colSudId).data().toInt();
        if (sudId > maxId)
            maxId = sudId;
    }
    return maxId + 1;
}

void ModelSud::defaultValues(QVariantMap &values) const
{
    if (!values.contains("ID"))
        values.insert("ID", getNextId());
    if (!values.contains("Erstellt"))
        values.insert("Erstellt", QDate::currentDate());
    if (!values.contains("Menge"))
        values.insert("Menge", 20);
    if (!values.contains("SW"))
        values.insert("SW", 12);
    if (!values.contains("CO2"))
        values.insert("CO2", 5);
    if (!values.contains("IBU"))
        values.insert("IBU", 26);
    if (!values.contains("BierWurdeGebraut"))
        values.insert("BierWurdeGebraut", 0);
    if (!values.contains("BierWurdeAbgefuellt"))
        values.insert("BierWurdeAbgefuellt", 0);
    if (!values.contains("BierWurdeVerbraucht"))
        values.insert("BierWurdeVerbraucht", 0);
}

QVariantMap ModelSud::copyValues(int row) const
{
    QVariantMap values = SqlTableModel::copyValues(row);
    values.insert("ID", getNextId());
    return values;
}
