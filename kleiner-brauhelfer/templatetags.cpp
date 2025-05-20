#include "templatetags.h"

#include <QLocale>
#include <QCoreApplication>
#include <QDir>
#include <QTextDocument>
#include <qmath.h>
#include "brauhelfer.h"
#include "biercalc.h"
#include "settings.h"
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

    // Style
    ctx[QStringLiteral("Style")] = gSettings->theme() == Qt::ColorScheme::Dark ? QStringLiteral("style_dunkel.css") : QStringLiteral("style_hell.css");

    // App
    QVariantMap ctxApp;
    ctxApp[QStringLiteral("Name")] = QCoreApplication::applicationName();
    ctxApp[QStringLiteral("Version")] = QCoreApplication::applicationVersion();
    ctx[QStringLiteral("App")] = ctxApp;

    if (sudRow >= 0)
    {
        // Rezept
        int ival;
        double fval, mengeMaischen, mengeKochbeginn100, mengeKochende100;
        QVariantMap ctxRezept;
        ctxRezept[QStringLiteral("SW")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSW).toDouble(), 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilMalz).toDouble();
        if (fval > 0)
            ctxRezept[QStringLiteral("SWAnteilMalz")] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzMaischen).toDouble();
        if (fval > 0)
            ctxRezept[QStringLiteral("SWAnteilZusatzMaischen")] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzKochen).toDouble();
        if (fval > 0)
            ctxRezept[QStringLiteral("SWAnteilZusatzKochen")] = locale.toString(fval, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilZusatzGaerung).toDouble();
        if (fval > 0)
            ctxRezept[QStringLiteral("SWAnteilZusatzGaerung")] = locale.toString(fval, 'f', 1);
        ctxRezept[QStringLiteral("Menge")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMenge).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("Alkohol")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAlkoholSoll).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("Bittere")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIBU).toInt());
        ctxRezept[QStringLiteral("CO2")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("Farbe")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toInt());
        ctxRezept[QStringLiteral("FarbeRgb")] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::Colerg_Farbe).toDouble())).name();
        ctxRezept[QStringLiteral("Kochdauer")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt());
        ival = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();
        if (ival > 0)
            ctxRezept[QStringLiteral("Nachisomerisierung")] = QString::number(ival);
        ival = bh->modelSud()->data(sudRow, ModelSud::ColhighGravityFaktor).toInt();
        if (ival > 0)
            ctxRezept[QStringLiteral("HighGravityFaktor")] = QString::number(ival);
        if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
        {
            ctxRezept[QStringLiteral("Wasserprofil")] = bh->modelSud()->data(sudRow, ModelSud::ColWasserprofil).toString();
            fval = bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetSoll).toDouble();
            if (fval > 0)
                ctxRezept[QStringLiteral("Restalkalitaet")] = locale.toString(fval, 'f', 2);
            ctxRezept[QStringLiteral("BemerkungWasseraufbereitung")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungWasseraufbereitung).toString());
        }
        ctxRezept[QStringLiteral("Reifezeit")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColReifezeit).toInt());
        mengeMaischen = bh->modelSud()->data(sudRow, ModelSud::Colerg_WHauptguss).toDouble() + BierCalc::MalzVerdraengung * bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble();
        ctxRezept[QStringLiteral("MengeMaischen")] = locale.toString(mengeMaischen, 'f', 1);
        ctxRezept[QStringLiteral("Name")] = bh->modelSud()->data(sudRow, ModelSud::ColSudname).toString();
        ctxRezept[QStringLiteral("Kategorie")] = bh->modelSud()->data(sudRow, ModelSud::ColKategorie).toString();
        ctxRezept[QStringLiteral("Nummer")] = bh->modelSud()->data(sudRow, ModelSud::ColSudnummer).toInt();
        ctxRezept[QStringLiteral("Kommentar")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColKommentar).toString());
        ctxRezept[QStringLiteral("BemerkungMaischen")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenMaischen).toString());
        ctxRezept[QStringLiteral("BemerkungKochen")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenKochen).toString());
        ctxRezept[QStringLiteral("BemerkungGaerung")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungZutatenGaerung).toString());
        ctxRezept[QStringLiteral("BemerkungMaischplan")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungMaischplan).toString());
        ctxRezept[QStringLiteral("Gesamtschuettung")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_S_Gesamt).toDouble(), 'f', 2);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochbeginn).toDouble();
        ctxRezept[QStringLiteral("MengeKochbeginn")] = locale.toString(fval, 'f', 1);
        mengeKochbeginn100 = BierCalc::volumenWasser(20.0, 100.0, fval);
        ctxRezept[QStringLiteral("MengeKochbeginn100")] = locale.toString(mengeKochbeginn100, 'f', 1);
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeSollKochende).toDouble();
        ctxRezept[QStringLiteral("MengeKochende")] = locale.toString(fval, 'f', 1);
        mengeKochende100 = BierCalc::volumenWasser(20.0, 100.0, fval);
        ctxRezept[QStringLiteral("MengeKochende100")] = locale.toString(mengeKochende100, 'f', 1);
        ctxRezept[QStringLiteral("SWKochbeginn")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginn).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("SWKochbeginnMitWz")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochbeginnMitWz).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("SWKochende")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollKochende).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("SWAnstellen")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWSollAnstellen).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("plato2brix")] = BierCalc::faktorPlatoToBrix;
        ctxRezept[QStringLiteral("SHA")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSudhausausbeute).toDouble(), 'f', 1);
        ctxRezept[QStringLiteral("EVG")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColVergaerungsgrad).toInt());
        fval = bh->modelSud()->data(sudRow, ModelSud::ColMengeHefestarter).toDouble();
        if (fval > 0)
        {
            ctxRezept[QStringLiteral("MengeHefestarter")] = locale.toString(fval, 'f', 1);
            ctxRezept[QStringLiteral("SWHefestarter")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWHefestarter).toDouble(), 'f', 1);
            ctxRezept[QStringLiteral("SWAnteilHefestarter")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWAnteilHefestarter).toDouble(), 'f', 1);
        }
        if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
        {
            if (bh->modelAusruestung()->rowCount() > 1)
                ctxRezept[QStringLiteral("Brauanlage")] = bh->modelSud()->data(sudRow, ModelSud::ColAnlage).toString();
            double A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
            double h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Hoehe).toDouble();
            ctxRezept[QStringLiteral("MengeMaischenUnten")] = locale.toString(mengeMaischen / A, 'f', 1);
            ctxRezept[QStringLiteral("MengeMaischenOben")] = locale.toString(h - mengeMaischen / A, 'f', 1);
            A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColSudpfanne_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
            h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColSudpfanne_Hoehe).toDouble();
            ctxRezept[QStringLiteral("MengeKochbeginn100Unten")] = locale.toString(mengeKochbeginn100 / A, 'f', 1);
            ctxRezept[QStringLiteral("MengeKochbeginn100Oben")] = locale.toString(h - mengeKochbeginn100 / A, 'f', 1);
            ctxRezept[QStringLiteral("MengeKochende100Unten")] = locale.toString(mengeKochende100 / A, 'f', 1);
            ctxRezept[QStringLiteral("MengeKochende100Oben")] = locale.toString(h - mengeKochende100 / A, 'f', 1);
        }
        ctx[QStringLiteral("Rezept")] = ctxRezept;

        // Sud
        QVariantMap ctxSud;
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->modelSud()->data(sudRow, ModelSud::ColStatus).toInt());
        ctxSud[QStringLiteral("Status")] = QString::number(static_cast<int>(status));
        switch (status)
        {
        case Brauhelfer::SudStatus::Rezept:
            ctxSud[QStringLiteral("StatusRezept")] = 1;
            break;
        case Brauhelfer::SudStatus::Gebraut:
            ctxSud[QStringLiteral("StatusGebraut")] = 1;
            break;
        case Brauhelfer::SudStatus::Abgefuellt:
            ctxSud[QStringLiteral("StatusAbgefuellt")] = 1;
            break;
        case Brauhelfer::SudStatus::Verbraucht:
            ctxSud[QStringLiteral("StatusVerbraucht")] = 1;
            break;
        }
        ctxSud[QStringLiteral("SW")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColSWIst).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("Menge")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColMengeIst).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("Bittere")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColIbuIst).toInt());
        ctxSud[QStringLiteral("CO2")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColCO2Ist).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("Farbe")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toInt());
        ctxSud[QStringLiteral("FarbeRgb")] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, ModelSud::ColFarbeIst).toDouble())).name();
        ctxSud[QStringLiteral("Braudatum")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColBraudatum).toDate(), QLocale::ShortFormat);
        ctxSud[QStringLiteral("Abfuelldatum")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColAbfuelldatum).toDate(), QLocale::ShortFormat);
        ctxSud[QStringLiteral("ReifungStart")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColReifungStart).toDate(), QLocale::ShortFormat);
        ctxSud[QStringLiteral("Alkohol")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Alkohol).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("EVG")] = QString::number(bh->modelSud()->data(sudRow, ModelSud::ColsEVG).toInt());
        ctxSud[QStringLiteral("SHA")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_Sudhausausbeute).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("effSHA")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::Colerg_EffektiveAusbeute).toDouble(), 'f', 1);
        ctxSud[QStringLiteral("BemerkungBrauen")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungBrauen).toString());
        ctxSud[QStringLiteral("BemerkungAbfuellen")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungAbfuellen).toString());
        ctxSud[QStringLiteral("BemerkungGaerung")] = textToHtml(bh->modelSud()->data(sudRow, ModelSud::ColBemerkungGaerung).toString());
        if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
        {
            ctxSud[QStringLiteral("Restalkalitaet")] = locale.toString(bh->modelSud()->data(sudRow, ModelSud::ColRestalkalitaetIst).toDouble(), 'f', 2);
        }
        if (gSettings->isModuleEnabled(Settings::ModuleBewertung))
        {
            int bewertung = bh->modelSud()->data(sudRow, ModelSud::ColBewertungMittel).toInt();
            ctxSud[QStringLiteral("Bewertung")] = QString::number(bewertung);
            if (bewertung > 0)
            {
                if (bewertung > 5)
                    bewertung = 5;
                QString s = QStringLiteral("");
                for (int i = 0; i < bewertung; i++)
                    s += QStringLiteral("<img class='star' width='24'>");
                for (int i = bewertung; i < 5; i++)
                    s += QStringLiteral("<img class='star_grey' width='24'>");
                ctxSud[QStringLiteral("BewertungSterne")] = s;
            }
        }
        ctx[QStringLiteral("Sud")] = ctxSud;

        if (sudRow == bh->sud()->row())
        {
                int kochDauer = bh->modelSud()->data(sudRow, ModelSud::ColKochdauer).toInt();
                int dauerIsomerisierung = bh->modelSud()->data(sudRow, ModelSud::ColNachisomerisierungszeit).toInt();

                liste.clear();
                ProxyModel *model = bh->sud()->modelMaischplan();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    int temp;
                    QVariantMap map;
                    map[QStringLiteral("Name")] = model->data(row, ModelMaischplan::ColName).toString();
                    map[QStringLiteral("Temp")] = QString::number(model->data(row, ModelMaischplan::ColTempRast).toInt());
                    map[QStringLiteral("Dauer")] = QString::number(model->data(row, ModelMaischplan::ColDauerRast).toInt());
                    switch (static_cast<Brauhelfer::RastTyp>(model->data(row, ModelMaischplan::ColTyp).toInt()))
                    {
                    case Brauhelfer::RastTyp::Einmaischen:
                        map[QStringLiteral("Einmaischen")] = true;
                        map[QStringLiteral("MengeWasser")] = locale.toString(model->data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1);
                        map[QStringLiteral("WasserTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempWasser).toInt());
                        map[QStringLiteral("MengeMalz")] = locale.toString(model->data(row, ModelMaischplan::ColMengeMalz).toDouble(), 'f', 2);
                        map[QStringLiteral("MalzTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempMalz).toInt());
                        break;
                    case Brauhelfer::RastTyp::Aufheizen:
                        map[QStringLiteral("Aufheizen")] = true;
                        break;
                    case Brauhelfer::RastTyp::Zubruehen:
                        map[QStringLiteral("Zubruehen")] = true;
                        map[QStringLiteral("MengeWasser")] = locale.toString(model->data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1);
                        map[QStringLiteral("WasserTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempWasser).toInt());
                        break;
                    case Brauhelfer::RastTyp::Zuschuetten:
                        map[QStringLiteral("Zuschuetten")] = true;
                        map[QStringLiteral("MengeWasser")] = locale.toString(model->data(row, ModelMaischplan::ColMengeWasser).toDouble(), 'f', 1);
                        map[QStringLiteral("WasserTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempWasser).toInt());
                        map[QStringLiteral("MengeMalz")] = locale.toString(model->data(row, ModelMaischplan::ColMengeMalz).toDouble(), 'f', 2);
                        map[QStringLiteral("MalzTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempMalz).toInt());
                        break;
                    case Brauhelfer::RastTyp::Dekoktion:
                        map[QStringLiteral("Dekoktion")] = true;
                        map[QStringLiteral("MengeMaische")] = locale.toString(model->data(row, ModelMaischplan::ColMengeMaische).toDouble(), 'f', 1);
                        temp = model->data(row, ModelMaischplan::ColDauerExtra1).toInt();
                        if (temp > 0)
                        {
                            map[QStringLiteral("TeilmaischeRastTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempExtra1).toInt());
                            map[QStringLiteral("TeilmaischeRastDauer")] = QString::number(temp);
                        }
                        temp = model->data(row, ModelMaischplan::ColDauerExtra2).toInt();
                        if (temp > 0)
                        {
                            map[QStringLiteral("TeilmaischeZusatzRastTemp")] = QString::number(model->data(row, ModelMaischplan::ColTempExtra2).toInt());
                            map[QStringLiteral("TeilmaischeZusatzRastDauer")] = QString::number(temp);
                        }
                        break;
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctx[QStringLiteral("Maischplan")] = QVariantMap({{"Liste", liste}});

                QVariantMap mapWasser;
                double f1 = 0.0, f2 = 0.0, f3 = 0.0;
                f1 = bh->sud()->geterg_WHauptguss();
                mapWasser[QStringLiteral("Hauptguss")] = locale.toString(f1, 'f', 1);
                if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
                {
                    double A = pow(bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Durchmesser).toDouble() / 2, 2) * M_PI / 1000;
                    double h = bh->modelSud()->dataAnlage(sudRow, ModelAusruestung::ColMaischebottich_Hoehe).toDouble();
                    mapWasser[QStringLiteral("HauptgussUnten")] = locale.toString(f1 / A, 'f', 1);
                    mapWasser[QStringLiteral("HauptgussOben")] = locale.toString(h - f1 / A, 'f', 1);
                }
                f2 = bh->sud()->geterg_WNachguss();
                mapWasser[QStringLiteral("Nachguss")] = locale.toString(f2, 'f', 1);
                if (bh->sud()->gethighGravityFaktor() != 0)
                {
                    f3 = bh->sud()->getMengeSollHgf();
                    mapWasser[QStringLiteral("Verduennung")] = locale.toString(f3, 'f', 2);
                }
                mapWasser[QStringLiteral("Gesamt")] = locale.toString(f1 + f2 + f3, 'f', 2);

                // Wasseraufbereitung
                if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
                {
                    liste.clear();
                    liste.reserve(model->rowCount());
                    model = bh->sud()->modelWasseraufbereitung();
                    for (int row = 0; row < model->rowCount(); ++row)
                    {
                        QVariantMap map;
                        map[QStringLiteral("Name")] = model->data(row, ModelWasseraufbereitung::ColName);
                        map[QStringLiteral("Einheit")] = MainWindow::Einheiten[model->data(row, ModelWasseraufbereitung::ColEinheit).toInt()];
                        double m = model->data(row, ModelWasseraufbereitung::ColMenge).toDouble();
                        map[QStringLiteral("MengeHauptguss")] = locale.toString(m * f1, 'f', 2);
                        map[QStringLiteral("MengeNachguss")] = locale.toString(m * f2, 'f', 2);
                        if (bh->sud()->gethighGravityFaktor() != 0)
                            map[QStringLiteral("MengeVerduennung")] = locale.toString(m * f3, 'f', 2);
                        map[QStringLiteral("Menge")] = locale.toString(m * (f1 + f2 + f3), 'f', 2);
                        liste << map;
                    }
                    if (!liste.empty())
                        mapWasser[QStringLiteral("Wasseraufbereitung")] = QVariantMap({{"Liste", liste}});
                }

                // Wasserprofil
                if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
                {
                    QVariantMap map;
                    map[QStringLiteral("Name")] = bh->sud()->getWasserData(ModelWasser::ColName).toString();
                    QString str = bh->sud()->getWasserData(ModelWasser::ColBemerkung).toString();
                    if (!str.isEmpty())
                        map[QStringLiteral("Bemerkung")] = str;
                    mapWasser[QStringLiteral("Wasserprofil")] = map;
                }

                ctx[QStringLiteral("Wasser")] = mapWasser;

                QVariantMap ctxZutaten;

                liste.clear();
                model = bh->sud()->modelMalzschuettung();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    map.insert(QStringLiteral("Name"), model->data(row, ModelMalzschuettung::ColName));
                    map.insert(QStringLiteral("Prozent"), locale.toString(model->data(row, ModelMalzschuettung::ColProzent).toDouble(), 'f', 1));
                    map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelMalzschuettung::Colerg_Menge).toDouble(), 'f', 2));
                    map.insert(QStringLiteral("Farbe"), QString::number(model->data(row, ModelMalzschuettung::ColFarbe).toInt()));
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten[QStringLiteral("Malz")] = QVariantMap({{"Liste", liste}});

                liste.clear();
                liste.reserve(model->rowCount());
                model = bh->sud()->modelHopfengaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    QVariantMap map;
                    int dauer = model->data(row, ModelHopfengaben::ColZeit).toInt();
                    map.insert(QStringLiteral("Name"), model->data(row, ModelHopfengaben::ColName));
                    int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map[QStringLiteral("Name")], ModelHopfen::ColTyp).toInt();
                    if (idx >= 0 && idx < MainWindow::HopfenTypname.count())
                        map.insert(QStringLiteral("Typ"), MainWindow::HopfenTypname[idx]);
                    map.insert(QStringLiteral("Prozent"), locale.toString(model->data(row, ModelHopfengaben::ColProzent).toDouble(), 'f', 1));
                    map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelHopfengaben::Colerg_Menge).toDouble(), 'f', 1));
                    map.insert(QStringLiteral("Alpha"), locale.toString(model->data(row, ModelHopfengaben::ColAlpha).toDouble(), 'f', 1));
                    if (static_cast<Brauhelfer::HopfenZeitpunkt>(model->data(row, ModelHopfengaben::ColZeitpunkt).toInt()) == Brauhelfer::HopfenZeitpunkt::Vorderwuerze)
                    {
                        map.insert(QStringLiteral("Kochdauer"), QString::number(dauer));
                        map.insert(QStringLiteral("ZugabeNach"), QString::number(kochDauer - dauer));
                        map.insert(QStringLiteral("Vorderwuerze"), true);
                        ctxZutaten[QStringLiteral("HatVorderwuerzehopfen")] = true;
                    }
                    else if (dauer > 0)
                    {
                        map.insert(QStringLiteral("Kochdauer"), QString::number(dauer));
                        map.insert(QStringLiteral("ZugabeNach"), QString::number(kochDauer - dauer));
                        map.insert(QStringLiteral("Kochen"), true);
                    }
                    else
                    {
                        map.insert(QStringLiteral("Dauer"), QString::number(dauerIsomerisierung+dauer));
                        map.insert(QStringLiteral("ZugabeNach"), QString::number(-dauer));
                        map.insert(QStringLiteral("Ausschlagen"), true);
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten[QStringLiteral("Hopfen")] = QVariantMap({{"Liste", liste}});

                liste.clear();
                model = bh->sud()->modelHefegaben();
                for (int row = 0; row < model->rowCount(); ++row)
                {
                    int menge = model->data(row, ModelHefegaben::ColMenge).toInt();
                    if (menge <= 0)
                        continue;
                    QVariantMap map;
                    map.insert(QStringLiteral("Name"), model->data(row, ModelHefegaben::ColName));
                    int idx = bh->modelHefe()->getValueFromSameRow(ModelHefe::ColName, map[QStringLiteral("Name")], ModelHefe::ColTypOGUG).toInt();
                    if (idx >= 0 && idx < MainWindow::HefeTypname.count())
                        map.insert(QStringLiteral("Typ"), MainWindow::HefeTypname[idx]);
                    map.insert(QStringLiteral("Menge"), menge);
                    if (model->data(row, ModelHefegaben::ColZugegeben).toBool())
                        map.insert(QStringLiteral("Status"), QObject::tr("zugegeben"));
                    else
                        map.insert(QStringLiteral("Status"), QObject::tr("nicht zugegeben"));
                    int zugabeNach = model->data(row, ModelHefegaben::ColZugabeNach).toInt();
                    if (zugabeNach > 0)
                    {
                        map.insert(QStringLiteral("ZugabeNach"), zugabeNach);
                        map.insert(QStringLiteral("ZugabeDatum"), locale.toString(model->data(row, ModelHefegaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
                    }
                    liste << map;
                }
                if (!liste.empty())
                    ctxZutaten[QStringLiteral("Hefe")] = QVariantMap({{"Liste", liste}});

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
                    map.insert(QStringLiteral("Name"), model->data(row, ModelWeitereZutatenGaben::ColName));
                    Brauhelfer::Einheit einheit = static_cast<Brauhelfer::Einheit>(model->data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
                    map.insert(QStringLiteral("Einheit"), MainWindow::Einheiten[static_cast<int>(einheit)]);
                    switch (einheit)
                    {
                    case Brauhelfer::Einheit::Kg:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::g:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    case Brauhelfer::Einheit::mg:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() * 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::Stk:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    case Brauhelfer::Einheit::l:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble() / 1000, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::ml:
                        map.insert(QStringLiteral("Menge"), locale.toString(model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(), 'f', 2));
                        break;
                    }
                    Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model->data(row, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
                    Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
                    Brauhelfer::ZusatzEntnahmeindex entnahmeindex = static_cast<Brauhelfer::ZusatzEntnahmeindex>(model->data(row, ModelWeitereZutatenGaben::ColEntnahmeindex).toInt());
                    Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(model->data(row, ModelWeitereZutatenGaben::ColZugabestatus).toInt());
                    switch (zeitpunkt)
                    {
                    case Brauhelfer::ZusatzZeitpunkt::Gaerung:
                        map.insert(QStringLiteral("Gaerung"), true);
                        map.insert(QStringLiteral("ZugabeNach"), QString::number(model->data(row, ModelWeitereZutatenGaben::ColZugabeNach).toInt()));
                        map.insert(QStringLiteral("ZugabeDatum"), locale.toString(model->data(row, ModelWeitereZutatenGaben::ColZugabeDatum).toDate(), QLocale::ShortFormat));
                        map.insert(QStringLiteral("EntnahmeDatum"), locale.toString(model->data(row, ModelWeitereZutatenGaben::ColEntnahmeDatum).toDate(), QLocale::ShortFormat));
                        if (entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme)
                        {
                            map.insert(QStringLiteral("Entnahme"), true);
                            map.insert(QStringLiteral("Dauer"), model->data(row, ModelWeitereZutatenGaben::ColZugabedauer).toInt() / 1440);
                        }
                        switch (status)
                        {
                        case Brauhelfer::ZusatzStatus::NichtZugegeben:
                            map.insert(QStringLiteral("Status"), QObject::tr("nicht zugegeben"));
                            break;
                        case Brauhelfer::ZusatzStatus::Zugegeben:
                            if (entnahmeindex == Brauhelfer::ZusatzEntnahmeindex::MitEntnahme)
                                map.insert(QStringLiteral("Status"), QObject::tr("nicht entnommen"));
                            else
                                map.insert(QStringLiteral("Status"), QObject::tr("zugegeben"));
                            break;
                        case Brauhelfer::ZusatzStatus::Entnommen:
                            map.insert(QStringLiteral("Status"), QObject::tr("entnommen"));
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
                            map.insert(QStringLiteral("Kochdauer"), QString::number(dauer));
                            map.insert(QStringLiteral("ZugabeNach"), QString::number(kochDauer - dauer));
                            map.insert(QStringLiteral("Kochen"), true);
                            listePtr = &listeZusatzKochen;
                        }
                        else
                        {
                            map.insert(QStringLiteral("Dauer"), QString::number(dauerIsomerisierung+dauer));
                            map.insert(QStringLiteral("ZugabeNach"), QString::number(-dauer));
                            map.insert(QStringLiteral("Ausschlagen"), true);
                            listePtr = &listeZusatzAusschlagen;
                        }
                        break;
                    case Brauhelfer::ZusatzZeitpunkt::Maischen:
                        map.insert(QStringLiteral("Maischen"), true);
                        listePtr = &listeZusatzMaischen;
                        break;
                    }
                    QString str = model->data(row, ModelWeitereZutatenGaben::ColBemerkung).toString();
                    if (!str.isEmpty())
                        map.insert(QStringLiteral("Bemerkung"), str);
                    if (typ == Brauhelfer::ZusatzTyp::Hopfen)
                    {
                        int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, map[QStringLiteral("Name")], ModelHopfen::ColTyp).toInt();
                        map.insert(QStringLiteral("Typ"), MainWindow::HopfenTypname[idx]);
                    }
                    else
                    {
                        map.insert(QStringLiteral("Typ"), MainWindow::ZusatzTypname[static_cast<int>(typ)]);
                        listeZusatzAlle << map;
                    }
                    if (listePtr)
                        *listePtr << map;
                }                
                if (!listeZusatzAlle.empty())
                    ctxZutaten[QStringLiteral("Zusatz")] = QVariantMap({{"Liste", listeZusatzAlle}});
                if (!listeZusatzMaischen.empty())
                    ctxZutaten[QStringLiteral("ZusatzMaischen")] = QVariantMap({{"Liste", listeZusatzMaischen}});
                if (!listeZusatzKochen.empty())
                    ctxZutaten[QStringLiteral("ZusatzKochen")] = QVariantMap({{"Liste", listeZusatzKochen}});
                if (!listeZusatzAusschlagen.empty())
                    ctxZutaten[QStringLiteral("ZusatzAusschlagen")] = QVariantMap({{"Liste", listeZusatzAusschlagen}});
                if (!listeZusatzGaerung.empty())
                    ctxZutaten[QStringLiteral("ZusatzGaerung")] = QVariantMap({{"Liste", listeZusatzGaerung}});
                if (!listeKalthopfen.empty())
                    ctxZutaten[QStringLiteral("Kalthopfen")] = QVariantMap({{"Liste", listeKalthopfen}});
                ctx[QStringLiteral("Zutaten")] = ctxZutaten;
        }

        // Anlage
        if (gSettings->isModuleEnabled(Settings::ModuleAusruestung))
        {
            QVariantMap ctxAnlage;
            QVariant anlageName = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColName).toString();
            QVariant anlageId = bh->modelAusruestung()->getValueFromSameRow(ModelAusruestung::ColName, anlageName, ModelAusruestung::ColID);
            ctxAnlage[QStringLiteral("Name")] = anlageName;
            ctxAnlage[QStringLiteral("KorrekturMenge")] = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColKorrekturMenge).toInt();
            ctxAnlage[QStringLiteral("Bemerkung")] = bh->modelAusruestung()->data(anlageIndex, ModelAusruestung::ColBemerkung).toString();
            ProxyModel modelGeraete;
            modelGeraete.setSourceModel(bh->modelGeraete());
            modelGeraete.setFilterKeyColumn(ModelGeraete::ColAusruestungAnlagenID);
            modelGeraete.setFilterRegularExpression(QStringLiteral("^%1$").arg(anlageId.toInt()));
            liste.clear();
            liste.reserve(modelGeraete.rowCount());
            for (int row = 0; row < modelGeraete.rowCount(); ++row)
            {
                liste << QVariantMap({{"Name", modelGeraete.data(row, ModelGeraete::ColBezeichnung)}});
            }
            if (!liste.empty())
                ctxAnlage[QStringLiteral("Geraete")] = QVariantMap({{"Liste", liste}});
            ctx[QStringLiteral("Anlage")] = ctxAnlage;
        }

        // Anhang
        QDir databasePath = QDir(gSettings->databaseDir());
        ProxyModel modelAnhang;
        modelAnhang.setSourceModel(bh->modelAnhang());
        modelAnhang.setFilterKeyColumn(ModelAnhang::ColSudID);
        modelAnhang.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId));
        liste.clear();
        liste.reserve(modelAnhang.rowCount());
        for (int row = 0; row < modelAnhang.rowCount(); ++row)
        {
            QVariantMap map;
            QString pfad = modelAnhang.data(row, ModelAnhang::ColPfad).toString();
            if (QDir::isRelativePath(pfad))
                pfad = QDir::cleanPath(databasePath.filePath(pfad));
            map.insert(QStringLiteral("Pfad"), pfad);
            map.insert(QStringLiteral("Bild"), WdgAnhang::isImage(pfad) && QFile::exists(pfad));
            liste << map;
        }
        if (!liste.empty())
            ctx[QStringLiteral("Anhang")] = QVariantMap({{"Liste", liste}});
    }

    // Tags
    QVariantMap ctxTags;
    QVariantMap ctxTagsGlobal;
    ProxyModel modelTags;
    modelTags.setSourceModel(bh->modelTags());
    modelTags.setFilterKeyColumn(ModelTags::ColSudID);
    modelTags.setFilterRegularExpression(QStringLiteral("^(%1|-.*)$").arg(sudId));
    for (int row = 0; row < modelTags.rowCount(); ++row)
    {
        QString t = modelTags.data(row, ModelTags::ColKey).toString();
        QString v = modelTags.data(row, ModelTags::ColValue).toString();
        t = t.simplified().replace(QStringLiteral(" "), QStringLiteral("_"));
        if (!t.isEmpty())
        {
            if (modelTags.data(row, ModelTags::ColGlobal).toBool())
                ctxTagsGlobal[t] = v;
            else
                ctxTags[t] = v;
        }
    }
    ctx[QStringLiteral("Tags")] = QVariantMap({{"ListeGlobal", ctxTagsGlobal}, {"Liste", ctxTags}});
}
