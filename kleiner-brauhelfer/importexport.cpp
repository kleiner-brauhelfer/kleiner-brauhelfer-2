#include "importexport.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDomDocument>
#include <QMessageBox>
#include "brauhelfer.h"

extern Brauhelfer* bh;

static void encodeHtml(QString& str)
{
    str.replace("Ä","&#196;");
    str.replace("ä","&#228;");
    str.replace("Ö","&#214;");
    str.replace("ö","&#246;");
    str.replace("Ü","&#220;");
    str.replace("ü","&#252;");
    str.replace("ß","&#223;");
    str.replace("º","&#186;");
    str.replace("°","&#176;");
    str.replace("®","&#174;");
    str.replace("©","&#169;");
}

static int findMax(const QJsonObject& jsn, const QString& str, int MAX = 20)
{
    int i = 1;
    for (; i < MAX; ++i)
    {
        QString search = QString(str).replace("%%", QString::number(i));
        if (!jsn.contains(search))
            break;
    }
    return i;
}

static double toDouble(const QJsonValueRef& value)
{
    QString s;
    switch (value.type())
    {
    case QJsonValue::Double:
        return value.toDouble();
    case QJsonValue::String:
        s = value.toString();
        if (s.startsWith("\""))
            s.truncate(1);
        if (s.endsWith("\""))
            s.chop(1);
        return s.toDouble();
    default:
        return 0.0;
    }
}

