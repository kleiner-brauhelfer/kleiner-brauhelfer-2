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
            QVariantMap ctxRezept;
            ctxRezept["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SW").toDouble(), 'f', 1);
            ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1);
            ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IBU").toDouble());
            ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2").toDouble(), 'f', 1);
            ctxRezept["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble(), 'f', 0);
            ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble())).name();
            ctxRezept["Nachisomerisierung"] = QString::number(bh->modelSud()->data(sudRow, "Nachisomerisierungszeit").toInt());
            ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, "Anlage").toString();
            ctxRezept["Name"] = bh->modelSud()->data(sudRow, "Sudname").toString();
            ctxRezept["Nummer"] = QString::number(bh->modelSud()->data(sudRow, "Sudnummer").toDouble());
            ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, "Kommentar").toString().replace("\n", "<br>");
            ctxRezept["Gesamtschuettung"] = locale.toString(bh->modelSud()->data(sudRow, "erg_S_Gesamt").toDouble(), 'f', 2);
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
            ctxSud["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "FarbeIst").toDouble(), 'f', 0);
            ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, "Braudatum").toDate(), QLocale::ShortFormat);
            ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, "Abfuelldatum").toDate(), QLocale::ShortFormat);
            ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "FarbeIst").toDouble())).name();
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
                f1 = bh->sud()->geterg_W_Gesamt();
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
                    mapWasser["HauptgussMilchsaeure"] = locale.toString(f1 * restalkalitaetFaktor, 'f', 0);
                    mapWasser["NachgussMilchsaeure"] = locale.toString(f2 * restalkalitaetFaktor, 'f', 0);
                    if (bh->sud()->gethighGravityFaktor() != 0)
                        mapWasser["VerduennungMilchsaeure"] = locale.toString(f3 * restalkalitaetFaktor, 'f', 0);
                    mapWasser["GesamtMilchsaeure"] = locale.toString((f1 + f2 + f3) * restalkalitaetFaktor, 'f', 0);
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
                    map.insert("Farbe", locale.toString(model->data(row, "Farbe").toDouble(), 'f', 0));
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Malz"] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHopfengaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert("Name", model->data(row, "Name"));
                    map.insert("Prozent", locale.toString(model->data(row, "Prozent").toDouble(), 'f', 1));
                    map.insert("Menge", locale.toString(model->data(row, "erg_Menge").toDouble(), 'f', 0));
                    map.insert("Kochdauer", locale.toString(model->data(row, "Zeit").toDouble(), 'f', 0));
                    map.insert("Alpha", locale.toString(model->data(row, "Alpha").toDouble(), 'f', 1));
                     map.insert("Vorderwuerze", model->data(row, "Vorderwuerze").toInt());
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
                    map.insert("Zugegeben", model->data(row, "Zugegeben").toInt());
                    map.insert("ZugabeNach", model->data(row, "ZugegebenNach").toInt());
                    map.insert("ZugabeZeitpunkt", model->data(row, "ZugabeZeitpunkt").toDateTime());
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

                    map.insert("Name", model->data(row, "Name"));
                    if (model->data(row, "Einheit").toInt() == EWZ_Einheit_Kg)
                    {
                        map.insert("Menge", locale.toString(model->data(row, "erg_Menge").toDouble() / 1000, 'f', 2));
                        map.insert("Einheit", QObject::tr("kg"));
                    }
                    else
                    {
                        map.insert("Menge", locale.toString(model->data(row, "erg_Menge").toDouble(), 'f', 0));
                        map.insert("Einheit", QObject::tr("g"));
                    }
                    switch (model->data(row, "Zeitpunkt").toInt())
                    {
                    case EWZ_Zeitpunkt_Gaerung:
                        map.insert("Gaerung", true);
                        map.insert("ZugabeNach", model->data(row, "ZugegebenNach").toInt());
                        map.insert("Dauer", model->data(row, "Zugabedauer").toInt() / 1440);
                        if (model->data(row, "Entnahmeindex").toInt() == EWZ_Entnahmeindex_MitEntnahme)
                            map.insert("Entnahme", true);
                        switch (model->data(row, "ZugabeStatus").toInt())
                        {
                        case EWZ_Zugabestatus_nichtZugegeben:
                            map.insert("Status", QObject::tr("nicht zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Zugegeben:
                            map.insert("Status", QObject::tr("zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Entnommen:
                            map.insert("Status", QObject::tr("entnommen"));
                            break;
                        }
                        ctxZutaten["ZutatenGaerung"] = true;
                        break;
                    case EWZ_Zeitpunkt_Kochen:
                        map.insert("Kochen", true);
                        map.insert("Kochdauer", model->data(row, "Zugabedauer").toInt());
                        map.insert("Von", model->data(row, "Zeitpunkt_von").toDateTime());
                        map.insert("Bis", model->data(row, "Zeitpunkt_bis").toDateTime());
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

        if (parts & TagAnhang)
        {

            QDir databasePath = QDir(gSettings->databaseDir());
            QVariantList liste;
            for (int row = 0; row < bh->modelAnhang()->rowCount(); ++row)
            {
                if (bh->modelAnhang()->data(row, "SudID").toInt() == sudId)
                {
                    QVariantMap map;
                    QString pfad = bh->modelAnhang()->data(row, "Pfad").toString();
                    if (QDir::isRelativePath(pfad))
                        pfad = QDir::cleanPath(databasePath.filePath(pfad));
                    map.insert("Pfad", pfad);
                    map.insert("Bild", WdgAnhang::isImage(pfad) ? "1" : "");
                    liste << map;
                }
            }
            if (!liste.empty())
                ctx["Anhang"] = QVariantMap({{"Liste", liste}});
        }
    }

    if (parts & TagTags)
    {
        QVariantMap ctxTags;
        for (int row = 0; row < bh->modelFlaschenlabelTags()->rowCount(); ++row)
        {
            int id = bh->modelFlaschenlabelTags()->data(row, "SudID").toInt();
            if (id == sudId || id < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(row, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(row, "Value").toString();
                if (!t.isEmpty())
                    ctxTags[t] = v;
            }
        }
        ctx["Tags"] = ctxTags;
    }
}
