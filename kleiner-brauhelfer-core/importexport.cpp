#include "importexport.h"
#include <QtMath>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include "proxymodel.h"
#include "brauhelfer.h"
#include "biercalc.h"

static int findMax(const QJsonObject& jsn, const QString& str, int MAX = 20)
{
    int i = 1;
    for (; i < MAX; ++i)
    {
        QString search = QString(str).replace(QStringLiteral("%%"), QString::number(i));
        if (!jsn.contains(search))
            break;
    }
    return i;
}

static double toDouble(QJsonValueRef value, double notFoundValue = 0.0)
{
    QString s;
    switch (value.type())
    {
    case QJsonValue::Double:
        return value.toDouble();
    case QJsonValue::String:
        s = value.toString();
        if (s.startsWith(QStringLiteral("\"")))
            s.truncate(1);
        if (s.endsWith(QStringLiteral("\"")))
            s.chop(1);
        return s.toDouble();
    default:
        return notFoundValue;
    }
}

static QString removeHtmlEntities(QString str)
{
    str.replace(QStringLiteral("&auml;"), QStringLiteral("ä"));
    str.replace(QStringLiteral("&ouml;"), QStringLiteral("ö"));
    str.replace(QStringLiteral("&uuml;"), QStringLiteral("ü"));
    str.replace(QStringLiteral("&lt;"), QStringLiteral("<"));
    str.replace(QStringLiteral("&gt;"), QStringLiteral(">"));
    str.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    str.replace(QStringLiteral("&quot;"), QStringLiteral("\""));
    return str;
}

