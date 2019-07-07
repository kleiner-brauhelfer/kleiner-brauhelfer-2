#include "templatetags.h"

#include <QLocale>
#include <QCoreApplication>
#include <QDir>
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/wdganhang.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

void TemplateTags::render(WdgWebViewEditable* view, TagParts parts, int sudRow)
{
    render(view, parts, nullptr, sudRow);
}

void TemplateTags::render(WdgWebViewEditable* view, TagParts parts, std::function<void(QVariantMap &)> fnc, int sudRow)
{
    view->mTemplateTags.clear();
    erstelleTagListe(view->mTemplateTags, parts, sudRow);
    if (fnc)
        fnc(view->mTemplateTags);
    view->updateTags();
    view->updateHtml();
}

void TemplateTags::erstelleTagListe(QVariantMap &ctx, TagParts parts, int sudRow)
{
    int sudId = sudRow >= 0 ? bh->modelSud()->data(sudRow, "ID").toInt() : -1;
    QLocale locale = QLocale();

    ctx["Style"] = gSettings->theme() == Settings::Dark ? "style_dunkel.css" : "style_hell.css";

    if (parts & TagApp)
    {
        QVariantMap ctxApp;
        ctxApp["Name"] = QCoreApplication::applicationName();
        ctxApp["Version"] = QCoreApplication::applicationVersion();
        ctx["App"] = ctxApp;
    }

    if (sudRow >= 0)
    {
        if (parts & TagRezept)
        {
            double val;
            QVariantMap ctxRezept;
            ctxRezept["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SW").toDouble(), 'f', 1);
            ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1);
            ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IBU").toInt());
            ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2").toDouble(), 'f', 1);
            ctxRezept["Farbe"] = QString::number(bh->modelSud()->data(sudRow, "erg_Farbe").toInt());
            ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble())).name();
            ctxRezept["Kochdauer"] = QString::number(bh->modelSud()->data(sudRow, "KochdauerNachBitterhopfung").toInt());
            ctxRezept["Nachisomerisierung"] = QString::number(bh->modelSud()->data(sudRow, "Nachisomerisierungszeit").toInt());
            ctxRezept["HighGravityFaktor"] = QString::number(bh->modelSud()->data(sudRow, "HighGravityFaktor").toInt());
            ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, "Anlage").toString();
            ctxRezept["Name"] = bh->modelSud()->data(sudRow, "Sudname").toString();
            ctxRezept["Nummer"] = QString::number(bh->modelSud()->data(sudRow, "Sudnummer").toInt());
            ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, "Kommentar").toString().replace("\n", "<br>");
            ctxRezept["Gesamtschuettung"] = locale.toString(bh->modelSud()->data(sudRow, "erg_S_Gesamt").toDouble(), 'f', 2);
            ctxRezept["EinmaischenTemp"] = QString::number(bh->modelSud()->data(sudRow, "EinmaischenTemp").toInt());
            val = bh->modelSud()->data(sudRow, "MengeSollKochbeginn").toDouble();
            ctxRezept["MengeKochbeginn"] = locale.toString(val, 'f', 1);
            ctxRezept["MengeKochbeginn100"] = locale.toString(BierCalc::volumenWasser(20.0, 100.0, val), 'f', 1);
            val = bh->modelSud()->data(sudRow, "MengeSollKochende").toDouble();
            ctxRezept["MengeKochende"] = locale.toString(val, 'f', 1);
            ctxRezept["MengeKochende100"] = locale.toString(BierCalc::volumenWasser(20.0, 100.0, val), 'f', 1);
            ctxRezept["SWKochbeginn"] = locale.toString(bh->modelSud()->data(sudRow, "SWSollKochbeginn").toDouble(), 'f', 1);
            ctxRezept["SWKochende"] = locale.toString(bh->modelSud()->data(sudRow, "SWSollKochende").toDouble(), 'f', 1);
            ctxRezept["SWAnstellen"] = locale.toString(bh->modelSud()->data(sudRow, "SWSollAnstellen").toDouble(), 'f', 1);
            ctx["Rezept"] = ctxRezept;
        }

        if (parts & TagSud)
        {
            QVariantMap ctxSud;
            int status = bh->modelSud()->data(sudRow, "Status").toInt();
            ctxSud["Status"] = QString::number(status);
            ctxSud["StatusRezept"] = status == Sud_Status_Rezept ? "1" : "";
            ctxSud["StatusGebraut"] = status == Sud_Status_Gebraut ? "1" : "";
            ctxSud["StatusAbgefuellt"] = status == Sud_Status_Abgefuellt ? "1" : "";
            ctxSud["StatusVerbraucht"] = status == Sud_Status_Verbraucht ? "1" : "";
            ctxSud["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SWIst").toDouble(), 'f', 1);
            ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "MengeIst").toDouble(), 'f', 1);
            ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IbuIst").toInt());
            ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2Ist").toDouble(), 'f', 1);
            ctxSud["Farbe"] = QString::number(bh->modelSud()->data(sudRow, "FarbeIst").toInt());
            ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "FarbeIst").toDouble())).name();
            ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, "Braudatum").toDate(), QLocale::ShortFormat);
            ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, "Abfuelldatum").toDate(), QLocale::ShortFormat);
            ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Alkohol").toDouble(), 'f', 1);
            ctxSud["tEVG"] = locale.toString(bh->modelSud()->data(sudRow, "tEVG").toDouble(), 'f', 1);
            ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, "erg_EffektiveAusbeute").toDouble(), 'f', 1);

            int bewertung = bh->modelSud()->data(sudRow, "BewertungMax").toInt();
            ctxSud["Bewertung"] = QString::number(bewertung);
            if (bewertung > 0)
            {
                if (bewertung > Bewertung_MaxSterne)
                    bewertung = Bewertung_MaxSterne;
                QString s = "";
                for (int i = 0; i < bewertung; i++)
                    s += "<img class='star' width='24'>";
                for (int i = bewertung; i < Bewertung_MaxSterne; i++)
                    s += "<img class='star_grey' width='24'>";
                ctxSud["BewertungSterne"] = s;
            }

            ctx["Sud"] = ctxSud;
        }

        if (sudRow == bh->sud()->row())
        {
            if (parts & TagBraudaten)
            {
                int kochDauer = bh->modelSud()->data(sudRow, "KochdauerNachBitterhopfung").toInt();

                QVariantList liste;
                ProxyModel *model = bh->sud()->modelRasten();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map["Name"] = model->data(row, "Name").toString();
                    map["Temp"] = QString::number(model->data(row, "Temp").toInt());
                    map["Dauer"] = QString::number(model->data(row, "Dauer").toInt());
                    liste << map;
                }
                if (!liste.empty())
                    ctx["Rasten"] = QVariantMap({{"Liste", liste}});

                QVariantMap mapWasser;
                double f1 = 0.0, f2 = 0.0, f3 = 0.0;
                f1 = bh->sud()->geterg_WHauptguss();
                mapWasser["Hauptguss"] = locale.toString(f1, 'f', 1);
                f2 = bh->sud()->geterg_WNachguss();
                mapWasser["Nachguss"] = locale.toString(f2, 'f', 1);
                if (bh->sud()->gethighGravityFaktor() != 0)
                {
                    f3 = bh->sud()->getMenge() - bh->sud()->getMengeSollKochende();
                    mapWasser["Verduennung"] = locale.toString(f3, 'f', 1);
                }
                mapWasser["Gesamt"] = locale.toString(f1 + f2 + f3, 'f', 1);
                double restalkalitaetFaktor = bh->sud()->getRestalkalitaetFaktor();
                if (restalkalitaetFaktor > 0)
                {
                    mapWasser["HauptgussMilchsaeure"] = QString::number((int)(f1 * restalkalitaetFaktor));
                    mapWasser["NachgussMilchsaeure"] = QString::number((int)(f2 * restalkalitaetFaktor));
                    if (bh->sud()->gethighGravityFaktor() != 0)
                        mapWasser["VerduennungMilchsaeure"] = QString::number((int)(f3 * restalkalitaetFaktor));
                    mapWasser["GesamtMilchsaeure"] = QString::number((int)((f1 + f2 + f3) * restalkalitaetFaktor));
                }
                ctx["Wasser"] = mapWasser;


                QVariantMap ctxZutaten;

                liste.clear();
                model = bh->sud()->modelMalzschuettung();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert("Name", model->data(row, "Name"));
                    map.insert("Prozent", locale.toString(model->data(row, "Prozent").toDouble(), 'f', 1));
                    map.insert("Menge", locale.toString(model->data(row, "erg_Menge").toDouble(), 'f', 2));
                    map.insert("Farbe", QString::number(model->data(row, "Farbe").toInt()));
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Malz"] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHopfengaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    int dauer = model->data(row, "Zeit").toInt();
                    int duaerIsomerisierung = bh->modelSud()->data(sudRow, "Nachisomerisierungszeit").toInt();
                    map.insert("Name", model->data(row, "Name"));
                    map.insert("Prozent", locale.toString(model->data(row, "Prozent").toDouble(), 'f', 1));
                    map.insert("Menge", QString::number(model->data(row, "erg_Menge").toInt()));
                    map.insert("Kochdauer", QString::number(dauer));
                    map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                    map.insert("Alpha", locale.toString(model->data(row, "Alpha").toDouble(), 'f', 1));
                    if (model->data(row, "Vorderwuerze").toBool())
                    {
                        map.insert("Vorderwuerze", true);
                        ctxZutaten["ZutatenVorderwuerze"] = true;
                    }
                    else if (dauer == -duaerIsomerisierung)
                    {
                        map.insert("ZugabeNach", QString::number(-dauer));
                        map.insert("Ausschlagen", true);
                    }
                    else if (dauer <= 0)
                    {
                        map.insert("ZugabeNach", QString::number(-dauer));
                        map.insert("Nachisomerisierung", true);
                    }
                    else
                        map.insert("Kochen", true);
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Hopfen"] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHefegaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert("Name", model->data(row, "Name"));
                    map.insert("Menge", model->data(row, "Menge").toInt());
                    if (model->data(row, "Zugegeben").toBool())
                        map.insert("Status", QObject::tr("zugegeben"));
                    else
                        map.insert("Status", QObject::tr("nicht zugegeben"));
                    map.insert("ZugabeNach", QString::number(model->data(row, "ZugabeNach").toInt()));
                    map.insert("ZugabeDatum", locale.toString(model->data(row, "ZugabeDatum").toDate(), QLocale::ShortFormat));
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Hefe"] = QVariantMap({{"Liste", liste}});

                QVariantList listeHonig;
                QVariantList listeZucker;
                QVariantList listeGewuerz;
                QVariantList listeFrucht;
                QVariantList listeSonstiges;
                QVariantList listeHopfen;
                model = bh->sud()->modelWeitereZutatenGaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    int ival;

                    map.insert("Name", model->data(row, "Name"));
                    if (model->data(row, "Einheit").toInt() == EWZ_Einheit_Kg)
                    {
                        map.insert("Menge", locale.toString(model->data(row, "erg_Menge").toDouble() / 1000, 'f', 2));
                        map.insert("Einheit", QObject::tr("kg"));
                    }
                    else
                    {
                        map.insert("Menge", QString::number(model->data(row, "erg_Menge").toInt()));
                        map.insert("Einheit", QObject::tr("g"));
                    }
                    switch (model->data(row, "Zeitpunkt").toInt())
                    {
                    case EWZ_Zeitpunkt_Gaerung:
                        map.insert("Gaerung", true);
                        map.insert("ZugabeNach", QString::number(model->data(row, "ZugabeNach").toInt()));
                        map.insert("ZugabeDatum", locale.toString(model->data(row, "ZugabeDatum").toDate(), QLocale::ShortFormat));
                        map.insert("EntnahmeDatum", locale.toString(model->data(row, "EntnahmeDatum").toDate(), QLocale::ShortFormat));
                        if (model->data(row, "Entnahmeindex").toInt() == EWZ_Entnahmeindex_MitEntnahme)
                        {
                            map.insert("Entnahme", true);
                            map.insert("Dauer", model->data(row, "Zugabedauer").toInt() / 1440);
                        }
                        switch (model->data(row, "ZugabeStatus").toInt())
                        {
                        case EWZ_Zugabestatus_nichtZugegeben:
                            map.insert("Status", QObject::tr("nicht zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Zugegeben:
                            if (model->data(row, "Entnahmeindex").toInt() == EWZ_Entnahmeindex_MitEntnahme)
                                map.insert("Status", QObject::tr("nicht entnommen"));
                            else
                                map.insert("Status", QObject::tr("zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Entnommen:
                            map.insert("Status", QObject::tr("entnommen"));
                            break;
                        }
                        if (model->data(row, "Typ").toInt() == EWZ_Typ_Hopfen)
                            ctxZutaten["ZutatenHopfenstopfen"] = true;
                        else
                            ctxZutaten["ZutatenGaerung"] = true;
                        break;
                    case EWZ_Zeitpunkt_Kochen:
                        map.insert("Kochen", true);
                        ival =  model->data(row, "Zugabedauer").toInt();
                        map.insert("Kochdauer", ival);
                        map.insert("ZugabeNach", QString::number(kochDauer - ival));
                        ctxZutaten["ZutatenKochen"] = true;
                        break;
                    case EWZ_Zeitpunkt_Maischen:
                        map.insert("Maischen", true);
                        ctxZutaten["ZutatenMaischen"] = true;
                        break;
                    }
                    map.insert("Bemerkung", model->data(row, "Bemerkung").toString());
                    switch (model->data(row, "Typ").toInt())
                    {
                    case EWZ_Typ_Honig:
                        listeHonig << map;
                        break;
                    case EWZ_Typ_Zucker:
                        listeZucker << map;
                        break;
                    case EWZ_Typ_Gewuerz:
                        listeGewuerz << map;
                        break;
                    case EWZ_Typ_Frucht:
                        listeFrucht << map;
                        break;
                    case EWZ_Typ_Sonstiges:
                        listeSonstiges << map;
                        break;
                    case EWZ_Typ_Hopfen:
                        listeHopfen << map;
                        break;
                    }
                }
                if (!listeHonig.empty())
                    ctxZutaten["Honig"] = QVariantMap({{"Liste", listeHonig}});
                if (!listeZucker.empty())
                    ctxZutaten["Zucker"] = QVariantMap({{"Liste", listeZucker}});
                if (!listeGewuerz.empty())
                    ctxZutaten["Gewuerz"] = QVariantMap({{"Liste", listeGewuerz}});
                if (!listeFrucht.empty())
                    ctxZutaten["Frucht"] = QVariantMap({{"Liste", listeFrucht}});
                if (!listeSonstiges.empty())
                    ctxZutaten["Sonstiges"] = QVariantMap({{"Liste", listeSonstiges}});
                if (!listeHopfen.empty())
                    ctxZutaten["Kalthopfen"] = QVariantMap({{"Liste", listeHopfen}});

                ctx["Zutaten"] = ctxZutaten;
            }
        }

        if (parts & TagGeraete)
        {
            QVariantList liste;
            QVariant anlageName = bh->modelSud()->data(sudRow, "Anlage");
            QVariant anlageId = bh->modelAusruestung()->getValueFromSameRow("Name", anlageName, "ID");
            ProxyModel modelGeraete;
            modelGeraete.setSourceModel(bh->modelGeraete());
            modelGeraete.setFilterKeyColumn(bh->modelGeraete()->fieldIndex("AusruestungAnlagenID"));
            modelGeraete.setFilterRegExp(QString("^%1$").arg(anlageId.toInt()));
            for (int row = 0; row < modelGeraete.rowCount(); ++row)
            {
                liste << QVariantMap({{"Name", modelGeraete.data(row, "Bezeichnung")}});
            }
            if (!liste.empty())
                ctx["Geraete"] = QVariantMap({{"Liste", liste}});
        }

        if (parts & TagAnhang)
        {
            QDir databasePath = QDir(gSettings->databaseDir());
            QVariantList liste;
            ProxyModel modelAnhang;
            modelAnhang.setSourceModel(bh->modelAnhang());
            modelAnhang.setFilterKeyColumn(bh->modelAnhang()->fieldIndex("SudID"));
            modelAnhang.setFilterRegExp(QString("^%1$").arg(sudId));
            for (int row = 0; row < modelAnhang.rowCount(); ++row)
            {
                QVariantMap map;
                QString pfad = modelAnhang.data(row, "Pfad").toString();
                if (QDir::isRelativePath(pfad))
                    pfad = QDir::cleanPath(databasePath.filePath(pfad));
                map.insert("Pfad", pfad);
                map.insert("Bild", WdgAnhang::isImage(pfad));
                liste << map;
            }
            if (!liste.empty())
                ctx["Anhang"] = QVariantMap({{"Liste", liste}});
        }
    }

    if (parts & TagTags)
    {
        QVariantMap ctxTags;
        ProxyModel modelFlaschenlabelTags;
        modelFlaschenlabelTags.setSourceModel(bh->modelFlaschenlabelTags());
        modelFlaschenlabelTags.setFilterKeyColumn(bh->modelFlaschenlabelTags()->fieldIndex("SudID"));
        modelFlaschenlabelTags.setFilterRegExp(QString("^(%1|-.*)$").arg(sudId));
        for (int row = 0; row < modelFlaschenlabelTags.rowCount(); ++row)
        {
            QString t = modelFlaschenlabelTags.data(row, "TagName").toString();
            QString v = modelFlaschenlabelTags.data(row, "Value").toString();
            if (!t.isEmpty())
                ctxTags[t] = v;
        }
        ctx["Tags"] = ctxTags;
    }
}