bool ImportExport::importMaischeMalzundMehr(const QString &fileName, int *_sudRow)
{
    // https://www.maischemalzundmehr.de/rezept.json.txt
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &jsonError);
    file.close();
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        QMessageBox::warning(nullptr, QObject::tr("Fehler beim lesen der JSON Datei"), jsonError.errorString());
        return false;
    }

    QJsonObject root = doc.object();

    double menge = toDouble(root["Ausschlagswuerze"]);
    int max_schuettung = findMax(root,"Malz%%");
    double gesamt_schuettung = 0.0;
    for (int i = 1; i < max_schuettung; ++i)
    {
        double kg = 0.0;
        if (root[QString("Malz%1_Einheit").arg(i)].toString() == "g")
            kg = toDouble(root[QString("Malz%1_Menge").arg(i)]) / 1000.0;
        else
            kg = toDouble(root[QString("Malz%1_Menge").arg(i)]);
        gesamt_schuettung += kg;
    }

    QMap<int, QVariant> values;
    values[ModelSud::ColSudname] = root["Name"].toString();
    values[ModelSud::ColMenge] = menge;
    values[ModelSud::ColSW] = toDouble(root["Stammwuerze"]);
    values[ModelSud::ColSudhausausbeute] = toDouble(root["Sudhausausbeute"]);
    values[ModelSud::ColFaktorHauptguss] = toDouble(root["Infusion_Hauptguss"]) / gesamt_schuettung;
    values[ModelSud::ColEinmaischenTemp] = toDouble(root["Infusion_Einmaischtemperatur"]);
    values[ModelSud::ColCO2] = toDouble(root["Karbonisierung"]);
    values[ModelSud::ColIBU] = toDouble(root["Bittere"]);
    values[ModelSud::ColberechnungsArtHopfen] = Hopfen_Berechnung_Keine;
    values[ModelSud::ColKochdauerNachBitterhopfung] = toDouble(root["Kochzeit_Wuerze"]);
    values[ModelSud::ColVergaerungsgrad] = toDouble(root["Endvergaerungsgrad"]);
    values[ModelSud::ColKommentar] = QString(QObject::tr("Rezept aus MaischMalzundMehr\n"
                                              "<b>Autor: </b> %1\n"
                                              "<b>Datum: </b> %2\n"
                                              "<b>Sorte: </b> %3\n\n"
                                              "%4\n\n%5"))
                            .arg(root["Autor"].toString())
                            .arg(root["Datum"].toString())
                            .arg(root["Sorte"].toString())
                            .arg(root["Kurzbeschreibung"].toString())
                            .arg(root["Anmerkung_Autor"].toString());

    int sudRow = bh->modelSud()->append(values);
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    // Rasten
    if (root["Maischform"].toString() == "infusion")
    {
        int max_rasten = findMax(root, "Infusion_Rasttemperatur%%");
        for (int i = 1; i < max_rasten; ++i)
        {
            values.clear();
            values[ModelRasten::ColSudID] = sudId;
            values[ModelRasten::ColName] = QString(QObject::tr("%1. Rast")).arg(i);
            values[ModelRasten::ColTemp] = toDouble(root[QString("Infusion_Rasttemperatur%1").arg(i)]);
            values[ModelRasten::ColDauer] = toDouble(root[QString("Infusion_Rastzeit%1").arg(i)]);
            bh->modelRasten()->append(values);
        }
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColName] = QObject::tr("Abmaischen");
        values[ModelRasten::ColTemp] = toDouble(root["Abmaischtemperatur"]);
        values[ModelRasten::ColDauer] = 10;
        bh->modelRasten()->append(values);
    }

    // Malzschuettung
    for (int i = 1; i < max_schuettung; ++i)
    {
        double kg = 0.0;
        if (root[QString("Malz%1_Einheit").arg(i)].toString() == "g")
            kg = toDouble(root[QString("Malz%1_Menge").arg(i)]) / 1000.0;
        else
            kg = toDouble( root[QString("Malz%1_Menge").arg(i)]);
        values.clear();
        values[ModelMalzschuettung::ColSudID] = sudId;
        values[ModelMalzschuettung::ColName] = root[QString("Malz%1").arg(i)].toString();
        values[ModelMalzschuettung::ColProzent] = kg / gesamt_schuettung * 100.0;
        values[ModelMalzschuettung::ColFarbe] = 1;
        bh->modelMalzschuettung()->append(values);
    }

    // Hopfen
    for (int i = 1; i < findMax(root, "Hopfen_VWH_%%_Sorte"); ++i)
    {
        values.clear();
        values[ModelHopfengaben::ColSudID] = sudId;
        values[ModelHopfengaben::ColName] = root[QString("Hopfen_VWH_%1_Sorte").arg(i)].toString();
        values[ModelHopfengaben::Colerg_Menge] = toDouble(root[QString("Hopfen_VWH_%1_Menge").arg(i)]);
        values[ModelHopfengaben::ColZeit] = toDouble(root["Kochzeit_Wuerze"]);
        values[ModelHopfengaben::ColAlpha] = toDouble(root[QString("Hopfen_VWH_%1_alpha").arg(i)]);
        values[ModelHopfengaben::ColPellets] = 1;
        values[ModelHopfengaben::ColVorderwuerze] = 1;
        bh->modelHopfengaben()->append(values);
    }
    for (int i = 1; i < findMax(root, "Hopfen_%%_Sorte"); ++i)
    {
        values.clear();
        values[ModelHopfengaben::ColSudID] = sudId;
        values[ModelHopfengaben::ColName] = root[QString("Hopfen_%1_Sorte").arg(i)].toString();
        values[ModelHopfengaben::Colerg_Menge] = toDouble(root[QString("Hopfen_%1_Menge").arg(i)]);
        values[ModelHopfengaben::ColZeit] = toDouble(root[QString("Hopfen_%1_Kochzeit").arg(i)]);
        values[ModelHopfengaben::ColAlpha] = toDouble(root[QString("Hopfen_%1_alpha").arg(i)]);
        values[ModelHopfengaben::ColPellets] = 1;
        bh->modelHopfengaben()->append(values);
    }

    // Hefe
    values.clear();
    values[ModelHefegaben::ColSudID] = sudId;
    values[ModelHefegaben::ColName] = root["Hefe"].toString();
    values[ModelHefegaben::ColMenge] = (int)(menge / 20.0);
    bh->modelHefegaben()->append(values);

    // Weitere Zutaten
    int max_wz_wuerze = findMax(root, "WeitereZutat_Wuerze_%%_Name");
    for (int i = 1; i < max_wz_wuerze; ++i)
    {
        values.clear();
        values[ModelWeitereZutatenGaben::ColSudID] = sudId;
        values[ModelWeitereZutatenGaben::ColName] = root[QString("WeitereZutat_Wuerze_%1_Name").arg(i)].toString();
        values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Menge").arg(i)]) / menge;
        if (root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(i)].toString() == "g")
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_g;
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_Kg;
        values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Sonstiges;
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Kochen;
        values[ModelWeitereZutatenGaben::ColZugabedauer] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(i)]);
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_KeineEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    int max_wz_gaerung = findMax(root, "WeitereZutat_Gaerung_%%_Name");
    for (int i = 1; i < max_wz_gaerung; ++i)
    {
        values.clear();
        values[ModelWeitereZutatenGaben::ColSudID] = sudId;
        values[ModelWeitereZutatenGaben::ColName] = root[QString("WeitereZutat_Gaerung_%1_Name").arg(i)].toString();
        values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QString("WeitereZutat_Gaerung_%1_Menge").arg(i)]) / menge;
        if (root[QString("WeitereZutat_Gaerung_%1_Einheit").arg(i)].toString() == "g")
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_g;
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_Kg;
        values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Sonstiges;
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Gaerung;
        values[ModelWeitereZutatenGaben::ColZugabedauer] = 0;
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_KeineEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    int max_hopfen_stopf = findMax(root, "Stopfhopfen_%%_Sorte");
    for (int i = 1; i < max_hopfen_stopf; ++i)
    {
        values.clear();
        values[ModelWeitereZutatenGaben::ColSudID] = sudId;
        values[ModelWeitereZutatenGaben::ColName] = root[QString("Stopfhopfen_%1_Sorte").arg(i)].toString();
        values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QString("Stopfhopfen_%1_Menge").arg(i)]) / menge;
        if (root[QString("Stopfhopfen_%1_Einheit").arg(i)].toString() == "g")
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_g;
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_Kg;
        values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Hopfen;
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Gaerung;
        values[ModelWeitereZutatenGaben::ColZugabedauer] = 7200;
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_MitEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    if (_sudRow)
        *_sudRow = sudRow;
    return true;
}