int ImportExport::importKbh(Brauhelfer* bh, const QByteArray &content)
{
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        qWarning(Brauhelfer::loggingCategory) << "Failed to parse JSON:" << jsonError.errorString();
        return -1;
    }

    QJsonObject root = doc.object();
    QJsonObject obj;
    QJsonArray array;
    QVariantMap values;
    SqlTableModel* model;

    obj = root[QStringLiteral("Global")].toObject();
    int version = obj[QStringLiteral("db_Version")].toInt();
    if (version < 2000)
    {
        qWarning(Brauhelfer::loggingCategory) << "Invalid version:" << version;
        return -1;
    }

    model = bh->modelSud();
    int sudId = model->getNextId();
    values = root[model->tableName()].toObject().toVariantMap();
    values[QStringLiteral("ID")] = sudId;
    int sudRow = model->appendDirect(values);

    model = bh->modelMaischplan();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelMalzschuettung();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHopfengaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHefegaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelWeitereZutatenGaben();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelSchnellgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelHauptgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelNachgaerverlauf();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelBewertungen();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelAnhang();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelEtiketten();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
        model->appendDirect(values);
    }

    model = bh->modelTags();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        if (values[QStringLiteral("Global")].toBool())
            continue;
        values[QStringLiteral("SudID")] = sudId;
        values.remove(QStringLiteral("Global"));
        model->appendDirect(values);
    }

    model = bh->modelWasseraufbereitung();
    array = root[model->tableName()].toArray();
    for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
    {
        values = it->toObject().toVariantMap();
        values[QStringLiteral("SudID")] = sudId;
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
        qWarning(Brauhelfer::loggingCategory) << "Failed to parse JSON:" << jsonError.errorString();
        return -1;
    }

    QJsonObject root = doc.object();
    QJsonObject obj;
    QJsonArray array;
    QMap<int, QVariant> values;

    int version = int(toDouble(root[QStringLiteral("ExportVersion")], 1.0));
    if (version >= 3.0)
    {
        qWarning(Brauhelfer::loggingCategory) << "Version not supported:" << version;
        return -1;
    }

    double menge = 0.0;
    int max_schuettung = 0;
    double gesamt_schuettung = 0.0;
    if (version == 1)
    {
        menge = toDouble(root[QStringLiteral("Ausschlagswuerze")]);
        max_schuettung = findMax(root, QStringLiteral("Malz%%"));
        for (int i = 1; i < max_schuettung; ++i)
        {
            double kg = 0.0;
            if (root[QStringLiteral("Malz%1_Einheit").arg(i)].toString() == QStringLiteral("g"))
                kg = toDouble(root[QStringLiteral("Malz%1_Menge").arg(i)]) / 1000.0;
            else
                kg = toDouble(root[QStringLiteral("Malz%1_Menge").arg(i)]);
            gesamt_schuettung += kg;
        }
    }
    else if (version == 2)
    {
        menge = toDouble(root[QStringLiteral("Ausschlagwuerze")]);
        max_schuettung = array.count();
        array = root[QStringLiteral("Malze")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            double kg = 0.0;
            if (obj[QStringLiteral("Einheit")].toString() == QStringLiteral("g"))
                kg = toDouble(obj[QStringLiteral("Menge")]) / 1000.0;
            else
                kg = toDouble(obj[QStringLiteral("Menge")]);
            gesamt_schuettung += kg;
        }
    }

    // Sud
    values.clear();
    values[ModelSud::ColSudname] = removeHtmlEntities(root[QStringLiteral("Name")].toString());
    values[ModelSud::ColMenge] = menge;
    values[ModelSud::ColSW] = toDouble(root[QStringLiteral("Stammwuerze")]);
    values[ModelSud::ColSudhausausbeute] = toDouble(root[QStringLiteral("Sudhausausbeute")], 60.0);
    QString maischform = root[QStringLiteral("Maischform")].toString();
    if (maischform == QStringLiteral("infusion"))
        values[ModelSud::ColFaktorHauptguss] = toDouble(root[version == 1 ? QStringLiteral("Infusion_Hauptguss") : QStringLiteral("Hauptguss")]) / gesamt_schuettung;
    else if (maischform == QStringLiteral("dekoktion"))
        values[ModelSud::ColFaktorHauptguss] = toDouble(root[version == 1 ? QStringLiteral("Dekoktion_Einmaisch_Zubruehwasser_gesamt") : QStringLiteral("Einmaisch_Zubruehwasser_gesamt")]) / gesamt_schuettung;
    values[ModelSud::ColCO2] = toDouble(root[QStringLiteral("Karbonisierung")]);
    values[ModelSud::ColIBU] = toDouble(root[QStringLiteral("Bittere")]);
    values[ModelSud::ColberechnungsArtHopfen] = static_cast<int>(Brauhelfer::BerechnungsartHopfen::Keine);
    values[ModelSud::ColKochdauer] = toDouble(root[QStringLiteral("Kochzeit_Wuerze")]);
    values[ModelSud::ColVergaerungsgrad] = toDouble(root[QStringLiteral("Endvergaerungsgrad")]);
    values[ModelSud::ColKommentar] = QString("Rezept aus %1\n"
                                             "Autor: %2\n"
                                             "Datum: %3\n"
                                             "Sorte: %4\n\n"
                                             "%5\n\n%6")
                                         .arg(removeHtmlEntities(root[QStringLiteral("Rezeptquelle")].toString()),
                                              removeHtmlEntities(root[QStringLiteral("Autor")].toString()),
                                              removeHtmlEntities(root[QStringLiteral("Datum")].toString()),
                                              removeHtmlEntities(root[QStringLiteral("Sorte")].toString()),
                                              removeHtmlEntities(root[QStringLiteral("Kurzbeschreibung")].toString()),
                                              removeHtmlEntities(root[QStringLiteral("Anmerkung_Autor")].toString()));
    values[ModelSud::ColBemerkungGaerung] = QString(QStringLiteral("Gärtemperatur: %1 °C")).arg(removeHtmlEntities(root[QStringLiteral("Gaertemperatur")].toString()));
    int sudRow = bh->modelSud()->append(values);
    bh->modelSud()->update(sudRow);
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    // Rasten
    if (maischform == QStringLiteral("infusion"))
    {
        double last_temp = 0.0;
        values.clear();
        values[ModelMaischplan::ColSudID] = sudId;
        values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
        values[ModelMaischplan::ColName] = "Einmaischen";
        values[ModelMaischplan::ColAnteilWasser] = 100;
        values[ModelMaischplan::ColAnteilMalz] = 100;
        values[ModelMaischplan::ColTempMalz] = 21;
        values[ModelMaischplan::ColTempWasser] = toDouble(root[version == 1 ? QStringLiteral("Infusion_Einmaischtemperatur") : QStringLiteral("Einmaischtemperatur")]);
        values[ModelMaischplan::ColDauerRast] = 10;
        bh->modelMaischplan()->append(values);
        if (version == 1)
        {
            for (int i = 1; i < findMax(root, QStringLiteral("Infusion_Rasttemperatur%%")); ++i)
            {
                values.clear();
                values[ModelMaischplan::ColSudID] = sudId;
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
                values[ModelMaischplan::ColName] = QStringLiteral("%1. Rast").arg(i);
                last_temp = toDouble(root[QStringLiteral("Infusion_Rasttemperatur%1").arg(i)]);
                values[ModelMaischplan::ColTempRast] = last_temp;
                values[ModelMaischplan::ColDauerRast] = toDouble(root[QStringLiteral("Infusion_Rastzeit%1").arg(i)]);
                bh->modelMaischplan()->appendDirect(values);
            }
        }
        else if (version == 2)
        {
            array = root[QStringLiteral("Rasten")].toArray();
            int i = 1;
            for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
            {
                obj = it->toObject();
                values.clear();
                values[ModelMaischplan::ColSudID] = sudId;
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
                values[ModelMaischplan::ColName] = QStringLiteral("%1. Rast").arg(i);
                last_temp = toDouble(obj[QStringLiteral("Temperatur")]);
                values[ModelMaischplan::ColTempRast] = last_temp;
                values[ModelMaischplan::ColDauerRast] = toDouble(obj[QStringLiteral("Zeit")]);
                bh->modelMaischplan()->appendDirect(values);
                i++;
            }
        }
        double temp = toDouble(root[QStringLiteral("Abmaischtemperatur")]);
        if (temp > last_temp)
        {
            values.clear();
            values[ModelMaischplan::ColSudID] = sudId;
            values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
            values[ModelMaischplan::ColName] = "Abmaischen";
            values[ModelMaischplan::ColTempRast] = temp;
            values[ModelMaischplan::ColDauerRast] = 1;
            bh->modelMaischplan()->appendDirect(values);
        }
    }
    else if (maischform == QStringLiteral("dekoktion"))
    {
        if (version == 1)
        {
            double V_tot = toDouble(root[QStringLiteral("Dekoktion_Einmaisch_Zubruehwasser_gesamt")]);
            values.clear();
            values[ModelMaischplan::ColSudID] = sudId;
            values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
            values[ModelMaischplan::ColName] = "Einmaischen";
            values[ModelMaischplan::ColAnteilWasser] = 100 * toDouble(root[QStringLiteral("Dekoktion_0_Volumen")]) / V_tot;
            values[ModelMaischplan::ColAnteilMalz] = 100;
            values[ModelMaischplan::ColTempWasser] = toDouble(root[QStringLiteral("Temperatur_ist")]);
            values[ModelMaischplan::ColTempRast] = toDouble(root[QStringLiteral("Dekoktion_0_Temperatur_resultierend")]);
            values[ModelMaischplan::ColDauerRast] = toDouble(root[QStringLiteral("Dekoktion_0_Rastzeit")]);
            bh->modelMaischplan()->append(values);
            for (int i = 1; i < findMax(root, QStringLiteral("Dekoktion_%%_Volumen")); ++i)
            {
                values.clear();
                values[ModelMaischplan::ColSudID] = sudId;
                values[ModelMaischplan::ColName] = removeHtmlEntities(root[QStringLiteral("Dekoktion_%1_Form").arg(i)].toString());
                values[ModelMaischplan::ColAnteilMaische] = 100 * toDouble(root[QStringLiteral("Dekoktion_%1_Volumen").arg(i)]) / V_tot;
                values[ModelMaischplan::ColTempRast] = toDouble(root[QStringLiteral("Dekoktion_%1_Temperatur_resultierend").arg(i)]);
                values[ModelMaischplan::ColDauerRast] = toDouble(root[QStringLiteral("Dekoktion_%1_Rastzeit").arg(i)]);
                if (values[ModelMaischplan::ColName] == "Kochendes Wasser")
                {
                    values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Zubruehen);
                    values[ModelMaischplan::ColTempWasser] = 95;
                }
                else
                {
                    values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Dekoktion);
                    values[ModelMaischplan::ColTempExtra1] = 95;
                    if (root.contains(QStringLiteral("Dekoktion_%1_Teilmaische_Kochzeit").arg(i)))
                        values[ModelMaischplan::ColDauerExtra1] = toDouble(root[QStringLiteral("Dekoktion_%1_Teilmaische_Kochzeit").arg(i)]);
                    else
                        values[ModelMaischplan::ColDauerExtra1] = 15;
                    values[ModelMaischplan::ColTempExtra2] = toDouble(root[QStringLiteral("Dekoktion_%1_Teilmaische_Temperatur").arg(i)]);
                    values[ModelMaischplan::ColDauerExtra2] = toDouble(root[QStringLiteral("Dekoktion_%1_Teilmaische_Rastzeit").arg(i)]);
                }
                bh->modelMaischplan()->appendDirect(values);
            }
            values.clear();
            values[ModelMaischplan::ColSudID] = sudId;
            values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
            values[ModelMaischplan::ColName] = "Abmaischen";
            values[ModelMaischplan::ColTempRast] = toDouble(root[QStringLiteral("Abmaischtemperatur")]);
            values[ModelMaischplan::ColDauerRast] = 10;
            bh->modelMaischplan()->appendDirect(values);
        }
        else if (version == 2)
        {
            double V_tot = toDouble(root[QStringLiteral("Einmaisch_Zubruehwasser_gesamt")]);
            array = root[QStringLiteral("Dekoktionen")].toArray();
            qDebug() << array;
            if (array.count() > 0)
            {
                obj = array[0].toObject();
                values.clear();
                values[ModelMaischplan::ColSudID] = sudId;
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
                values[ModelMaischplan::ColName] = "Einmaischen";
                values[ModelMaischplan::ColAnteilWasser] = 100 * toDouble(obj[QStringLiteral("Volumen")]) / V_tot;
                values[ModelMaischplan::ColAnteilMalz] = 100;
                values[ModelMaischplan::ColTempWasser] = toDouble(obj[QStringLiteral("Temperatur_ist")]);
                values[ModelMaischplan::ColTempRast] = toDouble(obj[QStringLiteral("Temperatur_resultierend")]);
                values[ModelMaischplan::ColDauerRast] = toDouble(obj[QStringLiteral("Rastzeit")]);
                bh->modelMaischplan()->append(values);

            }
            for (QJsonArray::const_iterator it = array.constBegin()+1; it != array.constEnd(); ++it)
            {
                obj = it->toObject();
                values.clear();
                values[ModelMaischplan::ColSudID] = sudId;
                values[ModelMaischplan::ColName] = removeHtmlEntities(obj[QStringLiteral("Form")].toString());
                values[ModelMaischplan::ColAnteilMaische] = 100 * toDouble(obj[QStringLiteral("Volumen")]) / V_tot;
                values[ModelMaischplan::ColTempRast] = toDouble(obj[QStringLiteral("Temperatur_resultierend")]);
                values[ModelMaischplan::ColDauerRast] = toDouble(obj[QStringLiteral("Rastzeit")]);
                if (values[ModelMaischplan::ColName] == "Kochendes Wasser")
                {
                    values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Zubruehen);
                    values[ModelMaischplan::ColTempWasser] = 95;
                }
                else
                {
                    values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Dekoktion);
                    values[ModelMaischplan::ColTempExtra1] = 95;
                    if (obj.contains(QStringLiteral("Teilmaische_Kochzeit")))
                        values[ModelMaischplan::ColDauerExtra1] = toDouble(obj[QStringLiteral("Teilmaische_Kochzeit")]);
                    else
                        values[ModelMaischplan::ColDauerExtra1] = 15;
                    values[ModelMaischplan::ColTempExtra2] = toDouble(obj[QStringLiteral("Teilmaische_Temperatur")]);
                    values[ModelMaischplan::ColDauerExtra2] = toDouble(obj[QStringLiteral("Teilmaische_Rastzeit")]);
                }
                bh->modelMaischplan()->appendDirect(values);
            }
            values.clear();
            values[ModelMaischplan::ColSudID] = sudId;
            values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
            values[ModelMaischplan::ColName] = "Abmaischen";
            values[ModelMaischplan::ColTempRast] = toDouble(root[QStringLiteral("Abmaischtemperatur")]);
            values[ModelMaischplan::ColDauerRast] = 10;
            bh->modelMaischplan()->appendDirect(values);
        }
    }

    // Malzschuettung
    if (version == 1)
    {
        for (int i = 1; i < max_schuettung; ++i)
        {
            double kg = 0.0;
            if (root[QStringLiteral("Malz%1_Einheit").arg(i)].toString() == QStringLiteral("g"))
                kg = toDouble(root[QStringLiteral("Malz%1_Menge").arg(i)]) / 1000.0;
            else
                kg = toDouble(root[QStringLiteral("Malz%1_Menge").arg(i)]);
            values.clear();
            values[ModelMalzschuettung::ColSudID] = sudId;
            values[ModelMalzschuettung::ColName] = removeHtmlEntities(root[QStringLiteral("Malz%1").arg(i)].toString());
            values[ModelMalzschuettung::ColProzent] = kg / gesamt_schuettung * 100.0;
            values[ModelMalzschuettung::ColFarbe] = 1;
            bh->modelMalzschuettung()->append(values);
        }
    }
    else if (version == 2)
    {
        array = root[QStringLiteral("Malze")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            double kg = 0.0;
            if (obj[QStringLiteral("Einheit")].toString() == QStringLiteral("g"))
                kg = toDouble(obj[QStringLiteral("Menge")]) / 1000.0;
            else
                kg = toDouble(obj[QStringLiteral("Menge")]);
            values.clear();
            values[ModelMalzschuettung::ColSudID] = sudId;
            values[ModelMalzschuettung::ColName] = removeHtmlEntities(obj[QStringLiteral("Name")].toString());
            values[ModelMalzschuettung::ColProzent] = kg / gesamt_schuettung * 100.0;
            values[ModelMalzschuettung::ColFarbe] = 1;
            bh->modelMalzschuettung()->append(values);
        }
    }

    // Hopfen
    if (version == 1)
    {
        for (int i = 1; i < findMax(root, QStringLiteral("Hopfen_VWH_%%_Sorte")); ++i)
        {
            values.clear();
            values[ModelHopfengaben::ColSudID] = sudId;
            values[ModelHopfengaben::ColName] = removeHtmlEntities(root[QStringLiteral("Hopfen_VWH_%1_Sorte").arg(i)].toString());
            values[ModelHopfengaben::Colerg_Menge] = toDouble(root[QStringLiteral("Hopfen_VWH_%1_Menge").arg(i)]);
            values[ModelHopfengaben::ColZeit] = toDouble(root[QStringLiteral("Kochzeit_Wuerze")]);
            values[ModelHopfengaben::ColAlpha] = toDouble(root[QStringLiteral("Hopfen_VWH_%1_alpha").arg(i)]);
            values[ModelHopfengaben::ColPellets] = 1;
            values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Vorderwuerze);
            bh->modelHopfengaben()->append(values);
        }
        for (int i = 1; i < findMax(root, QStringLiteral("Hopfen_%%_Sorte")); ++i)
        {
            values.clear();
            values[ModelHopfengaben::ColSudID] = sudId;
            values[ModelHopfengaben::ColName] = removeHtmlEntities(root[QStringLiteral("Hopfen_%1_Sorte").arg(i)].toString());
            values[ModelHopfengaben::Colerg_Menge] = toDouble(root[QStringLiteral("Hopfen_%1_Menge").arg(i)]);
            values[ModelHopfengaben::ColZeit] = toDouble(root[QStringLiteral("Hopfen_%1_Kochzeit").arg(i)]);
            values[ModelHopfengaben::ColAlpha] = toDouble(root[QStringLiteral("Hopfen_%1_alpha").arg(i)]);
            values[ModelHopfengaben::ColPellets] = 1;
            values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Kochen);
            bh->modelHopfengaben()->append(values);
        }
    }
    else if (version == 2)
    {
        array = root[QStringLiteral("Hopfenkochen")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            values.clear();
            values[ModelHopfengaben::ColSudID] = sudId;
            values[ModelHopfengaben::ColName] = removeHtmlEntities(obj[QStringLiteral("Sorte")].toString());
            values[ModelHopfengaben::Colerg_Menge] = toDouble(obj[QStringLiteral("Menge")]);
            values[ModelHopfengaben::ColZeit] = toDouble(obj[QStringLiteral("Zeit")]);
            values[ModelHopfengaben::ColAlpha] = toDouble(obj[QStringLiteral("Alpha")]);
            values[ModelHopfengaben::ColPellets] = 1;
            QString typ = obj[QStringLiteral("Typ")].toString();
            if (typ == QStringLiteral("Vorderwuerze"))
                values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Vorderwuerze);
            else if (typ == QStringLiteral("Whirlpool"))
                values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Ausschlagen);
            else
                values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Kochen);
            bh->modelHopfengaben()->append(values);
        }
    }

    // Hefe
    const QStringList hefen = removeHtmlEntities(root[QStringLiteral("Hefe")].toString()).split(QStringLiteral(", "));
    for (const QString& hefe : hefen)
    {
        values.clear();
        values[ModelHefegaben::ColSudID] = sudId;
        values[ModelHefegaben::ColName] = hefe;
        values[ModelHefegaben::ColMenge] = qCeil(menge / 20.0 / hefen.size());
        bh->modelHefegaben()->append(values);
    }

    // Weitere Zutaten
    if (version == 1)
    {
        for (int i = 1; i < findMax(root, QStringLiteral("WeitereZutat_Wuerze_%%_Name")); ++i)
        {
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(root[QStringLiteral("WeitereZutat_Wuerze_%1_Name").arg(i)].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QStringLiteral("WeitereZutat_Wuerze_%1_Menge").arg(i)]) / menge;
            if (root[QStringLiteral("WeitereZutat_Wuerze_%1_Einheit").arg(i)].toString() == QStringLiteral("g"))
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            else
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = toDouble(root[QStringLiteral("WeitereZutat_Wuerze_%1_Kochzeit").arg(i)]);
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }
        for (int i = 1; i < findMax(root, QStringLiteral("WeitereZutat_Gaerung_%%_Name")); ++i)
        {
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(root[QStringLiteral("WeitereZutat_Gaerung_%1_Name").arg(i)].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QStringLiteral("WeitereZutat_Gaerung_%1_Menge").arg(i)]) / menge;
            if (root[QStringLiteral("WeitereZutat_Gaerung_%1_Einheit").arg(i)].toString() == QStringLiteral("g"))
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            else
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = 0;
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }
        for (int i = 1; i < findMax(root, QStringLiteral("Stopfhopfen_%%_Sorte")); ++i)
        {
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(root[QStringLiteral("Stopfhopfen_%1_Sorte").arg(i)].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(root[QStringLiteral("Stopfhopfen_%1_Menge").arg(i)]) / menge;
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = 7200;
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }
    }
    else if (version == 2)
    {
        array = root[QStringLiteral("Gewuerze_etc")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(obj[QStringLiteral("Name")].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(obj[QStringLiteral("Menge")]) / menge;
            if (obj[QStringLiteral("Einheit")].toString() == QStringLiteral("g"))
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            else
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = toDouble(obj[QStringLiteral("Kochzeit")]);
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }

        array = root[QStringLiteral("WeitereZutatGaerung")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(obj[QStringLiteral("Name")].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(obj[QStringLiteral("Menge")]) / menge;
            if (obj[QStringLiteral("Einheit")].toString() == QStringLiteral("g"))
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            else
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = 0;
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }

        array = root[QStringLiteral("Stopfhopfen")].toArray();
        for (QJsonArray::const_iterator it = array.constBegin(); it != array.constEnd(); ++it)
        {
            obj = it->toObject();
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = removeHtmlEntities(obj[QStringLiteral("Sorte")].toString());
            values[ModelWeitereZutatenGaben::ColMenge] = toDouble(obj[QStringLiteral("Menge")]) / menge;
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen);
            values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = 7200;
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
            bh->modelWeitereZutatenGaben()->append(values);
        }
    }

    bh->modelSud()->update(sudRow);
    return sudRow;
}

