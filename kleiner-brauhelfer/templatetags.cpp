#include "templatetags.h"

#include <QLocale>
#include <QCoreApplication>
#include <QDir>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "tabrohstoffe.h"
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
            double fval, mengeMaischen, mengeKochbeginn100, mengeKochende100;
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
            ctxRezept["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAlkohol).toDouble(), 'f', 1);
            ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIBU).toInt());
            ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
            ctxRezept["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toInt());
            ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toDouble())).name();
            ctxRezept["Kochdauer"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());
            ival = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();
            if (ival > 0)
                ctxRezept["Nachisomerisierung"] = QString::number(ival);
            ival = bh->modelSud()->data(sudRow, ModelSud::ColhighGravityFaktor).toInt();
            if (ival > 0)
                ctxRezept["HighGravityFaktor"] = QString::number(ival);
            if (gSettings->module(Settings::ModuleWasseraufbereitung))
            {
                ctxRezept["Wasserprofil"] = bh->modelSud()->data(sudRow, ModelSud::ColWasserprofil).toString();
                ctxRezept["Restalkalitaet"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetSoll).toDouble(), 'f', 2);
            }
            ctxRezept["Reifezeit"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColReifezeit).toInt());
            mengeMaischen = bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble() + BierCalc::MalzVerdraengung * bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble();
            ctxRezept["MengeMaischen"] = locale.toString(mengeMaischen, 'f', 1);
            ctxRezept["Name"] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
            ctxRezept["Kategorie"] = bh->modelSud()->data(sudRow, ModelSud::ColKategorie).toString();
            ctxRezept["Nummer"] = bh->modelSud()->data(sudRow, ModelSud::ColSudnummer).toInt();
            ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString().replace("\n", "<br>");
            ctxRezept["Gesamtschuettung"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble(), 'f', 2);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble();
            ctxRezept["MengeKochbeginn"] = locale.toString(fval, 'f', 1);
            mengeKochbeginn100 = BierCalc::volumenWasser(20.0, 100.0, fval);
            ctxRezept["MengeKochbeginn100"] = locale.toString(mengeKochbeginn100, 'f', 1);
            fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochende).toDouble();
            ctxRezept["MengeKochende"] = locale.toString(fval, 'f', 1);
            mengeKochende100 = BierCalc::volumenWasser(20.0, 100.0, fval);
            ctxRezept["MengeKochende100"] = locale.toString(mengeKochende100, 'f', 1);
            ctxRezept["SWKochbeginn"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginn).toDouble(), 'f', 1);
            ctxRezept["SWKochbeginnMitWz"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginnMitWz).toDouble(), 'f', 1);
            ctxRezept["SWKochende"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochende).toDouble(), 'f', 1);
            ctxRezept["SWAnstellen"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollAnstellen).toDouble(), 'f', 1);
            ctxRezept["plato2brix"] = BierCalc::faktorPlatoToBrix;
            ctxRezept["SHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1);
            ctxRezept["EVG"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toInt());
            if (gSettings->module(Settings::ModuleAusruestung))
            {
                ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, ModelSud::ColAnlage).toString();
                ctxRezept["BrauanlageBemerkung"] = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColBemerkung).toString();
                double A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
                double h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Hoehe).toDouble();
                ctxRezept["MengeMaischenUnten"] = locale.toString(mengeMaischen / A, 'f', 1);
                ctxRezept["MengeMaischenOben"] = locale.toString(h - mengeMaischen / A, 'f', 1);
                A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColSudpfanne_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
                h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
                ctxRezept["MengeKochbeginn100Unten"] = locale.toString(mengeKochbeginn100 / A, 'f', 1);
                ctxRezept["MengeKochbeginn100Oben"] = locale.toString(h - mengeKochbeginn100 / A, 'f', 1);
                ctxRezept["MengeKochende100Unten"] = locale.toString(mengeKochende100 / A, 'f', 1);
                ctxRezept["MengeKochende100Oben"] = locale.toString(h - mengeKochende100 / A, 'f', 1);
            }
            ctx["Rezept"] = ctxRezept;
        }

        if (parts & TagSud)
        {
            QVariantMap ctxSud;
            Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->data(sudRow, ModelSud::ColStatus).toInt());
            ctxSud["Status"] = QString::number(static_cast<int>(status));
            switch (status)
            {
            case Brauhelfer::SudStatus::Rezept:
                ctxSud["StatusRezept"] = 1;
                break;
            case Brauhelfer::SudStatus::Gebraut:
                ctxSud["StatusGebraut"] = 1;
                break;
            case Brauhelfer::SudStatus::Abgefuellt:
                ctxSud["StatusAbgefuellt"] = 1;
                break;
            case Brauhelfer::SudStatus::Verbraucht:
                ctxSud["StatusVerbraucht"] = 1;
                break;
            }
            ctxSud["SW"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWIst).toDouble(), 'f', 1);
            ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMengeIst).toDouble(), 'f', 1);
            ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIbuIst).toInt());
            ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2Ist).toDouble(), 'f', 1);
            ctxSud["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toInt());
            ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toDouble())).name();
            ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColBraudatum).toDate(), QLocale::ShortFormat);
            ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAbfuelldatum).toDate(), QLocale::ShortFormat);
            ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Alkohol).toDouble(), 'f', 1);
            ctxSud["EVG"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColtEVG).toInt());
            ctxSud["SHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Sudhausausbeute).toDouble(), 'f', 1);
            ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_EffektiveAusbeute).toDouble(), 'f', 1);
            if (gSettings->module(Settings::ModuleWasseraufbereitung))
                ctxSud["Restalkalitaet"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetIst).toDouble(), 'f', 2);

            if (gSettings->module(Settings::ModuleBewertung))
            {
                int bewertung = bh->modelSud()->data(sudRow, ModelSud::ColBewertungMittel).toInt();
                ctxSud["Bewertung"] = QString::number(bewertung);
                if (bewertung > 0)
                {
                    if (bewertung > 5)
                        bewertung = 5;
                    QString s = "";
                    for (int i = 0; i < bewertung; i++)
                        s += "<img class='star' width='24'>";
                    for (int i = bewertung; i < 5; i++)
                        s += "<img class='star_grey' width='24'>";
                    ctxSud["BewertungSterne"] = s;
                }
            }

            ctx["Sud"] = ctxSud;
        }

        if (sudRow == bh->sud()->row())
        {
            if (parts & TagBraudaten)
            {
                int kochDauer = bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt();

                QVariantList liste;
                ProxyModel *model = bh->sud()->modelRasten();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    int temp;
                    QVariantMap map;
                    map["Name"] = model->data(row, ModelRasten::ColName).toString();
                    map["Temp"] = QString::number(model->data(row, ModelRasten::ColTemp).toInt());
                    map["Dauer"] = QString::number(model->data(row, ModelRasten::ColDauer).toInt());
                    map["Menge"] = locale.toString(model->data(row, ModelRasten::ColMenge).toDouble(), 'f', 1);
                    switch (static_cast<Brauhelfer::RastTyp>(model->data(row, ModelRasten::ColTyp).toInt()))
                    {
                    case Brauhelfer::RastTyp::Einmaischen:
                        map["Einmaischen"] = true;
                        map["WasserTemp"] = QString::number(model->data(row, ModelRasten::ColParam1).toInt());
                        break;
                    case Brauhelfer::RastTyp::Temperatur:
                        map["Rast"] = true;
                        break;
                    case Brauhelfer::RastTyp::Infusion:
                        map["Infusion"] = true;
                        map["WasserTemp"] = QString::number(model->data(row, ModelRasten::ColParam1).toInt());
                        break;
                    case Brauhelfer::RastTyp::Dekoktion:
                        map["Dekoktion"] = true;
                        temp = model->data(row, ModelRasten::ColParam2).toInt();
                        if (temp > 0)
                        {
                            map["TeilmaischeRastTemp"] = QString::number(model->data(row, ModelRasten::ColParam1).toInt());
                            map["TeilmaischeRastDauer"] = QString::number(temp);
                        }
                        temp = model->data(row, ModelRasten::ColParam4).toInt();
                        if (temp > 0)
                        {
                            map["TeilmaischeZusatzRastTemp"] = QString::number(model->data(row, ModelRasten::ColParam3).toInt());
                            map["TeilmaischeZusatzRastDauer"] = QString::number(temp);
                        }
                        break;
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctx["Rasten"] = QVariantMap({{"Liste", liste}});

                QVariantMap mapWasser;
                double f1 = 0.0, f2 = 0.0, f3 = 0.0;
                f1 = bh->sud()->geterg_WHauptguss();
                mapWasser["Hauptguss"] = locale.toString(f1, 'f', 2);
                if (gSettings->module(Settings::ModuleAusruestung))
                {
                    double A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
                    double h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Hoehe).toDouble();
                    mapWasser["HauptgussUnten"] = locale.toString(f1 / A, 'f', 1);
                    mapWasser["HauptgussOben"] = locale.toString(h - f1 / A, 'f', 1);
                }
                f2 = bh->sud()->geterg_WNachguss();
                mapWasser["Nachguss"] = locale.toString(f2, 'f', 2);
                if (bh->sud()->gethighGravityFaktor() != 0)
                {
                    f3 = bh->sud()->getWasserHgf();
                    mapWasser["Verduennung"] = locale.toString(f3, 'f', 2);
                }
                mapWasser["Gesamt"] = locale.toString(f1 + f2 + f3, 'f', 2);

                if (parts & TagWasseraufbereitung)
                {
                    if (gSettings->module(Settings::ModuleWasseraufbereitung))
                    {
                        liste.clear();
                        model = bh->sud()->modelWasseraufbereitung();
                        for (int row = 0; row < model->rowCount(); ++row)
                        {
                            QVariantMap map;
                            map["Name"] = model->data(row, ModelWasseraufbereitung::ColName);
                            map["Einheit"] = TabRohstoffe::Einheiten[model->data(row, ModelWasseraufbereitung::ColEinheit).toInt()];
                            double m = model->data(row, ModelWasseraufbereitung::ColMenge).toDouble();
                            map["MengeHauptguss"] = locale.toString(m * f1, 'f', 2);
                            map["MengeNachguss"] = locale.toString(m * f2, 'f', 2);
                            if (bh->sud()->gethighGravityFaktor() != 0)
                                map["MengeVerduennung"] = locale.toString(m * f3, 'f', 2);
                            map["Menge"] = locale.toString(m * (f1 + f2 + f3), 'f', 2);
                            liste << map;
                        }
                        if (!liste.empty())
                            mapWasser["Wasseraufbereitung"] = QVariantMap({{"Liste", liste}});
                    }
                }

                if (parts & TagWasserprofil)
                {
                    if (gSettings->module(Settings::ModuleWasseraufbereitung))
                    {
                        QVariantMap map;
                        map["Name"] = bh->sud()->getWasserData(ModelWasser::ColName).toString();
                        QString str = bh->sud()->getWasserData(ModelWasser::ColBemerkung).toString();
                        if (!str.isEmpty())
                            map["Bemerkung"] = str;
                        mapWasser["Wasserprofil"] = map;
                    }
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
                    int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map["Name"], ModelHopfen::ColTyp).toInt();
                    if (idx >= 0 && idx < TabRohstoffe::HopfenTypname.count())
                        map.insert("Typ", TabRohstoffe::HopfenTypname[idx]);
                    map.insert("Prozent", locale.toString(model->data(row, ModelHopfengaben::ColProzent).toDouble(), 'f', 1));
                    map.insert("Menge", locale.toString(model->data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1));
                    map.insert("Kochdauer", QString::number(dauer));
                    map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                    map.insert("Alpha", locale.toString(model->data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
                    if (static_cast<Brauhelfer::HopfenZeitpunkt>(model->data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
                    {
                        map.insert("Vorderwuerze", true);
                        ctxZutaten["HatVorderwuerzehopfen"] = true;
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
                    int menge = model->data(row, ModelHefegaben::ColMenge).toInt();
                    if (menge <= 0)
                        continue;
                    QVariantMap map;
                    map.insert("Name", model->data(row, ModelHefegaben::ColName));
                    int idx = bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, map["Name"], ModelHefe::ColTypOGUG).toInt();
                    if (idx >= 0 && idx < TabRohstoffe::HefeTypname.count())
                        map.insert("Typ", TabRohstoffe::HefeTypname[idx]);
                    map.insert("Menge", menge);
                    if (model->data(row, ModelHefegaben::ColZugegeben).toBool())
                        map.insert("Status", QObject::tr("zugegeben"));
                    else
                        map.insert("Status", QObject::tr("nicht zugegeben"));
                    int zugabeNach = model->data(row, ModelHefegaben::ColZugabeNach).toInt();
                    if (zugabeNach > 0)
                    {
                        map.insert("ZugabeNach", zugabeNach);
                        map.insert("ZugabeDatum", locale.toString(model->data(row, ModelHefegaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten["Hefe"] = QVariantMap({{"Liste", liste}});

                QVariantList listeZusatz;
                QVariantList listeHopfen;
                model = bh->sud()->modelWeitereZutatenGaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    int ival;
                    map.insert("Name", model->data(row, ModelWeitereZutatenGaben::ColName));
                    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model->data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
                    map.insert("Einheit", TabRohstoffe::Einheiten[static_cast<int>(einheit)]);
                    switch (einheit)
                    {
                    case Brauhelfer::Einheit::Kg:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::g:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    case Brauhelfer::Einheit::mg:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::Stk:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    case Brauhelfer::Einheit::l:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::ml:
                        map.insert("Menge", locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    }
                    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model->data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
                    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
                    Brauhelfer::ZusatzEntnahmeindex entnahmeindex = static_cast<Brauhelfer::ZusatzEntnahmeindex>(model->data(row, ModelWeitereZutatenGaben::ColEntnahmeindex).toInt());
                    Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(model->data(row, ModelWeitereZutatenGaben::ColZugabestatus).toInt());
                    switch (zeitpunkt)
                    {
                    case Brauhelfer::ZusatzZeitpunkt::Gaerung:
                        map.insert("Gaerung", true);
                        map.insert("ZugabeNach", QString::number(model->data(row, ModelWeitereZutatenGaben::ColZugabeNach).toInt()));
                        map.insert("ZugabeDatum", locale.toString(model->data(row, ModelWeitereZutatenGaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
                        map.insert("EntnahmeDatum", locale.toString(model->data(row, ModelWeitereZutatenGaben::ColEntnahmeDatum).toDate(), QLocale::ShortFormat));
                        if (entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme)
                        {
                            map.insert("Entnahme", true);
                            map.insert("Dauer", model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt() / 1440);
                        }
                        switch (status)
                        {
                        case Brauhelfer::ZusatzStatus::NichtZugegeben:
                            map.insert("Status", QObject::tr("nicht zugegeben"));
                            break;
                        case Brauhelfer::ZusatzStatus::Zugegeben:
                            if (entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme)
                                map.insert("Status", QObject::tr("nicht entnommen"));
                            else
                                map.insert("Status", QObject::tr("zugegeben"));
                            break;
                        case Brauhelfer::ZusatzStatus::Entnommen:
                            map.insert("Status", QObject::tr("entnommen"));
                            break;
                        }
                        if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                            ctxZutaten["HatKalthopfen"] = true;
                        else
                            ctxZutaten["HatZusatzGaerung"] = true;
                        break;
                    case Brauhelfer::ZusatzZeitpunkt::Kochen:
                        map.insert("Kochen", true);
                        ival =  model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt();
                        map.insert("Kochdauer", ival);
                        map.insert("ZugabeNach", QString::number(kochDauer - ival));
                        ctxZutaten["HatZusatzKochen"] = true;
                        break;
                    case Brauhelfer::ZusatzZeitpunkt::Maischen:
                        map.insert("Maischen", true);
                        ctxZutaten["HatZusatzMaischen"] = true;
                        break;
                    }
                    QString str = model->data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
                    if (!str.isEmpty())
                        map.insert("Bemerkung", str);
                    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                    {
                        int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map["Name"], ModelHopfen::ColTyp).toInt();
                        map.insert("Typ", TabRohstoffe::HopfenTypname[idx]);
                        listeHopfen << map;
                    }
                    else
                    {
                        map.insert("Typ", TabRohstoffe::ZusatzTypname[static_cast<int>(typ)]);
                        listeZusatz << map;
                    }
                }
                if (!listeZusatz.empty())
                    ctxZutaten["Zusatz"] = QVariantMap({{"Liste", listeZusatz}});
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
            modelGeraete.setFilterRegularExpression(QString("^%1$").arg(anlageId.toInt()));
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
            modelAnhang.setFilterRegularExpression(QString("^%1$").arg(sudId));
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
        QVariantMap ctxTagsGlobal;
        ProxyModel modelTags;
        modelTags.setSourceModel(bh->modelTags());
        modelTags.setFilterKeyColumn(ModelTags::ColSudID);
        modelTags.setFilterRegularExpression(QString("^(%1|-.*)$").arg(sudId));
        for (int row = 0; row < modelTags.rowCount(); ++row)
        {
            QString t = modelTags.data(row, ModelTags::ColKey).toString();
            QString v = modelTags.data(row, ModelTags::ColValue).toString();
            t = t.simplified().replace(" ", "_");
            if (!t.isEmpty())
            {
                if (modelTags.data(row, ModelTags::ColGlobal).toBool())
                    ctxTagsGlobal[t] = v;
                else
                    ctxTags[t] = v;
            }
        }
        ctx["Tags"] = QVariantMap({{"ListeGlobal", ctxTagsGlobal}, {"Liste", ctxTags}});
    }
}
