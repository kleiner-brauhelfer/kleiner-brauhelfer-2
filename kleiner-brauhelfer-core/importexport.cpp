#include "importexport.h"
#include <QtMath>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include "brauhelfer.h"

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

int ImportExport::importKbh(Brauhelfer* bh, const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        qWarning() << "Failed to parse JSON:" << jsonError.errorString();
        return -1;
    }

    QJsonObject root = doc.object();
    QJsonObject obj;
    QJsonArray array;
    QVariantMap values;
    SqlTableModel* model;

    obj = root["Global"].toObject();
    int version = obj["db_Version"].toInt();
    if (version < 2000)
    {
        qWarning() << "Invalid version:" << version;
        return -1;
    }

    model = bh->modelSud();
    int sudId = model->getNextId();
    values = root[model->tableName()].toObject().toVariantMap();
    values["ID"] = sudId;
    int sudRow = model->appendDirect(values);

    model = bh->modelRasten();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelMalzschuettung();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHopfengaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHefegaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelWeitereZutatenGaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelSchnellgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHauptgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelNachgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelBewertungen();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelAnhang();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelEtiketten();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelTags();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        if (values["Global"].toBool())
            continue;
        values["SudID"] = sudId;
        values.remove("Global");
        model->appendDirect(values);
    }

    model = bh->modelWasseraufbereitung();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values["SudID"] = sudId;
        model->appendDirect(values);
    }

    bh->modelSud()->update(sudRow);
    return sudRow;
}