int ImportExport::importBeerXml(Brauhelfer* bh, const QByteArray &content)
{
    int sudRow = -1;
    const QString BeerXmlVersion = QStringLiteral("1");

    QString xmlError;
    QDomDocument doc(QStringLiteral(""));
    doc.setContent(content, &xmlError);
    if (!xmlError.isEmpty())
    {
        qWarning(Brauhelfer::loggingCategory) << "Failed to parse XML:" << xmlError;
        return -1;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != QStringLiteral("RECIPES"))
        return -1;

    for (QDomNode nRecipe = root.firstChildElement(QStringLiteral("RECIPE")); !nRecipe.isNull(); nRecipe = nRecipe.nextSiblingElement(QStringLiteral("RECIPE")))
    {
        if (nRecipe.firstChildElement(QStringLiteral("VERSION")).text() != BeerXmlVersion)
            continue;

        QDomNode nStyle = nRecipe.firstChildElement(QStringLiteral("STYLE"));
        QDomNode nMash = nRecipe.firstChildElement(QStringLiteral("MASH"));
        QDomNode nMashSteps = nMash.firstChildElement(QStringLiteral("MASH_STEPS"));

        double min, max;
        double menge = nRecipe.firstChildElement(QStringLiteral("BATCH_SIZE")).text().toDouble();

        QMap<int, QVariant> values;
        values[ModelSud::ColSudname] = nRecipe.firstChildElement(QStringLiteral("NAME")).text();
        values[ModelSud::ColMenge] = menge;
        min = nStyle.firstChildElement(QStringLiteral("OG_MIN")).text().toDouble();
        max = nStyle.firstChildElement(QStringLiteral("OG_MAX")).text().toDouble();
        values[ModelSud::ColSW] = BierCalc::dichteToPlato((min+max)/2);
        values[ModelSud::ColFaktorHauptguss] = 3.5;
        min = nStyle.firstChildElement(QStringLiteral("CARB_MIN")).text().toDouble();
        max = nStyle.firstChildElement(QStringLiteral("CARB_MAX")).text().toDouble();
        values[ModelSud::ColCO2] = (min+max)/2;
        min = nStyle.firstChildElement(QStringLiteral("IBU_MIN")).text().toDouble();
        max = nStyle.firstChildElement(QStringLiteral("IBU_MAX")).text().toDouble();
        values[ModelSud::ColIBU] = (min+max)/2;
        values[ModelSud::ColberechnungsArtHopfen] = static_cast<int>(Brauhelfer::BerechnungsartHopfen::Keine);
        values[ModelSud::ColKochdauer] = nRecipe.firstChildElement(QStringLiteral("BOIL_TIME")).text().toDouble();

        sudRow = bh->modelSud()->append(values);
        bh->modelSud()->update(sudRow);
        int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

        // Rasten
        double V_tot = bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble();
        values.clear();
        values[ModelMaischplan::ColSudID] = sudId;
        values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Einmaischen);
        values[ModelMaischplan::ColName] = "Einmaischen";
        values[ModelMaischplan::ColAnteilWasser] = 100;
        values[ModelMaischplan::ColAnteilMalz] = 100;
        values[ModelMaischplan::ColTempRast] = nMashSteps.firstChildElement(QStringLiteral("MASH_STEP")).firstChildElement(QStringLiteral("STEP_TEMP")).text().toDouble();
        values[ModelMaischplan::ColDauerRast] = 10;
        bh->modelMaischplan()->appendDirect(values);
        for (QDomNode n = nMashSteps.firstChildElement(QStringLiteral("MASH_STEP")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("MASH_STEP")))
        {
            QString typ = n.firstChildElement(QStringLiteral("TYP")).text().toLower();
            values.clear();
            values[ModelMaischplan::ColSudID] = sudId;
            values[ModelMaischplan::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
            values[ModelMaischplan::ColTempRast] = n.firstChildElement(QStringLiteral("STEP_TEMP")).text().toDouble();
            values[ModelMaischplan::ColDauerRast] = n.firstChildElement(QStringLiteral("STEP_TIME")).text().toDouble();
            if (typ == QStringLiteral("temperature"))
            {
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Aufheizen);
            }
            else if (typ == QStringLiteral("infusion"))
            {
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Zubruehen);
                values[ModelMaischplan::ColAnteilWasser] = 100 * n.firstChildElement(QStringLiteral("INFUSE_AMOUNT")).text().toDouble() / V_tot;
            }
            else if (typ == QStringLiteral("decoction"))
            {
                values[ModelMaischplan::ColTyp] = static_cast<int>(Brauhelfer::RastTyp::Dekoktion);
            }
            bh->modelMaischplan()->appendDirect(values);
        }

        // Malzschuettung
        QDomNode nMalz = nRecipe.firstChildElement(QStringLiteral("FERMENTABLES"));
        double gesamt_malz = 0.0;
        for (QDomNode n = nMalz.firstChildElement(QStringLiteral("FERMENTABLE")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("FERMENTABLE")))
        {
            if (n.firstChildElement(QStringLiteral("TYPE")).text() == QStringLiteral("Grain"))
                gesamt_malz += n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble();
        }
        for (QDomNode n = nMalz.firstChildElement(QStringLiteral("FERMENTABLE")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("FERMENTABLE")))
        {
            QString type = n.firstChildElement(QStringLiteral("TYPE")).text();
            if (type == QStringLiteral("Grain"))
            {
                values.clear();
                values[ModelMalzschuettung::ColSudID] = sudId;
                values[ModelMalzschuettung::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
                values[ModelMalzschuettung::ColFarbe] = n.firstChildElement(QStringLiteral("COLOR")).text().toDouble() * 1.97;
                values[ModelMalzschuettung::ColProzent] = n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble() / gesamt_malz * 100;
                bh->modelMalzschuettung()->append(values);
            }
            else
            {
                values.clear();
                values[ModelWeitereZutatenGaben::ColSudID] = sudId;
                values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
                values[ModelWeitereZutatenGaben::ColFarbe] = n.firstChildElement(QStringLiteral("COLOR")).text().toDouble() * 1.97;
                values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble() / menge;
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::Kg);
                if (type == QStringLiteral("Sugar") || type == QStringLiteral("Extract") || type == QStringLiteral("Dry Extract"))
                    values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Zucker);
                if (n.firstChildElement(QStringLiteral("ADD_AFTER_BOIL")).text() == QStringLiteral("TRUE"))
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
                }
                else
                {
                    values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
                    values[ModelWeitereZutatenGaben::ColZugabedauer] = nRecipe.firstChildElement(QStringLiteral("BOIL_TIME")).text().toDouble();
                }
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
                values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement(QStringLiteral("NOTES")).text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
        }

        // Hopfen
        QDomNode nHops = nRecipe.firstChildElement(QStringLiteral("HOPS"));
        for (QDomNode n = nHops.firstChildElement(QStringLiteral("HOP")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("HOP")))
        {
            QString use = n.firstChildElement(QStringLiteral("USE")).text();
            if (use == QStringLiteral("Dry Hop"))
            {
                values.clear();
                values[ModelWeitereZutatenGaben::ColSudID] = sudId;
                values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
                values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble() * 1000 / menge;
                values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Hopfen);
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
                values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement(QStringLiteral("TIME")).text().toDouble();
                values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::MitEntnahme);
                values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement(QStringLiteral("NOTES")).text();
                bh->modelWeitereZutatenGaben()->append(values);
            }
            else
            {
                values.clear();
                values[ModelHopfengaben::ColSudID] = sudId;
                values[ModelHopfengaben::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
                values[ModelHopfengaben::Colerg_Menge] = n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble();
                values[ModelHopfengaben::ColZeit]  = n.firstChildElement(QStringLiteral("TIME")).text().toDouble();
                values[ModelHopfengaben::ColAlpha] = n.firstChildElement(QStringLiteral("ALPHA")).text().toDouble();
                values[ModelHopfengaben::ColPellets] = n.firstChildElement(QStringLiteral("FORM")).text() == QStringLiteral("Pellet");
                if (use == QStringLiteral("First Wort"))
                    values[ModelHopfengaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::HopfenZeitpunkt::Vorderwuerze);
                bh->modelHopfengaben()->append(values);
            }
        }

        // Hefe
        QDomNode nYeasts = nRecipe.firstChildElement(QStringLiteral("YEASTS"));
        for (QDomNode n = nYeasts.firstChildElement(QStringLiteral("YEAST")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("YEAST")))
        {
            values.clear();
            values[ModelHefegaben::ColSudID] = sudId;
            values[ModelHefegaben::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
            values[ModelHefegaben::ColMenge] = 0;
            bh->modelHefegaben()->append(values);
        }

        QDomNode nMiscs = nRecipe.firstChildElement(QStringLiteral("MISCS"));
        for (QDomNode n = nMiscs.firstChildElement(QStringLiteral("MISC")); !n.isNull(); n = n.nextSiblingElement(QStringLiteral("MISC")))
        {
            values.clear();
            values[ModelWeitereZutatenGaben::ColSudID] = sudId;
            values[ModelWeitereZutatenGaben::ColName] = n.firstChildElement(QStringLiteral("NAME")).text();
            QString type = n.firstChildElement(QStringLiteral("TYPE")).text();
            if (type == QStringLiteral("Spice"))
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz);
            else if (type == QStringLiteral("Herb"))
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Kraut);
            else if (type == QStringLiteral("Flavor"))
                values[ModelWeitereZutatenGaben::ColTyp] = static_cast<int>(Brauhelfer::ZusatzTyp::Frucht);
            QString use = n.firstChildElement(QStringLiteral("USE")).text();
            if (use == QStringLiteral("Boil"))
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Kochen);
            else if (use == QStringLiteral("Mash"))
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Maischen);
            else
                values[ModelWeitereZutatenGaben::ColZeitpunkt] = static_cast<int>(Brauhelfer::ZusatzZeitpunkt::Gaerung);
            values[ModelWeitereZutatenGaben::ColMenge] = n.firstChildElement(QStringLiteral("AMOUNT")).text().toDouble() * 1000 / menge;
            values[ModelWeitereZutatenGaben::ColEinheit] = static_cast<int>(Brauhelfer::Einheit::g);
            values[ModelWeitereZutatenGaben::ColZugabedauer] = n.firstChildElement(QStringLiteral("TIME")).text().toDouble();
            values[ModelWeitereZutatenGaben::ColEntnahmeindex] = static_cast<int>(Brauhelfer::ZusatzEntnahmeindex::OhneEntnahme);
            values[ModelWeitereZutatenGaben::ColBemerkung] = n.firstChildElement(QStringLiteral("NOTES")).text();
            bh->modelWeitereZutatenGaben()->append(values);
        }
    }

    bh->modelSud()->update(sudRow);
    return sudRow;
}