bool ImportExport::importBeerXml(const QString &fileName, int* _sudRow)
{
    int sudRow = -1;
    const QString BeerXmlVersion = "1";

    // http://www.beerxml.com/
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QString xmlError;
    QDomDocument doc("");
    doc.setContent(&file, &xmlError);
    file.close();
    if (!xmlError.isEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Fehler beim lesen der XML Datei"), xmlError);
        return false;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "RECIPES")
        return false;

    for(QDomNode nRecipe = root.firstChildElement("RECIPE"); !nRecipe.isNull(); nRecipe = nRecipe.nextSiblingElement("RECIPE"))
    {
        if (nRecipe.firstChildElement("VERSION").text() != BeerXmlVersion)
            continue;

        QDomNode nStyle = nRecipe.firstChildElement("STYLE");
        QDomNode nMash = nRecipe.firstChildElement("MASH");
        QDomNode nMashSteps = nMash.firstChildElement("MASH_STEPS");

        double min, max;
        double menge = nRecipe.firstChildElement("BATCH_SIZE").text().toDouble();

        QMap<int, QVariant> values;
        values[ModelSud::ColSudname] = nRecipe.firstChildElement("NAME").text();
        values[ModelSud::ColMenge] = menge;
        min = nStyle.firstChildElement("OG_MIN").text().toDouble();
        max = nStyle.firstChildElement("OG_MAX").text().toDouble();
        values[ModelSud::ColSW] = BierCalc::dichteToPlato((min+max)/2);
        values[ModelSud::ColFaktorHauptguss] = 3.5;
        values[ModelSud::ColEinmaischenTemp] = nMashSteps.firstChildElement("MASH_STEP").firstChildElement("STEP_TEMP").text().toDouble();
        min = nStyle.firstChildElement("CARB_MIN").text().toDouble();
        max = nStyle.firstChildElement("CARB_MAX").text().toDouble();
        values[ModelSud::ColCO2] = (min+max)/2;
        min = nStyle.firstChildElement("IBU_MIN").text().toDouble();
        max = nStyle.firstChildElement("IBU_MAX").text().toDouble();
        values[ModelSud::ColIBU] = (min+max)/2;
        values[ModelSud::ColberechnungsArtHopfen] = Hopfen_Berechnung_Keine;
        values[ModelSud::ColKochdauerNachBitterhopfung] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();

        sudRow = bh->modelSud()->append(values);
        int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

        // Rasten
        for(QDomNode n = nMashSteps.firstChildElement("MASH_STEP"); !n.isNull(); n = n.nextSiblingElement("MASH_STEP"))
        {
            values.clear();
            values[ModelRasten::ColSudID] = sudId;
            values[ModelRasten::ColName] = n.firstChildElement("NAME").text();
            values[ModelRasten::ColTemp] = n.firstChildElement("STEP_TEMP").text().toDouble();
            values[ModelRasten::ColDauer] = n.firstChildElement("STEP_TIME").text().toDouble();
            bh->modelRasten()->append(values);
        }

        // Malzschuettung
        QDomNode nMalz = nRecipe.firstChildElement("FERMENTABLES");
        double gesamt_malz = 0.0;
        for(QDomNode n = nMalz.firstChildElement("FERMENTABLE"); !n.isNull(); n = n.nextSiblingElement("FERMENTABLE"))
        {
            if (n.firstChildElement("TYPE").text() == "Grain")
                gesamt_malz += n.firstChildElement("AMOUNT").text().toDouble();
        }
        for(QDomNode n = nMalz.firstChildElement("FERMENTABLE"); !n.isNull(); n = n.nextSiblingElement("FERMENTABLE"))
        {
            QString type = n.firstChildElement("TYPE").text();
            if (type == "Grain")
            {
                values.clear();
                values[ModelMalzschuettung::ColSudID] = sudId;
                values[ModelMalzschuettung::ColName] = n.firstChildElement("NAME").text();
                values[ModelMalzschuettung::ColFarbe] = n.firstChildElement("COLOR").text().toDouble() * 1.97;
                values[ModelMalzschuettung::ColProzent] = n.firstChildElement("AMOUNT").text().toDouble() / gesamt_malz * 100;
                bh->modelMalzschuettung()->append(values);
            }
            else
            {
                values.clear();
                values[ModelWeitereZutatenGaben::ColSudID] = sudId;
                values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement("NAME").text();
                values[ModelWeitereZutatenGaben::ColFarbe] = n.firstChildElement("COLOR").text().toDouble() * 1.97;
                values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement("AMOUNT").text().toDouble() / menge;
                values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_Kg;
                if (type == "Sugar" || type == "Extract" || type == "Dry Extract")
                    values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Zucker;
                else
                    values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Sonstiges;
                if (n.firstChildElement("ADD_AFTER_BOIL").text() == "TRUE")
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Gaerung;
                }
                else
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Kochen;
                    values[ModelWeitereZutatenGaben::ColZugabedauer] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();
                }
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_KeineEntnahme;
                values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement("NOTES").text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
        }

        // Hopfen
        QDomNode nHops = nRecipe.firstChildElement("HOPS");
        for(QDomNode n = nHops.firstChildElement("HOP"); !n.isNull(); n = n.nextSiblingElement("HOP"))
        {
            QString use = n.firstChildElement("USE").text();
            if (use == "Dry Hop")
            {
                values.clear();
                values[ModelWeitereZutatenGaben::ColSudID] = sudId;
                values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement("NAME").text();
                values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement("AMOUNT").text().toDouble() * 1000 / menge;
                values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_g;
                values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Hopfen;
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Gaerung;
                values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement("TIME").text().toDouble();
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_MitEntnahme;
                values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement("NOTES").text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
            else
            {
                values.clear();
                values[ModelHopfengaben::ColSudID] = sudId;
                values[ModelHopfengaben::ColName] = n.firstChildElement("NAME").text();
                values[ModelHopfengaben::Colerg_Menge] = n.firstChildElement("AMOUNT").text().toDouble();
                values[ModelHopfengaben::ColZeit] = n.firstChildElement("TIME").text().toDouble();
                values[ModelHopfengaben::ColAlpha] = n.firstChildElement("ALPHA").text().toDouble();
                values[ModelHopfengaben::ColPellets] = n.firstChildElement("FORM").text() == "Pellet";
                values[ModelHopfengaben::ColVorderwuerze] = use == "First Wort";
                bh->modelHopfengaben()->append(values);
            }
        }

        // Hefe
        QDomNode nYeasts = nRecipe.firstChildElement("YEASTS");
        for(QDomNode n = nYeasts.firstChildElement("YEAST"); !n.isNull(); n = n.nextSiblingElement("YEAST"))
        {
            values.clear();
            values[ModelHefegaben::ColSudID] = sudId;
            values[ModelHefegaben::ColName] = n.firstChildElement("NAME").text();
            values[ModelHefegaben::ColMenge] = 0;
            bh->modelHefegaben()->append(values);
        }

        QDomNode nMiscs = nRecipe.firstChildElement("MISCS");
        for(QDomNode n = nMiscs.firstChildElement("MISC"); !n.isNull(); n = n.nextSiblingElement("MISC"))
        {
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement("NAME").text();
            QString type = n.firstChildElement("TYPE").text();
            if (type == "Spice" || type == "Herb")
                values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Gewuerz;
            else if (type == "Flavor")
                values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Frucht;
            else
                values[ModelWeitereZutatenGaben::ColTyp] = EWZ_Typ_Sonstiges;
            QString use = n.firstChildElement("USE").text();
            if (use == "Boil")
               values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Kochen;
            else if (use == "Mash")
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Maischen;
            else
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = EWZ_Zeitpunkt_Gaerung;
            values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement("AMOUNT").text().toDouble() * 1000 / menge;
            values[ModelWeitereZutatenGaben::ColEinheit] = EWZ_Einheit_g;
            values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement("TIME").text().toDouble();
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = EWZ_Entnahmeindex_KeineEntnahme;
            values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement("NOTES").text();
            bh->modelWeitereZutatenGaben()->append(values);
        }
    }
    if (_sudRow)
        *_sudRow = sudRow;
    return true;
}

