#include "importexport.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDomDocument>
#include <QMessageBox>
#include "brauhelfer.h"

extern Brauhelfer* bh;

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

bool ImportExport::importMaischeMalzundMehr(const QString &fileName)
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

    QVariantMap values;
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
        values["Typ"] = EWZ_Typ_Sonstiges;
        values["Zeitpunkt"] = EWZ_Typ_Hopfen;
        values["Zugabedauer"] = 5;
        values["Entnahmeindex"] = EWZ_Entnahmeindex_MitEntnahme;
        bh->modelWeitereZutatenGaben()->append(values);
    }
    return true;
}

bool ImportExport::importBeerXml(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QString xmlError;
    QDomDocument doc("mydocument");
    doc.setContent(&file, &xmlError);
    file.close();
    if (!xmlError.isEmpty())
    {
        QMessageBox::warning(nullptr, QObject::tr("Fehler beim lesen der XML Datei"), xmlError);
        return false;
    }

    return true;
}

bool ImportExport::exportMaischeMalzundMehr(const QString &fileName, int sudRow)
{
    return false;
}

bool ImportExport::exportBeerXml(const QString &fileName, int sudRow)
{
    return false;
}
