#include "templatetags.h"

#include <QLocale>
#include <QCoreApplication>
#include <QDir>
#include <QTextDocument>
#include <qmath.h>
#include "brauhelfer.h"
#include "settings.h"
#include "units.h"
#include "mainwindow.h"
#include "widgets/wdganhang.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

void TemplateTags::render(WdgWebViewEditable* view, int sudRow)
{
    render(view, nullptr, sudRow);
}

void TemplateTags::render(WdgWebViewEditable* view, std::function<void(QVariantMap &)> fnc, int sudRow)
{
    view->mTemplateTags.clear();
    erstelleTagListe(view->mTemplateTags, sudRow);
    if (fnc)
        fnc(view->mTemplateTags);
    view->updateTags();
    view->updateWebView();
}

QString textToHtml(QString text)
{
    if (text.isEmpty())
        return text;
    if (Qt::mightBeRichText(text))
        return text;
    else
        return "<div style=\"white-space: pre-wrap;\">" + text + "</div>";
}

void TemplateTags::erstelleTagListe(QVariantMap &ctx, int sudRow)
{
    QVariantList liste;

    int sudId = sudRow >= 0 ? bh->modelSud()->data(sudRow, ModelSud::ColID).toInt() : -1;
    int anlageIndex = bh->modelAusruestung()->getRowWithValue(ModelAusruestung::ColName, bh->modelSud()->data(sudRow, ModelSud::ColAnlage));
    QLocale locale = QLocale();
    Units::Unit grvunit = Units::GravityUnit();

    // Style
    ctx["Style"] = gSettings->theme() == Settings::Dark ? "style_dunkel.css" : "style_hell.css";

    // App
    QVariantMap ctxApp;
    ctxApp["Name"] = QCoreApplication::applicationName();
    ctxApp["Version"] = QCoreApplication::applicationVersion();
    ctxApp["GravityUnit"] = Units::text(grvunit);
    ctx["App"] = ctxApp;

    if (sudRow >= 0)
    {
        // Rezept
        int ival;
        double fval, mengeMaischen, mengeKochbeginn100, mengeKochende100;
        QVariantMap ctxRezept;
        ctxRezept["SW"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble(), locale);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilMalz).toDouble();
        if (fval > 0)
            ctxRezept["SWAnteilMalz"] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzMaischen).toDouble();
        if (fval > 0)
            ctxRezept["SWAnteilZusatzMaischen"] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzKochen).toDouble();
        if (fval > 0)
            ctxRezept["SWAnteilZusatzKochen"] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzGaerung).toDouble();
        if (fval > 0)
            ctxRezept["SWAnteilZusatzGaerung"] = locale.toString(fval, 'f', 1);
        ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1);
        ctxRezept["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAlkoholSoll).toDouble(), 'f', 1);
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
        if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
        {
            ctxRezept["Wasserprofil"] = bh->modelSud()->data(sudRow, ModelSud::ColWasserprofil).toString();
            fval = bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetSoll).toDouble();
            if (fval > 0)
                ctxRezept["Restalkalitaet"] = locale.toString(fval, 'f', 2);
            ctxRezept["BemerkungWasseraufbereitung"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungWasseraufbereitung).toString());
        }
        ctxRezept["Reifezeit"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColReifezeit).toInt());
        mengeMaischen = bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble() + BierCalc::MalzVerdraengung * bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble();
        ctxRezept["MengeMaischen"] = locale.toString(mengeMaischen, 'f', 1);
        ctxRezept["Name"] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
        ctxRezept["Kategorie"] = bh->modelSud()->data(sudRow, ModelSud::ColKategorie).toString();
        ctxRezept["Nummer"] = bh->modelSud()->data(sudRow, ModelSud::ColSudnummer).toInt();
        ctxRezept["Kommentar"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString());
        ctxRezept["BemerkungMaischen"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenMaischen).toString());
        ctxRezept["BemerkungKochen"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenKochen).toString());
        ctxRezept["BemerkungGaerung"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenGaerung).toString());
        ctxRezept["BemerkungMaischplan"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungMaischplan).toString());
        ctxRezept["Gesamtschuettung"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble(), 'f', 2);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble();
        ctxRezept["MengeKochbeginn"] = locale.toString(fval, 'f', 1);
        mengeKochbeginn100 = BierCalc::volumenWasser(20.0, 100.0, fval);
        ctxRezept["MengeKochbeginn100"] = locale.toString(mengeKochbeginn100, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochende).toDouble();
        ctxRezept["MengeKochende"] = locale.toString(fval, 'f', 1);
        mengeKochende100 = BierCalc::volumenWasser(20.0, 100.0, fval);
        ctxRezept["MengeKochende100"] = locale.toString(mengeKochende100, 'f', 1);
        ctxRezept["SWKochbeginn"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginn).toDouble(), locale);
        ctxRezept["SWKochbeginnMitWz"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginnMitWz).toDouble(), locale);
        ctxRezept["SWKochende"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochende).toDouble(), locale);
        ctxRezept["SWAnstellen"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWSollAnstellen).toDouble(), locale);
        ctxRezept["SWKochbeginnBx"] = Units::convertStr(Units::Plato, Units::Brix, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginn).toDouble(), locale);
        ctxRezept["SWKochbeginnMitWzBx"] = Units::convertStr(Units::Plato, Units::Brix, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginnMitWz).toDouble(), locale);
        ctxRezept["SWKochendeBx"] = Units::convertStr(Units::Plato, Units::Brix, bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochende).toDouble(), locale);
        ctxRezept["SWAnstellenBx"] = Units::convertStr(Units::Plato, Units::Brix, bh->modelSud()->data(sudRow, ModelSud::ColSWSollAnstellen).toDouble(), locale);
        ctxRezept["SHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1);
        ctxRezept["EVG"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toInt());
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeHefestarter).toDouble();
        if (fval > 0)
        {
            ctxRezept["MengeHefestarter"] = locale.toString(fval, 'f', 1);
            ctxRezept["SWHefestarter"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWHefestarter).toDouble(), locale);
            ctxRezept["SWAnteilHefestarter"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilHefestarter).toDouble(), 'f', 1);
        }
        if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
        {
            if (bh->modelAusruestung()->rowCount() > 1)
                ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, ModelSud::ColAnlage).toString();
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

        // Sud
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
        ctxSud["SW"] = Units::convertStr(Units::Plato, grvunit, bh->modelSud()->data(sudRow, ModelSud::ColSWIst).toDouble(), locale);
        ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMengeIst).toDouble(), 'f', 1);
        ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIbuIst).toInt());
        ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2Ist).toDouble(), 'f', 1);
        ctxSud["Farbe"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toInt());
        ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toDouble())).name();
        ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColBraudatum).toDate(), QLocale::ShortFormat);
        ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAbfuelldatum).toDate(), QLocale::ShortFormat);
        ctxSud["ReifungStart"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColReifungStart).toDate(), QLocale::ShortFormat);
        ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Alkohol).toDouble(), 'f', 1);
        ctxSud["EVG"] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColsEVG).toInt());
        ctxSud["SHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Sudhausausbeute).toDouble(), 'f', 1);
        ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_EffektiveAusbeute).toDouble(), 'f', 1);
        ctxSud["BemerkungBrauen"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungBrauen).toString());
        ctxSud["BemerkungAbfuellen"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungAbfuellen).toString());
        ctxSud["BemerkungGaerung"] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungGaerung).toString());
        if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
        {
            ctxSud["Restalkalitaet"] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetIst).toDouble(), 'f', 2);
        }
        if (gSettings->isModuleEnabled(Settings::ModuleBewertung))
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

        if (sudRow == bh->sud()->row())
        {
                int kochDauer = bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt();
                int dauerIsomerisierung = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();

                liste.clear();
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
                mapWasser["Hauptguss"] = locale.toString(f1, 'f', 1);
                if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
                {
                    double A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
                    double h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Hoehe).toDouble();
                    mapWasser["HauptgussUnten"] = locale.toString(f1 / A, 'f', 1);
                    mapWasser["HauptgussOben"] = locale.toString(h - f1 / A, 'f', 1);
                }
                f2 = bh->sud()->geterg_WNachguss();
                mapWasser["Nachguss"] = locale.toString(f2, 'f', 1);
                if (bh->sud()->gethighGravityFaktor() != 0)
                {
                    f3 = bh->sud()->getMengeSollHgf();
                    mapWasser["Verduennung"] = locale.toString(f3, 'f', 2);
                }
                mapWasser["Gesamt"] = locale.toString(f1 + f2 + f3, 'f', 2);

                // Wasseraufbereitung
                if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
                {
                    liste.clear();
                    model = bh->sud()->modelWasseraufbereitung();
                    for (int row = 0; row < model->rowCount(); ++row)
                    {
                        QVariantMap map;
                        map["Name"] = model->data(row, ModelWasseraufbereitung::ColName);
                        map["Einheit"] = MainWindow::Einheiten[model->data(row, ModelWasseraufbereitung::ColEinheit).toInt()];
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

                // Wasserprofil
                if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
                {
                    QVariantMap map;
                    map["Name"] = bh->sud()->getWasserData(ModelWasser::ColName).toString();
                    QString str = bh->sud()->getWasserData(ModelWasser::ColBemerkung).toString();
                    if (!str.isEmpty())
                        map["Bemerkung"] = str;
                    mapWasser["Wasserprofil"] = map;
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
                    map.insert("Name", model->data(row, ModelHopfengaben::ColName));
                    int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map["Name"], ModelHopfen::ColTyp).toInt();
                    if (idx >= 0 && idx < MainWindow::HopfenTypname.count())
                        map.insert("Typ", MainWindow::HopfenTypname[idx]);
                    map.insert("Prozent", locale.toString(model->data(row, ModelHopfengaben::ColProzent).toDouble(), 'f', 1));
                    map.insert("Menge", locale.toString(model->data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1));
                    map.insert("Alpha", locale.toString(model->data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
                    if (static_cast<Brauhelfer::HopfenZeitpunkt>(model->data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
                    {
                        map.insert("Kochdauer", QString::number(dauer));
                        map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                        map.insert("Vorderwuerze", true);
                        ctxZutaten["HatVorderwuerzehopfen"] = true;
                    }
                    else if (dauer > 0)
                    {
                        map.insert("Kochdauer", QString::number(dauer));
                        map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                        map.insert("Kochen", true);
                    }
                    else
                    {
                        map.insert("Dauer", QString::number(dauerIsomerisierung+dauer));
                        map.insert("ZugabeNach", QString::number(-dauer));
                        map.insert("Ausschlagen", true);
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
                    if (idx >= 0 && idx < MainWindow::HefeTypname.count())
                        map.insert("Typ", MainWindow::HefeTypname[idx]);
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

                QVariantList listeZusatzAlle;
                QVariantList listeZusatzMaischen;
                QVariantList listeZusatzKochen;
                QVariantList listeZusatzAusschlagen;
                QVariantList listeZusatzGaerung;
                QVariantList listeKalthopfen;
                model = bh->sud()->modelWeitereZutatenGaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantList* listePtr = nullptr;
                    QVariantMap map;
                    int dauer;
                    map.insert("Name", model->data(row, ModelWeitereZutatenGaben::ColName));
                    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model->data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
                    map.insert("Einheit", MainWindow::Einheiten[static_cast<int>(einheit)]);
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
                            listePtr = &listeKalthopfen;
                        else
                            listePtr = &listeZusatzGaerung;
                        break;
                    case Brauhelfer::ZusatzZeitpunkt::Kochen:
                        dauer =  model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt();
                        if (dauer > 0)
                        {
                            map.insert("Kochdauer", QString::number(dauer));
                            map.insert("ZugabeNach", QString::number(kochDauer - dauer));
                            map.insert("Kochen", true);
                            listePtr = &listeZusatzKochen;
                        }
                        else
                        {
                            map.insert("Dauer", QString::number(dauerIsomerisierung+dauer));
                            map.insert("ZugabeNach", QString::number(-dauer));
                            map.insert("Ausschlagen", true);
                            listePtr = &listeZusatzAusschlagen;
                        }
                        break;
                    case Brauhelfer::ZusatzZeitpunkt::Maischen:
                        map.insert("Maischen", true);
                        listePtr = &listeZusatzMaischen;
                        break;
                    }
                    QString str = model->data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
                    if (!str.isEmpty())
                        map.insert("Bemerkung", str);
                    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                    {
                        int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map["Name"], ModelHopfen::ColTyp).toInt();
                        map.insert("Typ", MainWindow::HopfenTypname[idx]);
                    }
                    else
                    {
                        map.insert("Typ", MainWindow::ZusatzTypname[static_cast<int>(typ)]);
                        listeZusatzAlle << map;
                    }
                    if (listePtr)
                        *listePtr << map;
                }                
                if (!listeZusatzAlle.empty())
                    ctxZutaten["Zusatz"] = QVariantMap({{"Liste", listeZusatzAlle}});
                if (!listeZusatzMaischen.empty())
                    ctxZutaten["ZusatzMaischen"] = QVariantMap({{"Liste", listeZusatzMaischen}});
                if (!listeZusatzKochen.empty())
                    ctxZutaten["ZusatzKochen"] = QVariantMap({{"Liste", listeZusatzKochen}});
                if (!listeZusatzAusschlagen.empty())
                    ctxZutaten["ZusatzAusschlagen"] = QVariantMap({{"Liste", listeZusatzAusschlagen}});
                if (!listeZusatzGaerung.empty())
                    ctxZutaten["ZusatzGaerung"] = QVariantMap({{"Liste", listeZusatzGaerung}});
                if (!listeKalthopfen.empty())
                    ctxZutaten["Kalthopfen"] = QVariantMap({{"Liste", listeKalthopfen}});
                ctx["Zutaten"] = ctxZutaten;
        }

        // Anlage
        if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
        {
            QVariantMap ctxAnlage;
            QVariant anlageName = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColName).toString();
            QVariant anlageId = bh->modelAusruestung()->getValueFromSameRow(ModelAusruestung::ColName, anlageName, ModelAusruestung::ColID);
            ctxAnlage["Name"] = anlageName;
            ctxAnlage["KorrekturMenge"] = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColKorrekturMenge).toInt();
            ctxAnlage["Bemerkung"] = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColBemerkung).toString();
            liste.clear();
            ProxyModel modelGeraete;
            modelGeraete.setSourceModel(bh->modelGeraete());
            modelGeraete.setFilterKeyColumn(ModelGeraete::ColAusruestungAnlagenID);
            modelGeraete.setFilterRegularExpression(QString("^%1$").arg(anlageId.toInt()));
            for (int row = 0; row < modelGeraete.rowCount(); ++row)
            {
                liste << QVariantMap({{"Name", modelGeraete.data(row, ModelGeraete::ColBezeichnung)}});
            }
            if (!liste.empty())
                ctxAnlage["Geraete"] = QVariantMap({{"Liste", liste}});
            ctx["Anlage"] = ctxAnlage;
        }

        // Anhang
        QDir databasePath = QDir(gSettings->databaseDir());
        liste.clear();
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

    // Tags
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
