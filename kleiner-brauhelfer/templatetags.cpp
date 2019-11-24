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
    view->updateWebView();
}

void TemplateTags::erstelleTagListe(QVariantMap &ctx, TagParts parts, int sudRow)
{
    int sudId = sudRow >= 0 ? bh->modelSud()->data(sudRow, ModelSud::ColID).toInt() : -1;
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
            int ival;
            double fval;
            QVariantMap ctxRezept;
            ctxRezept["SW"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble(), 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColSW_Malz).toDouble();
            if (fval > 0)
                ctxRezept["SW_Malz"] = locale.toString(fval, 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColSW_WZ_Maischen).toDouble();
            if (fval > 0)
                ctxRezept["SW_WZ_Maischen"] = locale.toString(fval, 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColSW_WZ_Kochen).toDouble();
            if (fval > 0)
                ctxRezept["SW_WZ_Kochen"] = locale.toString(fval, 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColSW_WZ_Gaerung).toDouble();
            if (fval > 0)
                ctxRezept["SW_WZ_Gaerung"] = locale.toString(fval, 'f', 1);
            ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1);
            ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIBU).toInt());
            ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
            ctxRezept["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toInt());
            ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toDouble())).name();
            ctxRezept["Kochdauer"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauerNachBitterhopfung).toInt());
            ival = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();
            if (ival > 0)
                ctxRezept["Nachisomerisierung"] = QString::number(ival);
            ival = bh->modelSud()->data(sudRow, ModelSud::ColhighGravityFaktor).toInt();
            if (ival > 0)
                ctxRezept["HighGravityFaktor"] = QString::number(ival);
            ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, ModelSud::ColAnlage).toString();
            ctxRezept["Name"] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
            ctxRezept["Nummer"] = bh->modelSud()->data(sudRow, ModelSud::ColSudnummer).toInt();
            ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString().replace("\n", "<br>");
            ctxRezept["Gesamtschuettung"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble(), 'f', 2);
            ctxRezept["EinmaischenTemp"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColEinmaischenTemp).toInt());
            fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble();
            ctxRezept["MengeKochbeginn"] = locale.toString(fval, 'f', 1);
            ctxRezept["MengeKochbeginn100"] = locale.toString(BierCalc::volumenWasser(20.0, 100.0, fval), 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochende).toDouble();
            ctxRezept["MengeKochende"] = locale.toString(fval, 'f', 1);
            ctxRezept["MengeKochende100"] = locale.toString(BierCalc::volumenWasser(20.0, 100.0, fval), 'f', 1);
            ctxRezept["SWKochbeginn"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginn).toDouble(), 'f', 1);
            ctxRezept["SWKochbeginnMitWz"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginnMitWz).toDouble(), 'f', 1);
            ctxRezept["SWKochende"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochende).toDouble(), 'f', 1);
            ctxRezept["SWAnstellen"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollAnstellen).toDouble(), 'f', 1);
            ctx["Rezept"] = ctxRezept;
        }

        if (parts & TagSud)
        {
            QVariantMap ctxSud;
            int status = bh->modelSud()->data(sudRow, ModelSud::ColStatus).toInt();
            ctxSud["Status"] = QString::number(status);
            ctxSud["StatusRezept"] = status == Sud_Status_Rezept ? "1" : "";
            ctxSud["StatusGebraut"] = status == Sud_Status_Gebraut ? "1" : "";
            ctxSud["StatusAbgefuellt"] = status == Sud_Status_Abgefuellt ? "1" : "";
            ctxSud["StatusVerbraucht"] = status == Sud_Status_Verbraucht ? "1" : "";
            ctxSud["SW"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWIst).toDouble(), 'f', 1);
            ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMengeIst).toDouble(), 'f', 1);
            ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIbuIst).toInt());
            ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2Ist).toDouble(), 'f', 1);
            ctxSud["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toInt());
            ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toDouble())).name();
            ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColBraudatum).toDate(), QLocale::ShortFormat);
            ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAbfuelldatum).toDate(), QLocale::ShortFormat);
            ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Alkohol).toDouble(), 'f', 1);
            ctxSud["tEVG"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColtEVG).toDouble(), 'f', 1);
            ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_EffektiveAusbeute).toDouble(), 'f', 1);

            int bewertung = bh->modelSud()->data(sudRow, ModelSud::ColBewertungMittel).toInt();
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
                int kochDauer = bh->modelSud()->data(sudRow, ModelSud::ColKochdauerNachBitterhopfung).toInt();

                QVariantList liste;
                ProxyModel *model = bh->sud()->modelRasten();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map["Name"] = model->data(row, ModelRasten::ColName).toString();
                    map["Temp"] = QString::number(model->data(row, ModelRasten::ColTemp).toInt());
                    map["Dauer"] = QString::number(model->data(row, ModelRasten::ColDauer).toInt());
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
                    mapWasser["HauptgussMilchsaeure"] = QString::number(f1 * restalkalitaetFaktor, 'f', 1);
                    mapWasser["NachgussMilchsaeure"] = QString::number(f2 * restalkalitaetFaktor, 'f', 1);
                    if (bh->sud()->gethighGravityFaktor() != 0)
                        mapWasser["VerduennungMilchsaeure"] = QString::number(f3 * restalkalitaetFaktor, 'f', 1);
                    mapWasser["GesamtMilchsaeure"] = QString::number((f1 + f2 + f3) * restalkalitaetFaktor, 'f', 1);
                }
                ctx["Wasser"] = mapWasser;


                QVariantMap ctxZutaten;

                liste.clear();
                model = bh->sud()->modelMalzschuettung();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert("Name", model->data(row, ModelMalzschuettung::ColName));
                    map.insert("Prozent", locale.toString(model->data(row, ModelMalzschuettung::ColProzent).toDouble(), 'f', 1));
                    map.insert("Menge", locale.toString(model->data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2));
                    map.insert("Farbe", QString::number(model->data(row, ModelMalzschuettung::ColFarbe).toInt()));
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Malz"] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHopfengaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    int dauer = model->data(row, ModelHopfengaben::ColZeit).toInt();
                    int duaerIsomerisierung = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();
                    map.insert("Name", model->data(row, ModelHopfengaben::ColName));
                    map.insert("Prozent", locale.toString(model->data(row, ModelHopfengaben::ColProzent).toDouble(), 'f', 1));
                    map.insert("Menge", QString::number(model->data(row, ModelHopfengaben::Colerg_Menge).toInt()));
                    map.insert("Kochdauer", QString::number(dauer));
                    map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                    map.insert("Alpha", locale.toString(model->data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
                    if (model->data(row, ModelHopfengaben::ColVorderwuerze).toBool())
                    {
                        map.insert("Vorderwuerze", true);
                        ctxZutaten["ZutatenVorderwuerze"] = true;
                    }
                    else if (dauer == 0)
                    {
                        map.insert("Kochen", true);
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
                    {
                        map.insert("Kochen", true);
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Hopfen"] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHefegaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert("Name", model->data(row, ModelHefegaben::ColName));
                    map.insert("Menge", model->data(row, ModelHefegaben::ColMenge).toInt());
                    if (model->data(row, ModelHefegaben::ColZugegeben).toBool())
                        map.insert("Status", QObject::tr("zugegeben"));
                    else
                        map.insert("Status", QObject::tr("nicht zugegeben"));
                    map.insert("ZugabeNach", QString::number(model->data(row, ModelHefegaben::ColZugabeNach).toInt()));
                    map.insert("ZugabeDatum", locale.toString(model->data(row, ModelHefegaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
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

                    map.insert("Name", model->data(row, ModelWeitereZutatenGaben::ColName));
                    switch (model->data(row, ModelWeitereZutatenGaben::ColEinheit).toInt())
                    {
                    case EWZ_Einheit_Kg:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2));
                        map.insert("Einheit", QObject::tr("kg"));
                        break;
                    case EWZ_Einheit_g:
                        map.insert("Menge", QString::number(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toInt()));
                        map.insert("Einheit", QObject::tr("g"));
                        break;
                    case EWZ_Einheit_mg:
                        map.insert("Menge", QString::number((int)(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000)));
                        map.insert("Einheit", QObject::tr("mg"));
                        break;
                    case EWZ_Einheit_Stk:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 1));
                        map.insert("Einheit", QObject::tr("Stk."));
                        break;
                    }
                    switch (model->data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt())
                    {
                    case EWZ_Zeitpunkt_Gaerung:
                        map.insert("Gaerung", true);
                        map.insert("ZugabeNach", QString::number(model->data(row, ModelWeitereZutatenGaben::ColZugabeNach).toInt()));
                        map.insert("ZugabeDatum", locale.toString(model->data(row, ModelWeitereZutatenGaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
                        map.insert("EntnahmeDatum", locale.toString(model->data(row, ModelWeitereZutatenGaben::ColEntnahmeDatum).toDate(), QLocale::ShortFormat));
                        if (model->data(row, ModelWeitereZutatenGaben::ColEntnahmeindex).toInt() == EWZ_Entnahmeindex_MitEntnahme)
                        {
                            map.insert("Entnahme", true);
                            map.insert("Dauer", model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt() / 1440);
                        }
                        switch (model->data(row, ModelWeitereZutatenGaben::ColZugabestatus).toInt())
                        {
                        case EWZ_Zugabestatus_nichtZugegeben:
                            map.insert("Status", QObject::tr("nicht zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Zugegeben:
                            if (model->data(row, ModelWeitereZutatenGaben::ColEntnahmeindex).toInt() == EWZ_Entnahmeindex_MitEntnahme)
                                map.insert("Status", QObject::tr("nicht entnommen"));
                            else
                                map.insert("Status", QObject::tr("zugegeben"));
                            break;
                        case EWZ_Zugabestatus_Entnommen:
                            map.insert("Status", QObject::tr("entnommen"));
                            break;
                        }
                        if (model->data(row, ModelWeitereZutatenGaben::ColTyp).toInt() == EWZ_Typ_Hopfen)
                            ctxZutaten["ZutatenHopfenstopfen"] = true;
                        else
                            ctxZutaten["ZutatenGaerung"] = true;
                        break;
                    case EWZ_Zeitpunkt_Kochen:
                        map.insert("Kochen", true);
                        ival =  model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt();
                        map.insert("Kochdauer", ival);
                        map.insert("ZugabeNach", QString::number(kochDauer - ival));
                        ctxZutaten["ZutatenKochen"] = true;
                        break;
                    case EWZ_Zeitpunkt_Maischen:
                        map.insert("Maischen", true);
                        ctxZutaten["ZutatenMaischen"] = true;
                        break;
                    }
                    map.insert("Bemerkung", model->data(row, ModelWeitereZutatenGaben::ColBemerkung).toString());
                    switch (model->data(row, ModelWeitereZutatenGaben::ColTyp).toInt())
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
            QVariant anlageName = bh->modelSud()->data(sudRow, ModelSud::ColAnlage);
            QVariant anlageId = bh->modelAusruestung()->getValueFromSameRow(ModelAusruestung::ColName, anlageName, ModelAusruestung::ColID);
            ProxyModel modelGeraete;
            modelGeraete.setSourceModel(bh->modelGeraete());
            modelGeraete.setFilterKeyColumn(ModelGeraete::ColAusruestungAnlagenID);
            modelGeraete.setFilterRegExp(QString("^%1$").arg(anlageId.toInt()));
            for (int row = 0; row < modelGeraete.rowCount(); ++row)
            {
                liste << QVariantMap({{"Name", modelGeraete.data(row, ModelGeraete::ColBezeichnung)}});
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
            modelAnhang.setFilterKeyColumn(ModelAnhang::ColSudID);
            modelAnhang.setFilterRegExp(QString("^%1$").arg(sudId));
            for (int row = 0; row < modelAnhang.rowCount(); ++row)
            {
                QVariantMap map;
                QString pfad = modelAnhang.data(row, ModelAnhang::ColPfad).toString();
                if (QDir::isRelativePath(pfad))
                    pfad = QDir::cleanPath(databasePath.filePath(pfad));
                map.insert("Pfad", pfad);
                map.insert("Bild", WdgAnhang::isImage(pfad) && QFile::exists(pfad));
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
        modelFlaschenlabelTags.setFilterKeyColumn(ModelFlaschenlabelTags::ColSudID);
        modelFlaschenlabelTags.setFilterRegExp(QString("^(%1|-.*)$").arg(sudId));
        for (int row = 0; row < modelFlaschenlabelTags.rowCount(); ++row)
        {
            QString t = modelFlaschenlabelTags.data(row, ModelFlaschenlabelTags::ColTagname).toString();
            QString v = modelFlaschenlabelTags.data(row, ModelFlaschenlabelTags::ColValue).toString();
            if (!t.isEmpty())
                ctxTags[t] = v;
        }
        ctx["Tags"] = ctxTags;
    }
}
