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
            kg = toDouble( root[QString("Malz%1_Menge").arg(i)]);
        gesamt_schuettung += kg;
    }

    QVariantMap values;
    values["Sudname"] = root["Name"].toString();
    values["Menge"] = menge;
    values["SW"] = toDouble(root["Stammwuerze"]);
    values["FaktorHauptguss"] = toDouble(root["Infusion_Hauptguss"]) / gesamt_schuettung;
    values["EinmaischenTemp"] = toDouble(root["Infusion_Einmaischtemperatur"]);
    values["CO2"] = toDouble(root["Karbonisierung"]);
    values["IBU"] = toDouble(root["Bittere"]);
    values["berechnungsArtHopfen"] = Hopfen_Berechnung_Gewicht;
    values["KochdauerNachBitterhopfung"] = toDouble(root["Kochzeit_Wuerze"]);
    values["Kommentar"] = QString(QObject::tr("Rezept aus MaischMalzundMehr\n"
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
    int sudId = bh->modelSud()->data(sudRow, "ID").toInt();

    // Rasten
    if (root["Maischform"].toString() == "infusion")
    {
        int max_rasten = findMax(root, "Infusion_Rasttemperatur%%");
        for (int i = 1; i < max_rasten; ++i)
        {
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = QString(QObject::tr("%1. Rast")).arg(i);
            values["Temp"] = toDouble(root[QString("Infusion_Rasttemperatur%1").arg(i)]);
            values["Dauer"] = toDouble(root[QString("Infusion_Rastzeit%1").arg(i)]);
            bh->modelRasten()->append(values);
        }
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = QObject::tr("Abmaischen");
        values["Temp"] = toDouble(root["Abmaischtemperatur"]);
        values["Dauer"] = 10;
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
        values["SudID"] = sudId;
        values["Name"] = root[QString("Malz%1").arg(i)].toString();
        values["Prozent"] = kg / gesamt_schuettung * 100.0;
        values["Farbe"] = 1;
        bh->modelMalzschuettung()->append(values);
    }

    // Hopfen
    double gesamt_hopfen = 0.0;
    int max_hopfen_vwh = findMax(root, "Hopfen_VWH_%%_Sorte");
    for (int i = 1; i < max_hopfen_vwh; ++i)
        gesamt_hopfen += toDouble(root[QString("Hopfen_VWH_%1_Menge").arg(i)]);
    int max_hopfen = findMax(root, "Hopfen_%%_Sorte");
    for (int i = 1; i < max_hopfen; ++i)
        gesamt_hopfen += toDouble(root[QString("Hopfen_%1_Menge").arg(i)]);
    for (int i = 1; i < max_hopfen_vwh; ++i)
    {
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = root[QString("Hopfen_VWH_%1_Sorte").arg(i)].toString();
        values["Prozent"] = toDouble(root[QString("Hopfen_VWH_%1_Menge").arg(i)]) / gesamt_hopfen * 100.0;
        values["Zeit"] = toDouble(root["Kochzeit_Wuerze"]);
        values["Alpha"] = toDouble(root[QString("Hopfen_VWH_%1_alpha").arg(i)]);
        values["Pellets"] = 1;
        values["Vorderwuerze"] = 1;
        bh->modelHopfengaben()->append(values);
    }
    for (int i = 1; i < max_hopfen; ++i)
    {
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = root[QString("Hopfen_%1_Sorte").arg(i)].toString();
        values["Prozent"] = toDouble(root[QString("Hopfen_%1_Menge").arg(i)]) / gesamt_hopfen * 100.0;
        values["Zeit"] = toDouble(root[QString("Hopfen_%1_Kochzeit").arg(i)]);
        values["Alpha"] = toDouble(root[QString("Hopfen_%1_alpha").arg(i)]);
        values["Pellets"] = 1;
        bh->modelHopfengaben()->append(values);
    }

    // Hefe
    values.clear();
    values["SudID"] = sudId;
    values["Name"] = root["Hefe"].toString();
    values["Menge"] = (int)(menge / 20.0);
    bh->modelHefegaben()->append(values);

    // Weitere Zutaten
    int max_wz_wuerze = findMax(root, "WeitereZutat_Wuerze_%%_Name");
    for (int i = 1; i < max_wz_wuerze; ++i)
    {
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = root[QString("WeitereZutat_Wuerze_%1_Name").arg(i)].toString();
        values["Menge"] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Menge").arg(i)]) / menge;
        if (root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(i)].toString() == "g")
            values["Einheit"] = EWZ_Einheit_g;
        else
            values["Einheit"] = EWZ_Einheit_Kg;
        values["Typ"] = EWZ_Typ_Sonstiges;
        values["Zeitpunkt"] = EWZ_Zeitpunkt_Kochen;
        values["Zugabedauer"] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(i)]);
        values["Entnahmeindex"] = EWZ_Entnahmeindex_KeineEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    int max_wz_gaerung = findMax(root, "WeitereZutat_Gaerung_%%_Name");
    for (int i = 1; i < max_wz_gaerung; ++i)
    {
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = root[QString("WeitereZutat_Gaerung_%1_Name").arg(i)].toString();
        values["Menge"] = toDouble(root[QString("WeitereZutat_Gaerung_%1_Menge").arg(i)]) / menge;
        if (root[QString("WeitereZutat_Gaerung_%1_Einheit").arg(i)].toString() == "g")
            values["Einheit"] = EWZ_Einheit_g;
        else
            values["Einheit"] = EWZ_Einheit_Kg;
        values["Typ"] = EWZ_Typ_Sonstiges;
        values["Zeitpunkt"] = EWZ_Zeitpunkt_Gaerung;
        values["Zugabedauer"] = 0;
        values["Entnahmeindex"] = EWZ_Entnahmeindex_KeineEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    int max_hopfen_stopf = findMax(root, "Stopfhopfen_%%_Sorte");
    for (int i = 1; i < max_hopfen_stopf; ++i)
    {
        values.clear();
        values["SudID"] = sudId;
        values["Name"] = root[QString("Stopfhopfen_%1_Sorte").arg(i)].toString();
        values["Menge"] = toDouble(root[QString("Stopfhopfen_%1_Menge").arg(i)]) / menge;
        if (root[QString("Stopfhopfen_%1_Einheit").arg(i)].toString() == "g")
            values["Einheit"] = EWZ_Einheit_g;
        else
            values["Einheit"] = EWZ_Einheit_Kg;
        values["Typ"] = EWZ_Typ_Hopfen;
        values["Zeitpunkt"] = EWZ_Zeitpunkt_Gaerung;
        values["Zugabedauer"] = 7200;
        values["Entnahmeindex"] = EWZ_Entnahmeindex_MitEntnahme;
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

        QVariantMap values;
        values["Sudname"] = nRecipe.firstChildElement("NAME").text();
        values["Menge"] = menge;
        min = nStyle.firstChildElement("OG_MIN").text().toDouble();
        max = nStyle.firstChildElement("OG_MAX").text().toDouble();
        values["SW"] = BierCalc::dichteToPlato((min+max)/2);
        values["FaktorHauptguss"] = 3.5;
        values["EinmaischenTemp"] = nMashSteps.firstChildElement("MASH_STEP").firstChildElement("STEP_TEMP").text().toDouble();
        min = nStyle.firstChildElement("CARB_MIN").text().toDouble();
        max = nStyle.firstChildElement("CARB_MAX").text().toDouble();
        values["CO2"] = (min+max)/2;
        min = nStyle.firstChildElement("IBU_MIN").text().toDouble();
        max = nStyle.firstChildElement("IBU_MAX").text().toDouble();
        values["IBU"] = (min+max)/2;
        values["berechnungsArtHopfen"] = Hopfen_Berechnung_Gewicht;
        values["KochdauerNachBitterhopfung"] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();

        sudRow = bh->modelSud()->append(values);
        int sudId = bh->modelSud()->data(sudRow, "ID").toInt();

        // Rasten
        for(QDomNode n = nMashSteps.firstChildElement("MASH_STEP"); !n.isNull(); n = n.nextSiblingElement("MASH_STEP"))
        {
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = n.firstChildElement("NAME").text();
            values["Temp"] = n.firstChildElement("STEP_TEMP").text().toDouble();
            values["Dauer"] = n.firstChildElement("STEP_TIME").text().toDouble();
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
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = n.firstChildElement("NAME").text();
            values["Farbe"] = n.firstChildElement("COLOR").text().toDouble() * 1.97;
            QString type = n.firstChildElement("TYPE").text();
            if (type == "Grain")
            {
                values["Prozent"] = n.firstChildElement("AMOUNT").text().toDouble() / gesamt_malz * 100;
                bh->modelMalzschuettung()->append(values);
            }
            else
            {
                values["Menge"] = n.firstChildElement("AMOUNT").text().toDouble() / menge;
                values["Einheit"] = EWZ_Einheit_Kg;
                if (type == "Sugar" || type == "Extract" || type == "Dry Extract")
                    values["Typ"] = EWZ_Typ_Zucker;
                else
                    values["Typ"] = EWZ_Typ_Sonstiges;

                if (n.firstChildElement("ADD_AFTER_BOIL").text() == "TRUE")
                {
                    values["Zeitpunkt"] = EWZ_Zeitpunkt_Gaerung;
                }
                else
                {
                    values["Zeitpunkt"] = EWZ_Zeitpunkt_Kochen;
                    values["Zugabedauer"] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();
                }
                values["Entnahmeindex"] = EWZ_Entnahmeindex_KeineEntnahme;
                values["Bemerkung"] = n.firstChildElement("NOTES").text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
        }

        // Hopfen
        QDomNode nHops = nRecipe.firstChildElement("HOPS");
        double gesamt_hopfen = 0.0;
        for(QDomNode n = nHops.firstChildElement("HOP"); !n.isNull(); n = n.nextSiblingElement("HOP"))
        {
            if (n.firstChildElement("USE").text() != "Dry Hop")
                gesamt_hopfen += n.firstChildElement("AMOUNT").text().toDouble();
        }
        for(QDomNode n = nHops.firstChildElement("HOP"); !n.isNull(); n = n.nextSiblingElement("HOP"))
        {
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = n.firstChildElement("NAME").text();
            QString use = n.firstChildElement("USE").text();
            if (use == "Dry Hop")
            {
                values["Menge"] = n.firstChildElement("AMOUNT").text().toDouble() * 1000 / menge;
                values["Einheit"] = EWZ_Einheit_g;
                values["Typ"] = EWZ_Typ_Hopfen;
                values["Zeitpunkt"] = EWZ_Zeitpunkt_Gaerung;
                values["Zugabedauer"] = n.firstChildElement("TIME").text().toDouble();
                values["Entnahmeindex"] = EWZ_Entnahmeindex_MitEntnahme;
                values["Bemerkung"] = n.firstChildElement("NOTES").text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
            else
            {
                values["Prozent"] = n.firstChildElement("AMOUNT").text().toDouble() / gesamt_hopfen * 100;
                values["Zeit"] = n.firstChildElement("TIME").text().toDouble();
                values["Alpha"] = n.firstChildElement("ALPHA").text().toDouble();
                values["Pellets"] = n.firstChildElement("FORM").text() == "Pellet";
                values["Vorderwuerze"] = use == "First Wort";
                bh->modelHopfengaben()->append(values);
            }
        }

        // Hefe
        QDomNode nYeasts = nRecipe.firstChildElement("YEASTS");
        for(QDomNode n = nYeasts.firstChildElement("YEAST"); !n.isNull(); n = n.nextSiblingElement("YEAST"))
        {
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = n.firstChildElement("NAME").text();
            values["Menge"] = 0;
            bh->modelHefegaben()->append(values);
        }

        QDomNode nMiscs = nRecipe.firstChildElement("MISCS");
        for(QDomNode n = nMiscs.firstChildElement("MISC"); !n.isNull(); n = n.nextSiblingElement("MISC"))
        {
            values.clear();
            values["SudID"] = sudId;
            values["Name"] = n.firstChildElement("NAME").text();
            QString type = n.firstChildElement("TYPE").text();
            if (type == "Spice" || type == "Herb")
                values["Typ"] = EWZ_Typ_Gewuerz;
            else if (type == "Flavor")
                values["Typ"] = EWZ_Typ_Frucht;
            else
                values["Typ"] = EWZ_Typ_Sonstiges;
            QString use = n.firstChildElement("USE").text();
            if (use == "Boil")
               values["Zeitpunkt"] = EWZ_Zeitpunkt_Kochen;
            else if (use == "Mash")
                values["Zeitpunkt"] = EWZ_Zeitpunkt_Maischen;
            else
                values["Zeitpunkt"] = EWZ_Zeitpunkt_Gaerung;
            values["Menge"] = n.firstChildElement("AMOUNT").text().toDouble() * 1000 / menge;
            values["Einheit"] = EWZ_Einheit_g;
            values["Zugabedauer"] = n.firstChildElement("TIME").text().toDouble();
            values["Entnahmeindex"] = EWZ_Entnahmeindex_KeineEntnahme;
            values["Bemerkung"] = n.firstChildElement("NOTES").text();
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
    int sudId = bh->modelSud()->data(sudRow, "ID").toInt();
    bool gebraut = bh->modelSud()->data(sudRow, "Status").toInt() != Sud_Status_Rezept;

    QJsonObject root;
    root["Name"] = bh->modelSud()->data(sudRow, "Sudname").toString();
    root["Datum"] = bh->modelSud()->data(sudRow, "Erstellt").toDate().toString("dd.MM.yyyy");
    root["Ausschlagswuerze"] = QString::number(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1);
    if (gebraut)
        root["Sudhausausbeute"] = QString::number(bh->modelSud()->data(sudRow, "erg_EffektiveAusbeute").toDouble(), 'f', 1);
    else
        root["Sudhausausbeute"] = QString::number(bh->modelSud()->data(sudRow, "AnlageSudhausausbeute").toDouble(), 'f', 1);
    root["Stammwuerze"] = QString::number(bh->modelSud()->data(sudRow, "SW").toDouble(), 'f', 1);
    root["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IBU").toInt());
    root["Farbe"] = QString::number(bh->modelSud()->data(sudRow, "erg_Farbe").toInt());
    if (gebraut)
        root["Alkohol"] = QString::number(bh->modelSud()->data(sudRow, "erg_Alkohol").toDouble(), 'f', 1);
    root["Kurzbeschreibung"] = bh->modelSud()->data(sudRow, "Kommentar").toString();
    root["Infusion_Hauptguss"] = QString::number(bh->modelSud()->data(sudRow, "erg_WHauptguss").toDouble(), 'f', 1);
    if (gebraut)
        root["Endvergaerungsgrad"] = QString::number(bh->modelSud()->data(sudRow, "tEVG").toDouble(), 'f', 1);
    root["Karbonisierung"] = QString::number(bh->modelSud()->data(sudRow, "CO2").toDouble(), 'f', 1);
    root["Nachguss"] = QString::number(bh->modelSud()->data(sudRow, "erg_WNachguss").toDouble(), 'f', 1);
    root["Kochzeit_Wuerze"] = QString::number(bh->modelSud()->data(sudRow, "KochdauerNachBitterhopfung").toInt());

    // Rasten
    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(bh->modelRasten()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QString("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, "Temp").toInt());
        root[QString("Infusion_Rastzeit%1").arg(n)] = QString::number(model.data(row, "Dauer").toInt());
        ++n;
    }
    root["Infusion_Einmaischtemperatur"] = QString::number(bh->modelSud()->data(sudRow, "EinmaischenTemp").toInt());
    root["Abmaischtemperatur"] = "78";

    // Malzschuettung
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QString("Malz%1").arg(n)] = model.data(row, "Name").toString();
        root[QString("Malz%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toDouble(), 'f', 2);
        root[QString("Malz%1_Einheit").arg(n)] = "kg";
        ++n;
    }
    root["Maischform"] = "infusion";

    // Hopfen
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(bh->modelHopfengaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (model.data(row, "Vorderwuerze").toBool())
        {
            root[QString("Hopfen_VWH_%1_Sorte").arg(n)] = model.data(row, "Name").toString();
            root[QString("Hopfen_VWH_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
            root[QString("Hopfen_VWH_%1_alpha").arg(n)] = QString::number(model.data(row, "Alpha").toDouble(), 'f', 1);
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (!model.data(row, "Vorderwuerze").toBool())
        {
            root[QString("Hopfen_%1_Sorte").arg(n)] = model.data(row, "Name").toString();
            root[QString("Hopfen_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
            root[QString("Hopfen_%1_alpha").arg(n)] = QString::number(model.data(row, "Alpha").toDouble(), 'f', 1);
            root[QString("Hopfen_%1_Kochzeit").arg(n)] = QString::number(model.data(row, "Zeit").toInt());
            ++n;
        }
    }

    // Hefe
    QStringList hefen;
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        QString hefe = model.data(row, "Name").toString();
        if (!hefen.contains(hefe))
            hefen.append(hefe);
    }
    root["Hefe"] = hefen.join(", ");

    // Weitere Zutaten
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, "Zeitpunkt").toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Maischen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, "Name").toString();
            if (model.data(row, "Einheit").toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
            }
            root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(bh->modelSud()->data(sudRow, "KochdauerNachBitterhopfung").toInt());
            ++n;
        }
        else if (zeitpunkt == EWZ_Zeitpunkt_Kochen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, "Name").toString();
            if (model.data(row, "Einheit").toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
                root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(model.data(row, "Zugabedauer").toInt());
            }
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, "Zeitpunkt").toInt();
        int typ = model.data(row, "Typ").toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung && typ == EWZ_Typ_Hopfen)
        {
            root[QString("Stopfhopfen_%1_Sorte").arg(n)] = model.data(row, "Name").toString();
            root[QString("Stopfhopfen_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int zeitpunkt = model.data(row, "Zeitpunkt").toInt();
        int typ = model.data(row, "Typ").toInt();
        if (zeitpunkt == EWZ_Zeitpunkt_Gaerung && typ != EWZ_Typ_Hopfen)
        {
            root[QString("WeitereZutat_Gaerung_%1_Name").arg(n)] = model.data(row, "Name").toString();
            if (model.data(row, "Einheit").toInt() == EWZ_Einheit_Kg)
            {
                root[QString("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Gaerung_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, "erg_Menge").toInt());
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
    int sudId = bh->modelSud()->data(sudRow, "ID").toInt();
    bool gebraut = bh->modelSud()->data(sudRow, "Status").toInt() != Sud_Status_Rezept;

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
    text = doc.createTextNode(bh->modelSud()->data(sudRow, "Sudname").toString());
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
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BOIL_SIZE");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, "MengeSollKochbeginn").toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("BOIL_TIME");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, "KochdauerNachBitterhopfung").toInt()));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("EFFICIENCY");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, gebraut ? "erg_EffektiveAusbeute" : "AnlageSudhausausbeute").toDouble(), 'f', 1));
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

    val = BierCalc::platoToDichte(bh->modelSud()->data(sudRow, "SW").toDouble());
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

    val = bh->modelSud()->data(sudRow, "IBU").toDouble();
    element = doc.createElement("IBU_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("IBU_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, "erg_Farbe").toDouble() * 0.508;
    element = doc.createElement("COLOR_MIN");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("COLOR_MAX");
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, "CO2").toDouble();
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
    model.setFilterKeyColumn(bh->modelHopfengaben()->fieldIndex("SudID"));
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
        text = doc.createTextNode(model.data(row, "Name").toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("ALPHA");
        text = doc.createTextNode(QString::number(model.data(row, "Alpha").toDouble(), 'f', 1));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode(QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 3));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("USE");
        text = doc.createTextNode(model.data(row, "Vorderwuerze").toBool() ? "First Wort" : "Boil");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TIME");
        text = doc.createTextNode(QString::number(model.data(row, "Zeit").toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        int typ = Hopfen_Universal;
        int rowHopfen = bh->modelHopfen()->getRowWithValue("Beschreibung", model.data(row, "Name"));
        if (rowHopfen >= 0)
        {
             typ = bh->modelHopfen()->data(rowHopfen, "Typ").toInt();
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
        text = doc.createTextNode(model.data(row, "Pellets").toBool() ? "Pellet" : "Leaf");
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (model.data(row, "Typ").toInt() == EWZ_Typ_Hopfen)
        {
            Anteil = doc.createElement("HOP");
            Hopfengaben.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, "Name").toString());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("USE");
            text = doc.createTextNode("Dry Hop");
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TIME");
            text = doc.createTextNode(QString::number(model.data(row, "Zugabedauer").toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, "Bemerkung").toString();
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
            int rowHopfen = bh->modelHopfen()->getRowWithValue("Beschreibung", model.data(row, "Name"));
            if (rowHopfen >= 0)
            {
                alpha = bh->modelHopfen()->data(rowHopfen, "Alpha").toDouble();
                pellets = bh->modelHopfen()->data(rowHopfen, "Pellets").toBool();
                typ = bh->modelHopfen()->data(rowHopfen, "Typ").toInt();
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
    model.setFilterKeyColumn(bh->modelMalzschuettung()->fieldIndex("SudID"));
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
        text = doc.createTextNode(model.data(row, "Name").toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Grain");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode(QString::number(model.data(row, "erg_Menge").toDouble(), 'f', 2));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("YIELD");
        text = doc.createTextNode("100");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("COLOR");
        double ebc = model.data(row, "Farbe").toDouble();
        double srm = ebc * 0.508;
        double l = (srm + 0.76) / 1.3546;
        text = doc.createTextNode(QString::number(l));
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        double Ausbeute = model.data(row, "Ausbeute").toDouble();
        if (Ausbeute > 0)
        {
            Anteil = doc.createElement("FERMENTABLE");
            fermentables.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, "Name").toString());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (model.data(row, "Typ").toInt())
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
            text = doc.createTextNode(QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("YIELD");
            text = doc.createTextNode(QString::number(Ausbeute, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("COLOR");
            double ebc = model.data(row, "Farbe").toDouble();
            double srm = ebc * 0.508;
            double l = (srm + 0.76) / 1.3546;
            text = doc.createTextNode(QString::number(l));
            element.appendChild(text);
            Anteil.appendChild(element);

            if (model.data(row, "Zeitpunkt").toInt() == EWZ_Zeitpunkt_Gaerung)
            {
                element = doc.createElement("ADD_AFTER_BOIL");
                text = doc.createTextNode("TRUE");
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            str = model.data(row, "Bemerkung").toString();
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
    model.setFilterKeyColumn(bh->modelHefegaben()->fieldIndex("SudID"));
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
        text = doc.createTextNode(model.data(row, "Name").toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Lager");
        element.appendChild(text);
        Anteil.appendChild(element);

        bool liquid = false;
        int rowHefe = bh->modelHefe()->getRowWithValue("Beschreibung", model.data(row, "Name"));
        if (rowHefe >= 0)
        {
            liquid = bh->modelHefe()->data(rowHefe, "TypTrFl").toInt() == Hefe_Fluessig;
        }

        element = doc.createElement("FORM");
        text = doc.createTextNode(liquid ? "Liquid" : "Dry");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("AMOUNT");
        text = doc.createTextNode("0");
        element.appendChild(text);
        Anteil.appendChild(element);

        if (model.data(row, "ZugabeNach").toInt() > 0)
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
    model.setFilterKeyColumn(bh->modelWeitereZutatenGaben()->fieldIndex("SudID"));
    model.setFilterRegExp(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        int typ = model.data(row, "Typ").toInt();
        double Ausbeute = model.data(row, "Ausbeute").toDouble();
        if (typ != EWZ_Typ_Hopfen && Ausbeute == 0)
        {
            Anteil = doc.createElement("MISC");
            miscs.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, "Name").toString());
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
            switch (model.data(row, "Zeitpunkt").toInt())
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
            text = doc.createTextNode(QString::number(model.data(row, "Zugabedauer").toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, "erg_Menge").toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("AMOUNT_IS_WEIGHT");
            text = doc.createTextNode("TRUE");
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, "Bemerkung").toString();
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
    text = doc.createTextNode(bh->modelSud()->data(sudRow, "Anlage").toString());
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement("AMOUNT");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, "erg_W_Gesamt").toDouble(), 'f', 1));
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
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, "EinmaischenTemp").toInt()));
    element.appendChild(text);
    Anteil.appendChild(element);

    element = doc.createElement("STEP_TIME");
    text = doc.createTextNode("10");
    element.appendChild(text);
    Anteil.appendChild(element);

    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(bh->modelRasten()->fieldIndex("SudID"));
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
        text = doc.createTextNode(model.data(row, "Name").toString());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Temperature");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("STEP_TEMP");
        text = doc.createTextNode(QString::number(model.data(row, "Temp").toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("STEP_TIME");
        text = doc.createTextNode(QString::number(model.data(row, "Dauer").toInt()));
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