QByteArray ImportExport::exportKbh(Brauhelfer* bh, int sudRow, const QStringList& exclude, bool compact)
{
    QJsonObject root;
    QJsonArray array;
    SqlTableModel* model;
    ProxyModel proxy;
    QVariantList list;

    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();
    QRegularExpression regExpId(QStringLiteral("^%1$").arg(sudId));

    root[QStringLiteral("Global")] = QJsonObject({{"db_Version", bh->databaseVersion()}});

    model = bh->modelSud();
    if (!exclude.contains(model->tableName()))
    {
        root[model->tableName()] = QJsonObject::fromVariantMap(model->toVariantMap(sudRow, QList<int>(), {ModelSud::ColID}));
    }

    model = bh->modelMaischplan();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelMaischplan::ColSudID);
        proxy.setFilterRegularExpression(regExpId);
        list.clear();
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelMaischplan::ColID, ModelMaischplan::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelAnhang::ColID, ModelAnhang::ColSudID}));
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    model = bh->modelTags();
    if (!exclude.contains(model->tableName()))
    {
        proxy.setSourceModel(model);
        proxy.setFilterKeyColumn(ModelTags::ColSudID);
        proxy.setFilterRegularExpression(QRegularExpression(QStringLiteral("^(%1|-.*)$").arg(sudId)));
        list.clear();
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
        {
            QVariantMap values = model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelTags::ColID, ModelTags::ColSudID});
            values[QStringLiteral("Global")] = proxy.data(row, ModelTags::ColGlobal).toBool();
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
        list.reserve(proxy.rowCount());
        for (int row = 0; row < proxy.rowCount(); ++row)
            list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelWasseraufbereitung::ColID, ModelWasseraufbereitung::ColSudID}));
        array = QJsonArray::fromVariantList(list);
        if (!array.isEmpty())
            root[model->tableName()] = array;
    }

    QJsonDocument doc(root);
    return doc.toJson(compact ? QJsonDocument::JsonFormat::Compact : QJsonDocument::JsonFormat::Indented);
}