int ImportExport::importMaischeMalzundMehr(Brauhelfer *bh, const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        qWarning() << "Failed to parse JSON:" << jsonError.errorString();
        return -1;
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
    if (root["Maischform"].toString() == "infusion")
        values[ModelSud::ColFaktorHauptguss] = toDouble(root["Infusion_Hauptguss"]) / gesamt_schuettung;
    else if (root["Maischform"].toString() == "dekoktion")
        values[ModelSud::ColFaktorHauptguss] = toDouble(root["Dekoktion_Einmaisch_Zubruehwasser_gesamt"]) / gesamt_schuettung;
    values[ModelSud::ColCO2] = toDouble(root["Karbonisierung"]);
    values[ModelSud::ColIBU] = toDouble(root["Bittere"]);
    values[ModelSud::ColberechnungsArtHopfen] = static_cast<int>(Brauhelfer::BerechnungsartHopfen::Keine);
    values[ModelSud::ColKochdauer] = toDouble(root["Kochzeit_Wuerze"]);
    values[ModelSud::ColVergaerungsgrad] = toDouble(root["Endvergaerungsgrad"]);
    values[ModelSud::ColKommentar] = QString("Rezept aus MaischeMalzundMehr\n"
                                              "<b>Autor: </b> %1\n"
                                              "<b>Datum: </b> %2\n"
                                              "<b>Sorte: </b> %3\n\n"
                                              "%4\n\n%5").arg(root["Autor"].toString(),
                                                              root["Datum"].toString(),
                                                              root["Sorte"].toString(),
                                                              root["Kurzbeschreibung"].toString(),
                                                              root["Anmerkung_Autor"].toString());
    int sudRow = bh->modelSud()->append(values);
    bh->modelSud()->update(sudRow);
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    // Rasten
    if (root["Maischform"].toString() == "infusion")
    {
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
        values[ModelRasten::ColName] = "Einmaischen";
        values[ModelRasten::ColTemp] = toDouble(root["Infusion_Einmaischtemperatur"]);
        values[ModelRasten::ColDauer] = 10;
        bh->modelRasten()->appendDirect(values);
        int max_rasten = findMax(root, "Infusion_Rasttemperatur%%");
        for (int i = 1; i < max_rasten; ++i)
        {
            values.clear();
            values[ModelRasten::ColSudID] = sudId;
            values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Temperatur);
            values[ModelRasten::ColName] = QString("%1. Rast").arg(i);
            values[ModelRasten::ColTemp] = toDouble(root[QString("Infusion_Rasttemperatur%1").arg(i)]);
            values[ModelRasten::ColDauer] = toDouble(root[QString("Infusion_Rastzeit%1").arg(i)]);
            bh->modelRasten()->appendDirect(values);
        }
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Temperatur);
        values[ModelRasten::ColName] = "Abmaischen";
        values[ModelRasten::ColTemp] = toDouble(root["Abmaischtemperatur"]);
        values[ModelRasten::ColDauer] = 1;
        bh->modelRasten()->appendDirect(values);
    }
    else if (root["Maischform"].toString() == "dekoktion")
    {
        double V_tot = toDouble(root["Dekoktion_Einmaisch_Zubruehwasser_gesamt"]);
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
        values[ModelRasten::ColName] = "Einmaischen";
        values[ModelRasten::ColMengenfaktor] = toDouble(root["Dekoktion_0_Volumen"]) / V_tot;
        values[ModelRasten::ColTemp] = toDouble(root["Dekoktion_0_Temperatur_resultierend"]);
        values[ModelRasten::ColDauer] = toDouble(root["Dekoktion_0_Rastzeit"]);
        bh->modelRasten()->appendDirect(values);
        int max_rasten = findMax(root, "Dekoktion_%%_Volumen");
        for (int i = 1; i < max_rasten; ++i)
        {
            values.clear();
            values[ModelRasten::ColSudID] = sudId;
            values[ModelRasten::ColName] = root[QString("Dekoktion_%1_Form").arg(i)].toString();
            values[ModelRasten::ColMengenfaktor] = toDouble(root[QString("Dekoktion_%1_Volumen").arg(i)]) / V_tot;
            values[ModelRasten::ColTemp] = toDouble(root[QString("Dekoktion_%1_Temperatur_resultierend").arg(i)]);
            values[ModelRasten::ColDauer] = toDouble(root[QString("Dekoktion_%1_Rastzeit").arg(i)]);
            if (values[ModelRasten::ColName] == "Kochendes Wasser")
            {
                values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Infusion);
                values[ModelRasten::ColParam1] = 95;
            }
            else
            {
                values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Dekoktion);
                values[ModelRasten::ColParam1] = 95;
                if (root.contains(QString("Dekoktion_%1_Teilmaische_Kochzeit").arg(i)))
                    values[ModelRasten::ColParam2] = toDouble(root[QString("Dekoktion_%1_Teilmaische_Kochzeit").arg(i)]);
                else
                    values[ModelRasten::ColParam2] = 15;
                values[ModelRasten::ColParam3] = toDouble(root[QString("Dekoktion_%1_Teilmaische_Temperatur").arg(i)]);
                values[ModelRasten::ColParam4] = toDouble(root[QString("Dekoktion_%1_Teilmaische_Rastzeit").arg(i)]);
            }
            bh->modelRasten()->appendDirect(values);
        }
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Temperatur);
        values[ModelRasten::ColName] = "Abmaischen";
        values[ModelRasten::ColTemp] = toDouble(root["Abmaischtemperatur"]);
        values[ModelRasten::ColDauer] = 10;
        bh->modelRasten()->appendDirect(values);
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
        values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Vorderwuerze);
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
    const QStringList hefen = root["Hefe"].toString().split(", ");
    for (const QString& hefe : hefen)
    {
        values.clear();
        values[ModelHefegaben::ColSudID] = sudId;
        values[ModelHefegaben::ColName] = hefe;
        values[ModelHefegaben::ColMenge] = qCeil(menge / 20.0 / hefen.size());
        bh->modelHefegaben()->append(values);
    }

    // Weitere Zutaten
    int max_wz_wuerze = findMax(root, "WeitereZutat_Wuerze_%%_Name");
    for (int i = 1; i < max_wz_wuerze; ++i)
    {
        values.clear();
        values[ModelWeitereZutatenGaben::ColSudID] = sudId;
        values[ModelWeitereZutatenGaben::ColName] = root[QString("WeitereZutat_Wuerze_%1_Name").arg(i)].toString();
        values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Menge").arg(i)]) / menge;
        if (root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(i)].toString() == "g")
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
        values[ModelWeitereZutatenGaben::ColZugabedauer] = toDouble(root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(i)]);
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
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
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
        values[ModelWeitereZutatenGaben::ColZugabedauer] = 0;
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
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
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
        else
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
        values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen);
        values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
        values[ModelWeitereZutatenGaben::ColZugabedauer] = 7200;
        values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
        bh->modelWeitereZutatenGaben()->append(values);
    }

    bh->modelSud()->update(sudRow);
    return sudRow;
}