bool ImportExport::exportMaischeMalzundMehr(const QString &fileName, int sudRow)
{
    ProxyModel model;
    int n;
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    QJsonObject root;
    root["Name"] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
    root["Datum"] = bh->modelSud()->data(sudRow, ModelSud::ColErstellt).toDate().toString("dd.MM.yyyy");
    root["Ausschlagswuerze"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1);
    root["Sudhausausbeute"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1);
    root["Stammwuerze"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble(), 'f', 1);
    root["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIBU).toInt());
    root["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toInt());
    root["Alkohol"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColAlkohol).toDouble(), 'f', 1);
    root["Kurzbeschreibung"] = bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString();
    root["Infusion_Hauptguss"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble(), 'f', 1);
    root["Endvergaerungsgrad"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toDouble(), 'f', 1);
    root["Karbonisierung"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
    root["Nachguss"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WNachguss).toDouble(), 'f', 1);
    root["Kochzeit_Wuerze"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauerNachBitterhopfung).toInt());

    // Rasten
    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(ModelRasten::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QString("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
        root[QString("Infusion_Rastzeit%1").arg(n)] = QString::number(model.data(row, ModelRasten::ColDauer).toInt());
        ++n;
    }
    root["Infusion_Einmaischtemperatur"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColEinmaischenTemp).toInt());
    root["Abmaischtemperatur"] = "78";

    // Malzschuettung
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QString("Malz%1").arg(n)] = model.data(row, ModelMalzschuettung::ColName).toString();
        root[QString("Malz%1_Menge").arg(n)] = QString::number(model.data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2);
        root[QString("Malz%1_Einheit").arg(n)] = "kg";
        ++n;
    }
    root["Maischform"] = "infusion";

    // Hopfen
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (model.data(row, ModelHopfengaben::ColVorderwuerze).toBool())
        {
            root[QString("Hopfen_VWH_%1_Sorte").arg(n)] = model.data(row, ModelHopfengaben::ColName).toString();
            root[QString("Hopfen_VWH_%1_Menge").arg(n)] = QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1);
            root[QString("Hopfen_VWH_%1_alpha").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1);
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (!model.data(row, ModelHopfengaben::ColVorderwuerze).toBool())
        {
            root[QString("Hopfen_%1_Sorte").arg(n)] = model.data(row, ModelHopfengaben::ColName).toString();
            root[QString("Hopfen_%1_Menge").arg(n)] = QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1);
            root[QString("Hopfen_%1_alpha").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1);
            root[QString("Hopfen_%1_Kochzeit").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColZeit).toInt());
            ++n;
        }
    }

    // Hefe
    QStringList hefen;
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(ModelHefegaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        QString hefe = model.data(row, ModelHefegaben::ColName).toString();
        if (!hefen.contains(hefe))
            hefen.append(hefe);
    }
    root["Hefe"] = hefen.join(", ");

    // Weitere Zutaten
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Maischen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            if (model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
            }
            root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauerNachBitterhopfung).toInt());
            ++n;
        }
        else if (zeitpunkt == EWZ_Zeitpunkt_Kochen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            if (model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
                root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt());
            }
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt();
        int typ = model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung && typ == EWZ_Typ_Hopfen)
        {
            root[QString("Stopfhopfen_%1_Sorte").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            root[QString("Stopfhopfen_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt();
        int typ = model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung && typ != EWZ_Typ_Hopfen)
        {
            root[QString("WeitereZutat_Gaerung_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            if (model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Gaerung_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QString("WeitereZutat_Gaerung_%1_Einheit").arg(n)] = "g";
            }
            ++n;
        }
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return false;
    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool ImportExport::exportBeerXml(const QString &fileName, int sudRow)
{
    const QString BeerXmlVersion = "1";

    double val;
    QString str;
    ProxyModel model;
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();
    bool gebraut = bh->modelSud()->data(sudRow, ModelSud::ColStatus).toInt() != Sud_Status_Rezept;

    QDomDocument doc("");
    QDomText text;
    QDomElement element;
    QDomElement Anteil;

    QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"ISO-8859-1\"" );
    doc.appendChild(header);

    QDomElement Rezepte = doc.createElement("RECIPES");
    doc.appendChild(Rezepte);

    QDomElement Rezept = doc.createElement("RECIPE");
    Rezepte.appendChild(Rezept);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString());
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("TYPE");
    text = doc.createTextNode("All Grain");
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BREWER");
    text = doc.createTextNode("kleiner-brauhelfer-2");
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BATCH_SIZE");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BOIL_SIZE");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BOIL_TIME");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauerNachBitterhopfung).toInt()));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("EFFICIENCY");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, gebraut ? ModelSud::Colerg_EffektiveAusbeute : ModelSud::ColAnlageSudhausausbeute).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    QDomElement style = doc.createElement("STYLE");
    Rezept.appendChild(style);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode("Unbekannt");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("CATEGORY");
    text = doc.createTextNode("Unbekannt");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("CATEGORY_NUMBER");
    text = doc.createTextNode("0");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("STYLE_LETTER");
    text = doc.createTextNode("");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("STYLE_GUIDE");
    text = doc.createTextNode("");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("TYPE");
    text = doc.createTextNode("");
    element.appendChild(text);
    style.appendChild(element);

    val = BierCalc::platoToDichte(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble());
    element = doc.createElement("OG_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("OG_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    val *= 0.8;
    element = doc.createElement("FG_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("FG_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::ColIBU).toDouble();
    element = doc.createElement("IBU_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("IBU_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toDouble() * 0.508;
    element = doc.createElement("COLOR_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("COLOR_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble();
    element = doc.createElement("CARB_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 1));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("CARB_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 1));
    element.appendChild(text);
    style.appendChild(element);

    QDomElement Hopfengaben = doc.createElement("HOPS");
    Rezept.appendChild(Hopfengaben);
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("HOP");
        Hopfengaben.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelHopfengaben::ColName).toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("ALPHA");
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble() / 1000, 'f', 4));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("USE");
        text = doc.createTextNode(model.data(row, ModelHopfengaben::ColVorderwuerze).toBool() ? "First Wort" : "Boil");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TIME");
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::ColZeit).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        int typ = Hopfen_Universal;
        int rowHopfen = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColBeschreibung, model.data(row, ModelHopfengaben::ColName));
        if (rowHopfen >= 0)
        {
             typ = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColTyp).toInt();
        }

        element = doc.createElement("TYPE");
        switch (typ)
        {
        case Hopfen_Aroma:
            text = doc.createTextNode("Aroma");
            break;
        case Hopfen_Bitter:
            text = doc.createTextNode("Bittering");
            break;
        case Hopfen_Unbekannt:
        case Hopfen_Universal:
        default:
            text = doc.createTextNode("Both");
            break;
        }
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("FORM");
        text = doc.createTextNode(model.data(row, ModelHopfen::ColPellets).toBool() ? "Pellet" : "Leaf");
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen)
        {
            Anteil = doc.createElement("HOP");
            Hopfengaben.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("USE");
            text = doc.createTextNode("Dry Hop");
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TIME");
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str.toHtmlEscaped());
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            double alpha = 0.0;
            bool pellets = false;
            int typ = Hopfen_Universal;
            int rowHopfen = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColBeschreibung, model.data(row, ModelWeitereZutatenGaben::ColName));
            if (rowHopfen >= 0)
            {
                alpha = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColAlpha).toDouble();
                pellets = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColPellets).toBool();
                typ = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColTyp).toInt();
            }

            element = doc.createElement("ALPHA");
            text = doc.createTextNode(QString::number(alpha, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (typ)
            {
            case Hopfen_Aroma:
                text = doc.createTextNode("Aroma");
                break;
            case Hopfen_Bitter:
                text = doc.createTextNode("Bittering");
                break;
            case Hopfen_Unbekannt:
            case Hopfen_Universal:
            default:
                text = doc.createTextNode("Both");
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("FORM");
            text = doc.createTextNode(pellets ? "Pellet" : "Leaf");
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QDomElement fermentables = doc.createElement("FERMENTABLES");
    Rezept.appendChild(fermentables);
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("FERMENTABLE");
        fermentables.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelMalzschuettung::ColName).toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Grain");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode(QString::number(model.data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("YIELD");
        text = doc.createTextNode("100");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("COLOR");
        double ebc = model.data(row, ModelMalzschuettung::ColFarbe).toDouble();
        double srm = ebc * 0.508;
        double l = (srm + 0.76) / 1.3546;
        text = doc.createTextNode(QString::number(l));
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        double Ausbeute = model.data(row, ModelWeitereZutatenGaben::ColAusbeute).toDouble();
        if (Ausbeute > 0)
        {
            Anteil = doc.createElement("FERMENTABLE");
            fermentables.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt())
            {
            case EWZ_Typ_Honig:
            case EWZ_Typ_Zucker:
            case EWZ_Typ_Frucht:
                text = doc.createTextNode("Sugar");
                break;
            case EWZ_Typ_Gewuerz:
            case EWZ_Typ_Sonstiges:
            default:
                text = doc.createTextNode("Adjunct");
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("YIELD");
            text = doc.createTextNode(QString::number(Ausbeute, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("COLOR");
            double ebc = model.data(row, ModelWeitereZutatenGaben::ColFarbe).toDouble();
            double srm = ebc * 0.508;
            double l = (srm + 0.76) / 1.3546;
            text = doc.createTextNode(QString::number(l));
            element.appendChild(text);
            Anteil.appendChild(element);

            if (model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt() == EWZ_Zeitpunkt_Gaerung)
            {
                element = doc.createElement("ADD_AFTER_BOIL");
                text = doc.createTextNode("TRUE");
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str.toHtmlEscaped());
                element.appendChild(text);
                Anteil.appendChild(element);
            }
        }
    }

    QDomElement yeasts = doc.createElement("YEASTS");
    Rezept.appendChild(yeasts);
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(ModelHefegaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("YEAST");
        yeasts.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelHefegaben::ColName).toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Lager");
        element.appendChild(text);
        Anteil.appendChild(element);

        bool liquid = false;
        int rowHefe = bh->modelHefe()->getRowWithValue(ModelHefe::ColBeschreibung, model.data(row, ModelHefegaben::ColName));
        if (rowHefe >= 0)
        {
            liquid = bh->modelHefe()->data(rowHefe, ModelHefe::ColTypTrFl).toInt() == Hefe_Fluessig;
        }

        element = doc.createElement("FORM");
        text = doc.createTextNode(liquid ? "Liquid" : "Dry");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode("0");
        element.appendChild(text);
        Anteil.appendChild(element);

        if (model.data(row, ModelHefegaben::ColZugabeNach).toInt() > 0)
        {
            element = doc.createElement("ADD_TO_SECONDARY");
            text = doc.createTextNode("TRUE");
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QDomElement miscs = doc.createElement("MISCS");
    Rezept.appendChild(miscs);
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int typ = model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt();
        double Ausbeute = model.data(row, ModelWeitereZutatenGaben::ColAusbeute).toDouble();
        if (typ != EWZ_Typ_Hopfen && Ausbeute == 0)
        {
            Anteil = doc.createElement("MISC");
            miscs.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (typ)
            {
            case EWZ_Typ_Honig:
            case EWZ_Typ_Zucker:
                text = doc.createTextNode("Sugar");
                break;
            case EWZ_Typ_Frucht:
                text = doc.createTextNode("Flavor");
                break;
            case EWZ_Typ_Gewuerz:
                text = doc.createTextNode("Spice");
                break;
            case EWZ_Typ_Sonstiges:
            default:
                text = doc.createTextNode("Other");
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("USE");
            switch (model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt())
            {
            case EWZ_Zeitpunkt_Gaerung:
                text = doc.createTextNode("Primary");
                break;
            case EWZ_Zeitpunkt_Kochen:
                text = doc.createTextNode("Boil");
                break;
            case EWZ_Zeitpunkt_Maischen:
                text = doc.createTextNode("Mash");
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TIME");
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT_IS_WEIGHT");
            text = doc.createTextNode("TRUE");
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str.toHtmlEscaped());
                element.appendChild(text);
                Anteil.appendChild(element);
            }
        }
    }

    QDomElement waters = doc.createElement("WATERS");
    Rezept.appendChild(waters);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColAnlage).toString());
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("AMOUNT");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_W_Gesamt).toDouble(), 'f', 1));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("CALCIUM");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("BICARBONATE");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("SULFATE");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("CHLORIDE");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("SODIUM");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("MAGNESIUM");
    text = doc.createTextNode("0.0");
    element.appendChild(text);
    waters.appendChild(element);

    QDomElement mash = doc.createElement("MASH");
    Rezept.appendChild(mash);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    mash.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode("Temperatur");
    element.appendChild(text);
    mash.appendChild(element);

    element = doc.createElement("GRAIN_TEMP");
    text = doc.createTextNode("18");
    element.appendChild(text);
    mash.appendChild(element);

    QDomElement mash_steps = doc.createElement("MASH_STEPS");
    mash.appendChild(mash_steps);

    Anteil = doc.createElement("MASH_STEP");
    mash_steps.appendChild(Anteil);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    Anteil.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode("Einmaischen");
    element.appendChild(text);
    Anteil.appendChild(element);

    element = doc.createElement("TYPE");
    text = doc.createTextNode("Temperature");
    element.appendChild(text);
    Anteil.appendChild(element);

    element = doc.createElement("STEP_TEMP");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColEinmaischenTemp).toInt()));
    element.appendChild(text);
    Anteil.appendChild(element);

    element = doc.createElement("STEP_TIME");
    text = doc.createTextNode("10");
    element.appendChild(text);
    Anteil.appendChild(element);

    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(ModelRasten::ColSudID);
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("MASH_STEP");
        mash_steps.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelRasten::ColName).toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Temperature");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("STEP_TEMP");
        text = doc.createTextNode(QString::number(model.data(row, ModelRasten::ColTemp).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("STEP_TIME");
        text = doc.createTextNode(QString::number(model.data(row, ModelRasten::ColDauer).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadWrite | QFile::Text))
      return false;
    QTextStream out(&file);
    out.setCodec("ISO 8859-1");

    QString strXml;
    QTextStream xml(&strXml);
    doc.save(xml, 2);
    encodeHtml(strXml);
    out << strXml;
    file.close();
    return true;
}