QByteArray ImportExport::exportBrautomat(Brauhelfer* bh, int sudRow, bool compact)
{
    QJsonObject root;
    SqlTableModel* model;
    ProxyModel proxy;
    QVariantList list;
    QJsonArray array;

    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();
    QRegularExpression regExpId(QStringLiteral("^%1$").arg(sudId));

    model = bh->modelSud();
    root[model->tableName()] = QJsonObject::fromVariantMap(model->toVariantMap(sudRow,
                                                                               {ModelSud::ColSudname,
                                                                                ModelSud::ColKochdauer,
                                                                                ModelSud::ColNachisomerisierungszeit,
                                                                                ModelSud::Colerg_S_Gesamt,
                                                                                ModelSud::Colerg_WHauptguss,
                                                                                ModelSud::Colerg_WNachguss}));

    model = bh->modelMaischplan();
    proxy.setSourceModel(model);
    proxy.setFilterKeyColumn(ModelMaischplan::ColSudID);
    proxy.setFilterRegularExpression(regExpId);
    list.clear();
    list.reserve(proxy.rowCount());
    for (int row = 0; row < proxy.rowCount(); ++row)
    {
        QVariantMap map = model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelMaischplan::ColID, ModelMaischplan::ColSudID});
        if (map[QStringLiteral("Param1")].toInt() == 0)
            map.remove(QStringLiteral("Param1"));
        if (map[QStringLiteral("Param2")].toInt() == 0)
            map.remove(QStringLiteral("Param2"));
        if (map[QStringLiteral("Param3")].toInt() == 0)
            map.remove(QStringLiteral("Param3"));
        if (map[QStringLiteral("Param4")].toInt() == 0)
            map.remove(QStringLiteral("Param4"));
        list.append(map);
    }
    array = QJsonArray::fromVariantList(list);
    if (!array.isEmpty())
        root[model->tableName()] = array;

    model = bh->modelHopfengaben();
    proxy.setSourceModel(model);
    proxy.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    proxy.setFilterRegularExpression(regExpId);
    list.clear();
    list.reserve(proxy.rowCount());
    for (int row = 0; row < proxy.rowCount(); ++row)
        list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID}));
    array = QJsonArray::fromVariantList(list);
    if (!array.isEmpty())
        root[model->tableName()] = array;

    model = bh->modelWeitereZutatenGaben();
    proxy.setSourceModel(model);
    proxy.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    proxy.setFilterRegularExpression(regExpId);
    list.clear();
    list.reserve(proxy.rowCount());
    for (int row = 0; row < proxy.rowCount(); ++row)
        list.append(model->toVariantMap(proxy.mapRowToSource(row), QList<int>(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID}));
    array = QJsonArray::fromVariantList(list);
    if (!array.isEmpty())
        root[model->tableName()] = array;

    QJsonDocument doc(root);
    return doc.toJson(compact ? QJsonDocument::JsonFormat::Compact : QJsonDocument::JsonFormat::Indented);
}