int ImportExport::importBeerXml(Brauhelfer* bh, const QByteArray &content)
{
    int sudRow = -1;
    const QString BeerXmlVersion = "1";

    QString xmlError;
    QDomDocument doc("");
    doc.setContent(content, &xmlError);
    if (!xmlError.isEmpty())
    {
        qWarning() << "Failed to parse XML:" << xmlError;
        return -1;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "RECIPES")
        return -1;

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
        min = nStyle.firstChildElement("CARB_MIN").text().toDouble();
        max = nStyle.firstChildElement("CARB_MAX").text().toDouble();
        values[ModelSud::ColCO2] = (min+max)/2;
        min = nStyle.firstChildElement("IBU_MIN").text().toDouble();
        max = nStyle.firstChildElement("IBU_MAX").text().toDouble();
        values[ModelSud::ColIBU] = (min+max)/2;
        values[ModelSud::ColberechnungsArtHopfen] = static_cast<int>(Brauhelfer::BerechnungsartHopfen::Keine);
        values[ModelSud::ColKochdauer] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();

        sudRow = bh->modelSud()->append(values);
        bh->modelSud()->update(sudRow);
        int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

        // Rasten
        double V_tot = bh->modelSud()->data(sudRow, ModelSud::ColWHauptgussEmpfehlung).toDouble();
        values.clear();
        values[ModelRasten::ColSudID] = sudId;
        values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
        values[ModelRasten::ColName] = "Einmaischen";
        values[ModelRasten::ColTemp] = nMashSteps.firstChildElement("MASH_STEP").firstChildElement("STEP_TEMP").text().toDouble();
        values[ModelRasten::ColDauer] = 10;
        bh->modelRasten()->appendDirect(values);
        for(QDomNode n = nMashSteps.firstChildElement("MASH_STEP"); !n.isNull(); n = n.nextSiblingElement("MASH_STEP"))
        {
            QString typ = n.firstChildElement("TYP").text().toLower();
            values.clear();
            values[ModelRasten::ColSudID] = sudId;
            values[ModelRasten::ColName] = n.firstChildElement("NAME").text();
            values[ModelRasten::ColTemp] = n.firstChildElement("STEP_TEMP").text().toDouble();
            values[ModelRasten::ColDauer] = n.firstChildElement("STEP_TIME").text().toDouble();
            if (typ == "temperature")
            {
                values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Temperatur);
            }
            else if (typ == "infusion")
            {
                values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Infusion);
                values[ModelRasten::ColMengenfaktor] = n.firstChildElement("INFUSE_AMOUNT").text().toDouble() / V_tot;
            }
            else if (typ == "decoction")
            {
                values[ModelRasten::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Dekoktion);
            }
            bh->modelRasten()->appendDirect(values);
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
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
                if (type == "Sugar" || type == "Extract" || type == "Dry Extract")
                    values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Zucker);
                if (n.firstChildElement("ADD_AFTER_BOIL").text() == "TRUE")
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
                }
                else
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
                    values[ModelWeitereZutatenGaben::ColZugabedauer] = nRecipe.firstChildElement("BOIL_TIME").text().toDouble();
                }
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
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
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen);
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
                values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement("TIME").text().toDouble();
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
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
                if (use == "First Wort")
                    values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Vorderwuerze);
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
            if (type == "Spice")
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz);
            else if (type == "Herb")
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Kraut);
            else if (type == "Flavor")
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Frucht);
            QString use = n.firstChildElement("USE").text();
            if (use == "Boil")
               values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
            else if (use == "Mash")
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Maischen);
            else
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement("AMOUNT").text().toDouble() * 1000 / menge;
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement("TIME").text().toDouble();
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement("NOTES").text();
            bh->modelWeitereZutatenGaben()->append(values);
        }
    }

    bh->modelSud()->update(sudRow);
    return sudRow;
}

QByteArray ImportExport::exportKbh(Brauhelfer* bh, int sudRow, const QStringList& exclude)
{
    QJsonObject root;
    QJsonArray array;
    SqlTableModel* model;
    ProxyModel proxy;
    QVariantList list;

    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();
    QRegularExpression regExpId(QString("^%1$").arg(sudId));

    root["Global"] = QJsonObject({{"db_Version", bh->databaseVersion()}});

    if (!exclude.contains(bh->modelSud()->tableName()))
        root[bh->modelSud()->tableName()] = QJsonObject::fromVariantMap(bh->modelSud()->toVariantMap(sudRow, {ModelSud::ColID}));

    model = bh->modelRasten();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelRasten::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelRasten::ColID, ModelRasten::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelMalzschuettung();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelHopfengaben();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelHopfengaben::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelHefegaben();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelHefegaben::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelHefegaben::ColID, ModelHefegaben::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelWeitereZutatenGaben();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelSchnellgaerverlauf();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelSchnellgaerverlauf::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelHauptgaerverlauf();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelHauptgaerverlauf::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelNachgaerverlauf();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelNachgaerverlauf::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelBewertungen();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelBewertungen::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelBewertungen::ColID, ModelBewertungen::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelAnhang();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelAnhang::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelAnhang::ColID, ModelAnhang::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelEtiketten();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelEtiketten::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelEtiketten::ColID, ModelEtiketten::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelTags();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelTags::ColSudID);
        proxy.setFilterRegularExpression(QRegularExpression(QString("^(%1|-.*)$").arg(sudId)));
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
        {
            QVariantMap values = model->toVariantMap(proxy.mapRowToSource(row), {ModelTags::ColID, ModelTags::ColSudID});
            values["Global"] = proxy.data(row, ModelTags::ColGlobal).toBool();
            list.append(values);
        }
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelWasseraufbereitung();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelWasseraufbereitung::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), {ModelWasseraufbereitung::ColID, ModelWasseraufbereitung::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
        }

    QJsonDocument doc(root);
    return doc.toJson();
}