QByteArray ImportExport::exportMaischeMalzundMehr(Brauhelfer *bh, int sudRow, bool compact)
{
    ProxyModel model;
    int n;
    bool dekoktion = false;
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    QJsonObject root;
    root[QStringLiteral("Name")] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
    root[QStringLiteral("Datum")] = bh->modelSud()->data(sudRow, ModelSud::ColErstellt).toDate().toString(QStringLiteral("dd.MM.yyyy"));
    root[QStringLiteral("Ausschlagswuerze")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1);
    root[QStringLiteral("Sudhausausbeute")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1);
    root[QStringLiteral("Stammwuerze")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble(), 'f', 1);
    root[QStringLiteral("Bittere")] = QString::number( bh->modelSud()->data(sudRow, ModelSud::ColIBU).toInt());
    root[QStringLiteral("Farbe")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toInt());
    root[QStringLiteral("Alkohol")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColAlkoholSoll).toDouble(), 'f', 1);
    root[QStringLiteral("Kurzbeschreibung")] = bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString();
    root[QStringLiteral("Endvergaerungsgrad")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toDouble(), 'f', 1);
    root[QStringLiteral("Karbonisierung")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
    root[QStringLiteral("Nachguss")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WNachguss).toDouble(), 'f', 1);
    root[QStringLiteral("Kochzeit_Wuerze")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());

    // Rasten
    model.setSourceModel(bh->modelMaischplan());
    model.setFilterKeyColumn(ModelMaischplan::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(row, ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            break;
        case Brauhelfer::RastTyp::Aufheizen:
            break;
        case Brauhelfer::RastTyp::Zubruehen:
            dekoktion = true;
            break;
        case Brauhelfer::RastTyp::Zuschuetten:
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
        root[QStringLiteral("Maischform")] = "dekoktion";
        root[QStringLiteral("Dekoktion_Einmaisch_Zubruehwasser_gesamt")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble(), 'f', 1);
    }
    else
    {
        root[QStringLiteral("Maischform")] = "infusion";
        root[QStringLiteral("Infusion_Hauptguss")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble(), 'f', 1);
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        switch (static_cast<Brauhelfer::RastTyp>(model.data(row, ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Einmaischen:
            if (!dekoktion)
            {
                root[QStringLiteral("Infusion_Einmaischtemperatur")] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                int dauer = model.data(row, ModelMaischplan::ColDauerRast).toInt();
                if (dauer > 0)
                {
                    root[QStringLiteral("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                    root[QStringLiteral("Infusion_Rastzeit%1").arg(n)] = QString::number(dauer);
                    ++n;
                }
            }
            else
            {
                root[QStringLiteral("Dekoktion_0_Volumen")] = QString::number(model.data(row, ModelMaischplan::ColMengeWasser).toInt());
                root[QStringLiteral("Dekoktion_0_Temperatur_ist")] = QString::number(model.data(row, ModelMaischplan::ColTempWasser).toInt());
                root[QStringLiteral("Dekoktion_0_Temperatur_resultierend")] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                root[QStringLiteral("Dekoktion_0_Rastzeit")] = QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt());
            }
            break;
        case Brauhelfer::RastTyp::Aufheizen:
            if (!dekoktion)
            {
                root[QStringLiteral("Infusion_Rasttemperatur%1").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                root[QStringLiteral("Infusion_Rastzeit%1").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt());
                ++n;
            }
            break;
        case Brauhelfer::RastTyp::Zubruehen:
            if (dekoktion)
            {
                root[QStringLiteral("Dekoktion_%1_Form").arg(n)] = "Kochendes Wasser";
                root[QStringLiteral("Dekoktion_%1_Volumen").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1);
                root[QStringLiteral("Dekoktion_%1_Temperatur_resultierend").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                root[QStringLiteral("Dekoktion_%1_Rastzeit").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt());
                ++n;
            }
            break;
        case Brauhelfer::RastTyp::Zuschuetten:
            if (dekoktion)
            {
                root[QStringLiteral("Dekoktion_%1_Form").arg(n)] = "Kochendes Wasser";
                root[QStringLiteral("Dekoktion_%1_Volumen").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1);
                root[QStringLiteral("Dekoktion_%1_Temperatur_resultierend").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                root[QStringLiteral("Dekoktion_%1_Rastzeit").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt());
                ++n;
            }
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            if (dekoktion)
            {
                root[QStringLiteral("Dekoktion_%1_Form").arg(n)] = "Dünnmaische";
                root[QStringLiteral("Dekoktion_%1_Volumen").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColMengeMaische).toDouble(), 'f', 1);
                root[QStringLiteral("Dekoktion_%1_Temperatur_resultierend").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt());
                root[QStringLiteral("Dekoktion_%1_Rastzeit").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt());
                root[QStringLiteral("Dekoktion_%1_Teilmaische_Temperatur").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColTempExtra2).toInt());
                root[QStringLiteral("Dekoktion_%1_Teilmaische_Kochzeit").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerExtra1).toInt());
                root[QStringLiteral("Dekoktion_%1_Teilmaische_Rastzeit").arg(n)] = QString::number(model.data(row, ModelMaischplan::ColDauerExtra2).toInt());
                ++n;
            }
            break;
        }
    }
    root[QStringLiteral("Abmaischtemperatur")] = "78";

    // Malzschuettung
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        root[QStringLiteral("Malz%1").arg(n)] = model.data(row, ModelMalzschuettung::ColName).toString();
        root[QStringLiteral("Malz%1_Menge").arg(n)] = QString::number(model.data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2);
        root[QStringLiteral("Malz%1_Einheit").arg(n)] = "kg";
        ++n;
    }

    // Hopfen
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
        {
            root[QStringLiteral("Hopfen_VWH_%1_Sorte").arg(n)] = model.data(row, ModelHopfengaben::ColName).toString();
            root[QStringLiteral("Hopfen_VWH_%1_Menge").arg(n)] = QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1);
            root[QStringLiteral("Hopfen_VWH_%1_alpha").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1);
            ++n;
        }
    }
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        if (static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) != Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
        {
            root[QStringLiteral("Hopfen_%1_Sorte").arg(n)] = model.data(row, ModelHopfengaben::ColName).toString();
            root[QStringLiteral("Hopfen_%1_Menge").arg(n)] = QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1);
            root[QStringLiteral("Hopfen_%1_alpha").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1);
            root[QStringLiteral("Hopfen_%1_Kochzeit").arg(n)] = QString::number(model.data(row, ModelHopfengaben::ColZeit).toInt());
            ++n;
        }
    }

    // Hefe
    QStringList hefen;
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(ModelHefegaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    hefen.reserve(model.rowCount());
    for (int row = 0; row < model.rowCount(); ++row)
    {
        QString hefe = model.data(row, ModelHefegaben::ColName).toString();
        if (!hefen.contains(hefe))
            hefen.append(hefe);
    }
    root[QStringLiteral("Hefe")] = hefen.join(QStringLiteral(", "));

    // Weitere Zutaten
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    n = 1;
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Maischen)
        {
            root[QStringLiteral("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
            {
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
            }
            root[QStringLiteral("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());
            ++n;
        }
        else if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Kochen)
        {
            root[QStringLiteral("WeitereZutat_Wuerze_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
            {
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Menge").arg(n)] = QString::number( model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Einheit").arg(n)] = "g";
                root[QStringLiteral("WeitereZutat_Wuerze_%1_Kochzeit").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt());
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
            root[QStringLiteral("Stopfhopfen_%1_Sorte").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            root[QStringLiteral("Stopfhopfen_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
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
            root[QStringLiteral("WeitereZutat_Gaerung_%1_Name").arg(n)] = model.data(row, ModelWeitereZutatenGaben::ColName).toString();
            Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            if (einheit == Brauhelfer::Einheit::Kg)
            {
                root[QStringLiteral("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2);
                root[QStringLiteral("WeitereZutat_Gaerung_%1_Einheit").arg(n)] = "kg";
            }
            else
            {
                root[QStringLiteral("WeitereZutat_Gaerung_%1_Menge").arg(n)] = QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt());
                root[QStringLiteral("WeitereZutat_Gaerung_%1_Einheit").arg(n)] = "g";
            }
            ++n;
        }
    }

    QJsonDocument doc(root);
    return doc.toJson(compact ? QJsonDocument::JsonFormat::Compact : QJsonDocument::JsonFormat::Indented);
}

QByteArray ImportExport::exportBeerXml(Brauhelfer* bh, int sudRow, bool compact)
{
    const QString BeerXmlVersion = QStringLiteral("1");

    double val;
    QString str;
    ProxyModel model;
    int sudId = bh->modelSud()->data(sudRow, ModelSud::ColID).toInt();

    QDomDocument doc;
    QDomText text;
    QDomElement element;
    QDomElement Anteil;

    QDomProcessingInstruction header = doc.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\" encoding=\"UTF-8\""));
    doc.appendChild(header);

    QDomElement Rezepte = doc.createElement(QStringLiteral("RECIPES"));
    doc.appendChild(Rezepte);

    QDomElement Rezept = doc.createElement(QStringLiteral("RECIPE"));
    Rezepte.appendChild(Rezept);

    element = doc.createElement(QStringLiteral("VERSION"));
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("NAME"));
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString().toHtmlEscaped());
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("TYPE"));
    text = doc.createTextNode(QStringLiteral("All Grain"));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("BREWER"));
    text = doc.createTextNode(QStringLiteral("kleiner-brauhelfer-2"));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("BATCH_SIZE"));
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("BOIL_SIZE"));
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("BOIL_TIME"));
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt()));
    element.appendChild(text);
    Rezept.appendChild(element);

    element = doc.createElement(QStringLiteral("EFFICIENCY"));
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1));
    element.appendChild(text);
    Rezept.appendChild(element);

    QDomElement style = doc.createElement(QStringLiteral("STYLE"));
    Rezept.appendChild(style);

    element = doc.createElement(QStringLiteral("VERSION"));
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("NAME"));
    text = doc.createTextNode(QStringLiteral("Unknown"));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("CATEGORY"));
    text = doc.createTextNode(QStringLiteral("Unknown"));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("CATEGORY_NUMBER"));
    text = doc.createTextNode(QStringLiteral("0"));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("STYLE_LETTER"));
    text = doc.createTextNode(QStringLiteral(""));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("STYLE_GUIDE"));
    text = doc.createTextNode(QStringLiteral(""));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("TYPE"));
    text = doc.createTextNode(QStringLiteral(""));
    element.appendChild(text);
    style.appendChild(element);

    val = BierCalc::platoToDichte(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble());
    element = doc.createElement(QStringLiteral("OG_MIN"));
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("OG_MAX"));
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    val *= 0.8;
    element = doc.createElement(QStringLiteral("FG_MIN"));
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("FG_MAX"));
    text = doc.createTextNode(QString::number(val, 'f', 4));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::ColIBU).toDouble();
    element = doc.createElement(QStringLiteral("IBU_MIN"));
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("IBU_MAX"));
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toDouble() * 0.508;
    element = doc.createElement(QStringLiteral("COLOR_MIN"));
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("COLOR_MAX"));
    text = doc.createTextNode(QString::number(val, 'f', 0));
    element.appendChild(text);
    style.appendChild(element);

    val = bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble();
    element = doc.createElement(QStringLiteral("CARB_MIN"));
    text = doc.createTextNode(QString::number(val, 'f', 1));
    element.appendChild(text);
    style.appendChild(element);

    element = doc.createElement(QStringLiteral("CARB_MAX"));
    text = doc.createTextNode(QString::number(val, 'f', 1));
    element.appendChild(text);
    style.appendChild(element);

    QDomElement Hopfengaben = doc.createElement(QStringLiteral("HOPS"));
    Rezept.appendChild(Hopfengaben);
    model.setSourceModel(bh->modelHopfengaben());
    model.setFilterKeyColumn(ModelHopfengaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement(QStringLiteral("HOP"));
        Hopfengaben.appendChild(Anteil);

        element = doc.createElement(QStringLiteral("VERSION"));
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("NAME"));
        text = doc.createTextNode(model.data(row, ModelHopfengaben::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("ALPHA"));
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("AMOUNT"));
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::Colerg_Menge).toDouble() / 1000, 'f', 4));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("USE"));
        text = doc.createTextNode(static_cast<Brauhelfer::HopfenZeitpunkt>(model.data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze ? QStringLiteral("First Wort") : QStringLiteral("Boil"));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("TIME"));
        text = doc.createTextNode(QString::number(model.data(row, ModelHopfengaben::ColZeit).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        Brauhelfer::HopfenTyp typ = Brauhelfer::HopfenTyp::Universal;
        int rowHopfen = bh->modelHopfen()->getRowWithValue(ModelHopfen::ColName, model.data(row, ModelHopfengaben::ColName));
        if (rowHopfen >= 0)
        {
             typ = static_cast<Brauhelfer::HopfenTyp>(bh->modelHopfen()->data(rowHopfen, ModelHopfen::ColTyp).toInt());
        }

        element = doc.createElement(QStringLiteral("TYPE"));
        switch (typ)
        {
        case Brauhelfer::HopfenTyp::Aroma:
            text = doc.createTextNode(QStringLiteral("Aroma"));
            break;
        case Brauhelfer::HopfenTyp::Bitter:
            text = doc.createTextNode(QStringLiteral("Bittering"));
            break;
        case Brauhelfer::HopfenTyp::Unbekannt:
        case Brauhelfer::HopfenTyp::Universal:
            text = doc.createTextNode(QStringLiteral("Both"));
            break;
        }
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("FORM"));
        text = doc.createTextNode(model.data(row, ModelHopfen::ColPellets).toBool() ? QStringLiteral("Pellet") : QStringLiteral("Leaf"));
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
        {
            Anteil = doc.createElement(QStringLiteral("HOP"));
            Hopfengaben.appendChild(Anteil);

            element = doc.createElement(QStringLiteral("VERSION"));
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("NAME"));
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("AMOUNT"));
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("USE"));
            text = doc.createTextNode(QStringLiteral("Dry Hop"));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("TIME"));
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
               element = doc.createElement(QStringLiteral("NOTES"));
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

            element = doc.createElement(QStringLiteral("ALPHA"));
            text = doc.createTextNode(QString::number(alpha, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("TYPE"));
            switch (typ)
            {
            case Brauhelfer::HopfenTyp::Aroma:
                text = doc.createTextNode(QStringLiteral("Aroma"));
                break;
            case Brauhelfer::HopfenTyp::Bitter:
                text = doc.createTextNode(QStringLiteral("Bittering"));
                break;
            case Brauhelfer::HopfenTyp::Unbekannt:
            case Brauhelfer::HopfenTyp::Universal:
                text = doc.createTextNode(QStringLiteral("Both"));
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("FORM"));
            text = doc.createTextNode(pellets ? QStringLiteral("Pellet") : QStringLiteral("Leaf"));
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QDomElement fermentables = doc.createElement(QStringLiteral("FERMENTABLES"));
    Rezept.appendChild(fermentables);
    model.setSourceModel(bh->modelMalzschuettung());
    model.setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement(QStringLiteral("FERMENTABLE"));
        fermentables.appendChild(Anteil);

        element = doc.createElement(QStringLiteral("VERSION"));
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("NAME"));
        text = doc.createTextNode(model.data(row, ModelMalzschuettung::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("TYPE"));
        text = doc.createTextNode(QStringLiteral("Grain"));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("AMOUNT"));
        text = doc.createTextNode(QString::number(model.data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("YIELD"));
        text = doc.createTextNode(QStringLiteral("77"));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("COLOR"));
        double ebc = model.data(row, ModelMalzschuettung::ColFarbe).toDouble();
        double srm = ebc * 0.508;
        double l = (srm + 0.76) / 1.3546;
        text = doc.createTextNode(QString::number(l));
        element.appendChild(text);
        Anteil.appendChild(element);
    }
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        double Ausbeute = model.data(row, ModelWeitereZutatenGaben::ColAusbeute).toDouble();
        if (Ausbeute > 0)
        {
            Anteil = doc.createElement(QStringLiteral("FERMENTABLE"));
            fermentables.appendChild(Anteil);

            element = doc.createElement(QStringLiteral("VERSION"));
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("NAME"));
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("TYPE"));
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
            switch (typ)
            {
            case Brauhelfer::ZusatzTyp::Honig:
            case Brauhelfer::ZusatzTyp::Zucker:
            case Brauhelfer::ZusatzTyp::Frucht:
                text = doc.createTextNode(QStringLiteral("Sugar"));
                break;
            case Brauhelfer::ZusatzTyp::Gewuerz:
            case Brauhelfer::ZusatzTyp::Sonstiges:
            case Brauhelfer::ZusatzTyp::Kraut:
            case Brauhelfer::ZusatzTyp::Wasseraufbereiung:
            case Brauhelfer::ZusatzTyp::Klaermittel:
            case Brauhelfer::ZusatzTyp::Hopfen:
                text = doc.createTextNode(QStringLiteral("Adjunct"));
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("AMOUNT"));
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("YIELD"));
            text = doc.createTextNode(QString::number(Ausbeute, 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("COLOR"));
            double ebc = model.data(row, ModelWeitereZutatenGaben::ColFarbe).toDouble();
            double srm = ebc * 0.508;
            double l = (srm + 0.76) / 1.3546;
            text = doc.createTextNode(QString::number(l));
            element.appendChild(text);
            Anteil.appendChild(element);

            Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
            if (zeitpunkt == Brauhelfer::ZusatzZeitpunkt::Gaerung)
            {
                element = doc.createElement(QStringLiteral("ADD_AFTER_BOIL"));
                text = doc.createTextNode(QStringLiteral("TRUE"));
                element.appendChild(text);
                Anteil.appendChild(element);
            }

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
                element = doc.createElement(QStringLiteral("NOTES"));
                text = doc.createTextNode(str);
                element.appendChild(text);
                Anteil.appendChild(element);
            }
        }
    }

    QDomElement yeasts = doc.createElement(QStringLiteral("YEASTS"));
    Rezept.appendChild(yeasts);
    model.setSourceModel(bh->modelHefegaben());
    model.setFilterKeyColumn(ModelHefegaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement(QStringLiteral("YEAST"));
        yeasts.appendChild(Anteil);

        element = doc.createElement(QStringLiteral("VERSION"));
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("NAME"));
        text = doc.createTextNode(model.data(row, ModelHefegaben::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("TYPE"));
        text = doc.createTextNode(QStringLiteral("Lager"));
        element.appendChild(text);
        Anteil.appendChild(element);

        bool liquid = false;
        int rowHefe = bh->modelHefe()->getRowWithValue(ModelHefe::ColName, model.data(row, ModelHefegaben::ColName));
        if (rowHefe >= 0)
        {
            Brauhelfer::HefeTyp typ = static_cast<Brauhelfer::HefeTyp>(bh->modelHefe()->data(rowHefe, ModelHefe::ColTypTrFl).toInt());
            liquid = typ == Brauhelfer::HefeTyp::Fluessig;
        }

        element = doc.createElement(QStringLiteral("FORM"));
        text = doc.createTextNode(liquid ? QStringLiteral("Liquid") : QStringLiteral("Dry"));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("AMOUNT"));
        text = doc.createTextNode(QStringLiteral("0"));
        element.appendChild(text);
        Anteil.appendChild(element);

        if (model.data(row, ModelHefegaben::ColZugabeNach).toInt() > 0)
        {
            element = doc.createElement(QStringLiteral("ADD_TO_SECONDARY"));
            text = doc.createTextNode(QStringLiteral("TRUE"));
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QDomElement miscs = doc.createElement(QStringLiteral("MISCS"));
    Rezept.appendChild(miscs);
    model.setSourceModel(bh->modelWeitereZutatenGaben());
    model.setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
        double Ausbeute = model.data(row, ModelWeitereZutatenGaben::ColAusbeute).toDouble();
        if (typ != Brauhelfer::ZusatzTyp::Hopfen && Ausbeute == 0)
        {
            Anteil = doc.createElement(QStringLiteral("MISC"));
            miscs.appendChild(Anteil);

            element = doc.createElement(QStringLiteral("VERSION"));
            text = doc.createTextNode(BeerXmlVersion);
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("NAME"));
            text = doc.createTextNode(model.data(row, ModelWeitereZutatenGaben::ColName).toString().toHtmlEscaped());
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("TYPE"));
            switch (typ)
            {
            case Brauhelfer::ZusatzTyp::Honig:
            case Brauhelfer::ZusatzTyp::Zucker:
                text = doc.createTextNode(QStringLiteral("Sugar"));
                break;
            case Brauhelfer::ZusatzTyp::Frucht:
                text = doc.createTextNode(QStringLiteral("Flavor"));
                break;
            case Brauhelfer::ZusatzTyp::Gewuerz:
                text = doc.createTextNode(QStringLiteral("Spice"));
                break;
            case Brauhelfer::ZusatzTyp::Kraut:
                text = doc.createTextNode(QStringLiteral("Herb"));
                break;
            case Brauhelfer::ZusatzTyp::Sonstiges:
            case Brauhelfer::ZusatzTyp::Wasseraufbereiung:
            case Brauhelfer::ZusatzTyp::Klaermittel:
                text = doc.createTextNode(QStringLiteral("Other"));
                break;
            case Brauhelfer::ZusatzTyp::Hopfen:
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("USE"));
            Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model.data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
            switch (zeitpunkt)
            {
            case Brauhelfer::ZusatzZeitpunkt::Gaerung:
                text = doc.createTextNode(QStringLiteral("Primary"));
                break;
            case Brauhelfer::ZusatzZeitpunkt::Kochen:
                text = doc.createTextNode(QStringLiteral("Boil"));
                break;
            case Brauhelfer::ZusatzZeitpunkt::Maischen:
                text = doc.createTextNode(QStringLiteral("Mash"));
                break;
            }
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("TIME"));
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt()));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("AMOUNT"));
            text = doc.createTextNode(QString::number(model.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 3));
            element.appendChild(text);
            Anteil.appendChild(element);

            element = doc.createElement(QStringLiteral("AMOUNT_IS_WEIGHT"));
            text = doc.createTextNode(QStringLiteral("TRUE"));
            element.appendChild(text);
            Anteil.appendChild(element);

            str = model.data(row, ModelWeitereZutatenGaben::ColBemerkung).toString().toHtmlEscaped();
            if (!str.isEmpty())
            {
                element = doc.createElement(QStringLiteral("NOTES"));
                text = doc.createTextNode(str);
                element.appendChild(text);
                Anteil.appendChild(element);
            }
        }
    }

    QDomElement waters = doc.createElement(QStringLiteral("WATERS"));
    Rezept.appendChild(waters);

    element = doc.createElement(QStringLiteral("VERSION"));
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("NAME"));
    text = doc.createTextNode(bh->modelSud()->data(sudRow, ModelSud::ColWasserprofil).toString().toHtmlEscaped());
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("AMOUNT"));
    text = doc.createTextNode(QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_W_Gesamt).toDouble(), 'f', 1));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("CALCIUM"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("BICARBONATE"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("SULFATE"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("CHLORIDE"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("SODIUM"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    element = doc.createElement(QStringLiteral("MAGNESIUM"));
    text = doc.createTextNode(QStringLiteral("0.0"));
    element.appendChild(text);
    waters.appendChild(element);

    QDomElement mash = doc.createElement(QStringLiteral("MASH"));
    Rezept.appendChild(mash);

    element = doc.createElement(QStringLiteral("VERSION"));
    text = doc.createTextNode(BeerXmlVersion);
    element.appendChild(text);
    mash.appendChild(element);

    element = doc.createElement(QStringLiteral("NAME"));
    text = doc.createTextNode(QStringLiteral("Temperatur"));
    element.appendChild(text);
    mash.appendChild(element);

    element = doc.createElement(QStringLiteral("GRAIN_TEMP"));
    text = doc.createTextNode(QStringLiteral("18"));
    element.appendChild(text);
    mash.appendChild(element);

    QDomElement mash_steps = doc.createElement(QStringLiteral("MASH_STEPS"));
    mash.appendChild(mash_steps);

    model.setSourceModel(bh->modelMaischplan());
    model.setFilterKeyColumn(ModelMaischplan::ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
    for (int row = 0; row < model.rowCount(); ++row)
    {
        Anteil = doc.createElement(QStringLiteral("MASH_STEP"));
        mash_steps.appendChild(Anteil);

        element = doc.createElement(QStringLiteral("VERSION"));
        text = doc.createTextNode(BeerXmlVersion);
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("NAME"));
        text = doc.createTextNode(model.data(row, ModelMaischplan::ColName).toString().toHtmlEscaped());
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("TYPE"));
        Brauhelfer::RastTyp typ = static_cast<Brauhelfer::RastTyp>(model.data(row, ModelMaischplan::ColTyp).toInt());
        switch (typ)
        {
        case Brauhelfer::RastTyp::Einmaischen:
            text = doc.createTextNode(QStringLiteral("Infusion"));
            break;
        case Brauhelfer::RastTyp::Aufheizen:
            text = doc.createTextNode(QStringLiteral("Temperature"));
            break;
        case Brauhelfer::RastTyp::Zubruehen:
            text = doc.createTextNode(QStringLiteral("Infusion"));
            break;
        case Brauhelfer::RastTyp::Zuschuetten:
            text = doc.createTextNode(QStringLiteral("Infusion"));
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            text = doc.createTextNode(QStringLiteral("Decoction"));
            break;
        }
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("STEP_TEMP"));
        text = doc.createTextNode(QString::number(model.data(row, ModelMaischplan::ColTempRast).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        element = doc.createElement(QStringLiteral("STEP_TIME"));
        text = doc.createTextNode(QString::number(model.data(row, ModelMaischplan::ColDauerRast).toInt()));
        element.appendChild(text);
        Anteil.appendChild(element);

        if (typ == Brauhelfer::RastTyp::Einmaischen || typ == Brauhelfer::RastTyp::Zubruehen || typ == Brauhelfer::RastTyp::Zuschuetten)
        {
            element = doc.createElement(QStringLiteral("INFUSE_AMOUNT"));
            text = doc.createTextNode(QString::number(model.data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1));
            element.appendChild(text);
            Anteil.appendChild(element);
        }
    }

    QString contentString;
    QTextStream stream(&contentString);
    doc.save(stream, compact ? 0 : 2, QDomNode::EncodingFromTextStream);
    return contentString.toUtf8();
}