QByteArray ImportExport::exportMaischeMalzundMehr(Brauhelfer *bh, int sudRow)
{
    ProxyModel model;
    int n;
    bool dekoktion = false;
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
    root["Endvergaerungsgrad"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toDouble(), 'f', 1);
    root["Karbonisierung"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
    root["Nachguss"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WNachguss).toDouble(), 'f', 1);
    root["Kochzeit_Wuerze"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());

    // Rasten
    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(ModelRasten::ColSudID);
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(row, ModelRasten::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            break;
        case Brauhelfer::RastTyp::Temperatur:
            break;
        case Brauhelfer::RastTyp::Infusion:
            dekoktion = true;
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            dekoktion = true;
            break;
        }
        if (dekoktion)
            break;
    }
    if (dekoktion)
    {
        root["Maischform"] = "dekoktion";
        root["Dekoktion_Einmaisch_Zubruehwasser_gesamt"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble(), 'f', 1);
    }
    else
    {
        root["Maischform"] = "infusion";
        root["Infusion_Hauptguss"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble(), 'f', 1);
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(row, ModelRasten::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            if (!dekoktion)
            {
                root["Infusion_Einmaischtemperatur"] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                int dauer = model.data(row, ModelRasten::ColDauer).toInt();
                if (dauer > 0)
                {
                    root[QString("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                    root[QString("Infusion_Rastzeit%1").arg(n)] = QString::number(dauer);
                    ++n;
                }
            }
            else
            {
                root["Dekoktion_0_Volumen"] = QString::number(model.data(row, ModelRasten::ColMenge).toInt());
                root["Dekoktion_0_Temperatur_ist"] = QString::number(model.data(row, ModelRasten::ColParam1).toInt());
                root["Dekoktion_0_Temperatur_resultierend"] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                root["Dekoktion_0_Rastzeit"] = QString::number(model.data(row, ModelRasten::ColDauer).toInt());
            }
            break;
        case Brauhelfer::RastTyp::Temperatur:
            if (!dekoktion)
            {
                root[QString("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                root[QString("Infusion_Rastzeit%1").arg(n)] = QString::number(model.data(row, ModelRasten::ColDauer).toInt());
                ++n;
            }
            break;
        case Brauhelfer::RastTyp::Infusion:
            if (dekoktion)
            {
                root[QString("Dekoktion_%1_Form").arg(n)] = "Kochendes Wasser";
                root[QString("Dekoktion_%1_Volumen").arg(n)] = QString::number(model.data(row, ModelRasten::ColMenge).toDouble(), 'f', 1);
                root[QString("Dekoktion_%1_Temperatur_resultierend").arg(n)] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                root[QString("Dekoktion_%1_Rastzeit").arg(n)] = QString::number(model.data(row, ModelRasten::ColDauer).toInt());
                ++n;
            }
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            if (dekoktion)
            {
                root[QString("Dekoktion_%1_Form").arg(n)] = "Dünnmaische";
                root[QString("Dekoktion_%1_Volumen").arg(n)] = QString::number(model.data(row, ModelRasten::ColMenge).toDouble(), 'f', 1);
                root[QString("Dekoktion_%1_Temperatur_resultierend").arg(n)] = QString::number(model.data(row, ModelRasten::ColTemp).toInt());
                root[QString("Dekoktion_%1_Rastzeit").arg(n)] = QString::number(model.data(row, ModelRasten::ColDauer).toInt());
                root[QString("Dekoktion_%1_Teilmaische_Temperatur").arg(n)] = QString::number(model.data(row, ModelRasten::ColParam3).toInt());
                root[QString("Dekoktion_%1_Teilmaische_Kochzeit").arg(n)] = QString::number(model.data(row, ModelRasten::ColParam2).toInt());
                root[QString("Dekoktion_%1_Teilmaische_Rastzeit").arg(n)] = QString::number(model.data(row, ModelRasten::ColParam4).toInt());
                ++n;
            }
            break;
        }
    }
    root["Abmaischtemperatur"] = "78";

    // Malzschuettung
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QString("Malz%1").arg(n)] = model.data(row, ModelMalzschuettung::ColName).toString();
        root[QString("Malz%1_Menge").arg(n)] = QString::number(model.data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2);
        root[QString("Malz%1_Einheit").arg(n)] = "kg";
        ++n;
    }

    // Hopfen
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
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
        if (static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) != Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Maischen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QString("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QString("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
            }
            root[QString("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());
            ++n;
        }
        else if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen)
        {
            root[QString("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
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
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung && typ == Brauhelfer::ZusatzTyp::Hopfen)
        {
            root[QString("Stopfhopfen_%1_Sorte").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            root[QString("Stopfhopfen_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung && typ != Brauhelfer::ZusatzTyp::Hopfen)
        {
            root[QString("WeitereZutat_Gaerung_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
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

    QJsonDocument doc(root);
    return doc.toJson();
}

QByteArray ImportExport::exportBeerXml(Brauhelfer* bh, int sudRow)
{
    const QString BeerXmlVersion = "1";

    double val;
    QString str;
    ProxyModel model;
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    QDomDocument doc;
    QDomText text;
    QDomElement element;
    QDomElement Anteil;

    QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" );
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
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString().toHtmlEscaped());
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
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt()));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement("EFFICIENCY");
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    QDomElement style = doc.createElement("STYLE");
    Rezept.appendChild(style);

    element = doc.createElement("VERSION");
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("NAME");
    text = doc.createTextNode("Unknown");
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement("CATEGORY");
    text = doc.createTextNode("Unknown");
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("HOP");
        Hopfengaben.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelHopfengaben::ColName).toString().toHtmlEscaped());
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
        text = doc.createTextNode(static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze ? "First Wort" : "Boil");
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TIME");
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::ColZeit).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        Brauhelfer::HopfenTyp typ = Brauhelfer::HopfenTyp::Universal;
        int rowHopfen = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, model.data(row, ModelHopfengaben::ColName));
        if (rowHopfen >= 0)
        {
             typ = static_cast<Brauhelfer::HopfenTyp>(bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColTyp).toInt());
        }

        element = doc.createElement("TYPE");
        switch (typ)
        {
        case Brauhelfer::HopfenTyp::Aroma:
            text = doc.createTextNode("Aroma");
            break;
        case Brauhelfer::HopfenTyp::Bitter:
            text = doc.createTextNode("Bittering");
            break;
        case Brauhelfer::HopfenTyp::Unbekannt:
        case Brauhelfer::HopfenTyp::Universal:
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        {
            Anteil = doc.createElement("HOP");
            Hopfengaben.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
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

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str);
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            double alpha = 0.0;
            bool pellets = false;
            Brauhelfer::HopfenTyp typ = Brauhelfer::HopfenTyp::Universal;
            int rowHopfen = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, model.data(row, ModelWeitereZutatenGaben::ColName));
            if (rowHopfen >= 0)
            {
                alpha = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColAlpha).toDouble();
                pellets = bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColPellets).toBool();
                typ = static_cast<Brauhelfer::HopfenTyp>(bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColTyp).toInt());
            }

            element = doc.createElement("ALPHA");
            text = doc.createTextNode(QString::number(alpha, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (typ)
            {
            case Brauhelfer::HopfenTyp::Aroma:
                text = doc.createTextNode("Aroma");
                break;
            case Brauhelfer::HopfenTyp::Bitter:
                text = doc.createTextNode("Bittering");
                break;
            case Brauhelfer::HopfenTyp::Unbekannt:
            case Brauhelfer::HopfenTyp::Universal:
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("FERMENTABLE");
        fermentables.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelMalzschuettung::ColName).toString().toHtmlEscaped());
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
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
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
            switch (typ)
            {
            case Brauhelfer::ZusatzTyp::Honig:
            case Brauhelfer::ZusatzTyp::Zucker:
            case Brauhelfer::ZusatzTyp::Frucht:
                text = doc.createTextNode("Sugar");
                break;
            case Brauhelfer::ZusatzTyp::Gewuerz:
            case Brauhelfer::ZusatzTyp::Sonstiges:
            case Brauhelfer::ZusatzTyp::Kraut:
            case Brauhelfer::ZusatzTyp::Wasseraufbereiung:
            case Brauhelfer::ZusatzTyp::Klaermittel:
            case Brauhelfer::ZusatzTyp::Hopfen:
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

            Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
            if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                element = doc.createElement("ADD_AFTER_BOIL");
                text = doc.createTextNode("TRUE");
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str);
                element.appendChild(text);
                Anteil.appendChild(element);
            }
        }
    }

    QDomElement yeasts = doc.createElement("YEASTS");
    Rezept.appendChild(yeasts);
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(ModelHefegaben::ColSudID);
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("YEAST");
        yeasts.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelHefegaben::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        text = doc.createTextNode("Lager");
        element.appendChild(text);
        Anteil.appendChild(element);

        bool liquid = false;
        int rowHefe = bh->modelHefe()->getRowWithValue(ModelHefe::ColName, model.data(row, ModelHefegaben::ColName));
        if (rowHefe >= 0)
        {
            Brauhelfer::HefeTyp typ = static_cast<Brauhelfer::HefeTyp>(bh->modelHefe()->data(rowHefe, ModelHefe::ColTypTrFl).toInt());
            liquid = typ == Brauhelfer::HefeTyp::Fluessig;
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
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        double Ausbeute = model.data(row, ModelWeitereZutatenGaben::ColAusbeute).toDouble();
        if (typ != Brauhelfer::ZusatzTyp::Hopfen && Ausbeute == 0)
        {
            Anteil = doc.createElement("MISC");
            miscs.appendChild(Anteil);

            element = doc.createElement("VERSION");
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("NAME");
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("TYPE");
            switch (typ)
            {
            case Brauhelfer::ZusatzTyp::Honig:
            case Brauhelfer::ZusatzTyp::Zucker:
                text = doc.createTextNode("Sugar");
                break;
            case Brauhelfer::ZusatzTyp::Frucht:
                text = doc.createTextNode("Flavor");
                break;
            case Brauhelfer::ZusatzTyp::Gewuerz:
                text = doc.createTextNode("Spice");
                break;
            case Brauhelfer::ZusatzTyp::Kraut:
                text = doc.createTextNode("Herb");
                break;
            case Brauhelfer::ZusatzTyp::Sonstiges:
            case Brauhelfer::ZusatzTyp::Wasseraufbereiung:
            case Brauhelfer::ZusatzTyp::Klaermittel:
                text = doc.createTextNode("Other");
                break;
            case Brauhelfer::ZusatzTyp::Hopfen:
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement("USE");
            Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
            switch (zeitpunkt)
            {
            case Brauhelfer::ZusatzZeitpunkt::Gaerung:
                text = doc.createTextNode("Primary");
                break;
            case Brauhelfer::ZusatzZeitpunkt::Kochen:
                text = doc.createTextNode("Boil");
                break;
            case Brauhelfer::ZusatzZeitpunkt::Maischen:
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

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
                element = doc.createElement("NOTES");
                text = doc.createTextNode(str);
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
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColWasserprofil).toString().toHtmlEscaped());
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

    model.setSourceModel(bh->modelRasten());
    model.setFilterKeyColumn(ModelRasten::ColSudID);
    model.setFilterRegularExpression(QString("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement("MASH_STEP");
        mash_steps.appendChild(Anteil);

        element = doc.createElement("VERSION");
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("NAME");
        text = doc.createTextNode(model.data(row, ModelRasten::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement("TYPE");
        Brauhelfer::RastTyp typ = static_cast<Brauhelfer::RastTyp>(model.data(row, ModelRasten::ColTyp).toInt());
        switch (typ)
        {
        case Brauhelfer::RastTyp::Einmaischen:
            text = doc.createTextNode("Temperature");
            break;
        case Brauhelfer::RastTyp::Temperatur:
            text = doc.createTextNode("Temperature");
            break;
        case Brauhelfer::RastTyp::Infusion:
            text = doc.createTextNode("Infusion");
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            text = doc.createTextNode("Decoction");
            break;
        }
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

        if (typ == Brauhelfer::RastTyp::Infusion)
        {
            element = doc.createElement("INFUSE_AMOUNT");
            text = doc.createTextNode(QString::number(model.data(row, ModelRasten::ColMenge).toDouble(), 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QString contentString;
    QTextStream stream(&contentString);
    doc.save(stream, QDomNode::EncodingFromTextStream);
    return contentString.toUtf8();
}
