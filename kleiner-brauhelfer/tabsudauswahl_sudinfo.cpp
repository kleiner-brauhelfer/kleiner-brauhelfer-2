#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QTextDocument>
#include <QDir>
#include <QFile>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodel.h"
#include "widgets/wdganhang.h"

extern Brauhelfer *bh;
extern Settings* gSettings;

// TODO: support generic sud tags like Sud.CO2 (tolower, float precision)

void TabSudAuswahl::erstelleSudInfo()
{
    struct Rohstoff
    {
        QString Name;
        double Menge;
        int Einheit;
    };

    QVariantHash contextVariables;
    QString s;
    QList<int> ListSudID;
    QLocale locale;

    ui->webview->erstelleTagListe(contextVariables, false);

    const ProxyModel *proxyModel = static_cast<ProxyModel*>(ui->tableSudauswahl->model());
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 1)
    {
        int SudID = proxyModel->data(selection[0].row(), "ID").toInt();
        ListSudID.append(SudID);

        //Überschrift
        contextVariables["Sudname"] = proxyModel->data(selection[0].row(), "Sudname").toString();

        //bild mit entsprechender Bierfarbe
        QColor farbe(BierCalc::ebcToColor(proxyModel->data(selection[0].row(), "erg_Farbe").toDouble()));
        int bewertung = proxyModel->data(selection[0].row(), "BewertungMax").toInt();

        //Bewertung
        if (bewertung > 0){
          if (bewertung > Bewertung_MaxSterne)
            bewertung = Bewertung_MaxSterne;
          s = "";
          for (int i = 0; i<bewertung; i++){
            s += "<img style='padding:0px;margin:0px;' src='qrc:/images/star.svg' width='24' border=0>";
          }
          for (int i = bewertung; i < Bewertung_MaxSterne; i++){
            if (gSettings->theme() == Settings::Dark)
              s += "<img style='padding:0px;margin:0px;' src='qrc:/images/star_gr_dark.svg' width='24' border=0>";
            else
              s += "<img style='padding:0px;margin:0px;' src='qrc:/images/star_gr.svg' width='24' border=0>";
          }
          contextVariables["Sterne"] = s;
        }

        // FarbeHtml
        contextVariables["FarbeHtml"] = farbe.name();

        // Glas
        s = "<div style='background-color:" + farbe.name() +";padding:0px;margin:0px;width:100%;height:100%'>";
        if (gSettings->theme() == Settings::Dark)
          s += "<img style='padding:0px;margin:0px;width:100%;height:100%' src='qrc:/images/bier_dark.svg'>";
        else
          s += "<img style='padding:0px;margin:0px;width:100%;height:100%' src='qrc:/images/bier.svg'>";
        s += "</div>";
        contextVariables["Glas"] = s;

        // Glas-hell
        s = "<div style='background-color:" + farbe.name() +";padding:0px;margin:0px;width:100%;height:100%'>";
        s += "<img style='padding:0px;margin:0px;width:100%;height:100%' src='qrc:/images/bier.svg'>";
        s += "</div>";
        contextVariables["Glas-hell"] = s;

        // Glas-dunkel
        s = "<div style='background-color:" + farbe.name() +";padding:0px;margin:0px;width:100%;height:100%'>";
        s += "<img style='padding:0px;margin:0px;width:100%;height:100%' src='qrc:/images/bier_dark.svg'>";
        s += "</div>";
        contextVariables["Glas-dunkel"] = s;

        //Solldaten des Rezeptes
        s = "<table><tbody>";
        s += "<tr>";
        s += "<td>" + tr("Menge") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(proxyModel->data(selection[0].row(), "Menge").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("Liter") + "</td>";
        s += "</tr>";
        s += "<tr>";
        s += "<td>" + tr("Stammwürze") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(proxyModel->data(selection[0].row(), "SW").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("°P") + "</td>";
        s += "</tr>";
        s += "<tr>";
        s += "<td>" + tr("Bittere") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(proxyModel->data(selection[0].row(), "IBU").toInt()) + "</td>";
        s += "<td>" + tr("IBU") + "</td>";
        s += "</tr>";
        s += "<tr>";
        s += "<td>" + tr("Farbe") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(proxyModel->data(selection[0].row(), "erg_Farbe").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("EBC") + "</td>";
        s += "</tr>";
        s += "<tr>";
        s += "<td>" + tr("CO₂-Gehalt") + "</td>";
        s += "<td class='value' align='right'>" + locale.toString(proxyModel->data(selection[0].row(), "CO2").toDouble(), 'f', 1) + "</td>";
        s += "<td>" + tr("g/l") + "</td>";
        s += "</tr>";
        s += "</tbody></table>";
        contextVariables["Rezept"] = s;

        // Kommentar
        QString kommentar = proxyModel->data(selection[0].row(), "Kommentar").toString();
        kommentar.replace("\n", "<br>");
        contextVariables["Kommentar"] = "<div style=\"word-wrap: break-word;\">" + kommentar + "</div>";
    }
    else if (selection.count() > 1)
    {
        for (int i = 0; i < selection.count(); ++i)
        {
            int SudID = proxyModel->data(selection[i].row(), "ID").toInt();
            ListSudID.append(SudID);
        }
    }

    QList<Rohstoff> ListMalz;
    for (int i = 0; i < bh->modelMalzschuettung()->rowCount(); ++i)
    {
        int sudId = bh->modelMalzschuettung()->data(i, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelMalzschuettung()->data(i, "Name").toString();
            eintrag.Menge = bh->modelMalzschuettung()->data(i, "erg_Menge").toDouble();
            bool found = false;
            for (int j = 0; j < ListMalz.size(); ++j)
            {
                if (eintrag.Name == ListMalz[j].Name)
                {
                    ListMalz[j].Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                ListMalz.append(eintrag);
        }
    }

    QList<Rohstoff> ListHopfen;
    for (int i = 0; i < bh->modelHopfengaben()->rowCount(); ++i)
    {
        int sudId = bh->modelHopfengaben()->data(i, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelHopfengaben()->data(i, "Name").toString();
            eintrag.Menge = bh->modelHopfengaben()->data(i, "erg_Menge").toDouble();
            bool found = false;
            for (int j = 0; j < ListHopfen.size(); ++j)
            {
                if (eintrag.Name == ListHopfen[j].Name)
                {
                    ListHopfen[j].Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                ListHopfen.append(eintrag);
        }
    }

    QList<Rohstoff> ListHefe;
    for (int sid = 0; sid < ListSudID.size(); ++sid)
    {
        int row = bh->modelSud()->getRowWithValue("ID", ListSudID[sid]);
        Rohstoff eintrag;
        eintrag.Name = bh->modelSud()->data(row, "AuswahlHefe").toString();
        eintrag.Menge = bh->modelSud()->data(row, "HefeAnzahlEinheiten").toDouble();
        if (eintrag.Name.isEmpty())
            continue;
        bool found = false;
        for (int j = 0; j < ListHefe.size(); ++j)
        {
            if (eintrag.Name == ListHefe[j].Name)
            {
                ListHefe[j].Menge += eintrag.Menge;
                found = true;
                break;
            }
        }
        if (!found)
            ListHefe.append(eintrag);

    }

    QList<Rohstoff> ListWeitereZutatenHonig;
    QList<Rohstoff> ListWeitereZutatenZucker;
    QList<Rohstoff> ListWeitereZutatenGewuerz;
    QList<Rohstoff> ListWeitereZutatenFrucht;
    QList<Rohstoff> ListWeitereZutatenSonstiges;
    for (int i = 0; i < bh->modelWeitereZutatenGaben()->rowCount(); ++i)
    {
        int sudId = bh->modelWeitereZutatenGaben()->data(i, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelWeitereZutatenGaben()->data(i, "Name").toString();
            eintrag.Menge = bh->modelWeitereZutatenGaben()->data(i, "erg_Menge").toDouble();
            eintrag.Einheit = bh->modelWeitereZutatenGaben()->data(i, "Einheit").toInt();
            bool found = false;
            QList<Rohstoff> *liste = nullptr;
            switch (bh->modelWeitereZutatenGaben()->data(i, "Typ").toInt())
            {
            case EWZ_Typ_Honig:
                liste = &ListWeitereZutatenHonig;
                break;
            case EWZ_Typ_Zucker:
                liste = &ListWeitereZutatenZucker;
                break;
            case EWZ_Typ_Gewuerz:
                liste = &ListWeitereZutatenGewuerz;
                break;
            case EWZ_Typ_Frucht:
                liste = &ListWeitereZutatenFrucht;
                break;
            case EWZ_Typ_Sonstiges:
                liste = &ListWeitereZutatenSonstiges;
                break;
            case EWZ_Typ_Hopfen:
                liste = &ListHopfen;
                break;
            }
            for (int j = 0; j < liste->size(); ++j)
            {
                if (eintrag.Name == (*liste)[j].Name)
                {
                    (*liste)[j].Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                liste->append(eintrag);
        }
    }

    if (selection.count() > 0)
    {
        //Überschrift Benötigte Rohstoffe
        contextVariables["Rohstoffe"] = tr("Rohstoffe");
    }

    if (ListMalz.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + " (" + tr("kg") + ")" + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + " (" + tr("kg") + ")" + "</td>";
        s += "<td align='center'>" + tr("rest") + " (" + tr("kg") + ")" + "</td></tr>";
        for (int i = 0; i < ListMalz.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelMalz()->rowCount(); o++)
            {
                if (bh->modelMalz()->data(o, "Beschreibung").toString() == ListMalz.at(i).Name)
                {
                    ist = bh->modelMalz()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListMalz.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListMalz.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListMalz.at(i).Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(ListMalz.at(i).Menge, 'f', 2) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 2) + "</td>";
            if (ist < ListMalz.at(i).Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListMalz.at(i).Menge, 'f', 2) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListMalz.at(i).Menge, 'f', 2) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Malz"] = s;
    }

    if (ListHopfen.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + " (" + tr("g") + ")" + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + " (" + tr("g") + ")" + "</td>";
        s += "<td align='center'>" + tr("rest") + " (" + tr("g") + ")" + "</td></tr>";
        for (int i = 0; i < ListHopfen.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelHopfen()->rowCount(); o++)
            {
                if (bh->modelHopfen()->data(o, "Beschreibung").toString() == ListHopfen.at(i).Name)
                {
                    ist = bh->modelHopfen()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListHopfen.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListHopfen.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListHopfen.at(i).Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(ListHopfen.at(i).Menge, 'f', 0) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 0) + "</td>";
            if (ist < ListHopfen.at(i).Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListHopfen.at(i).Menge, 'f', 0) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListHopfen.at(i).Menge, 'f', 0) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Hopfen"] = s;
    }

    if (ListHefe.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListHefe.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelHefe()->rowCount(); o++)
            {
                if (bh->modelHefe()->data(o, "Beschreibung").toString() == ListHefe.at(i).Name)
                {
                    ist = bh->modelHefe()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListHefe.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListHefe.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListHefe.at(i).Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(ListHefe.at(i).Menge, 'f', 0) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 0) + "</td>";
            if (ist < ListHefe.at(i).Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListHefe.at(i).Menge, 'f', 0) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListHefe.at(i).Menge, 'f', 0) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Hefe"] = s;
    }

    if (ListWeitereZutatenHonig.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListWeitereZutatenHonig.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == ListWeitereZutatenHonig.at(i).Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListWeitereZutatenHonig.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListWeitereZutatenHonig.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListWeitereZutatenHonig.at(i).Name + "</td>";
            if (ListWeitereZutatenHonig.at(i).Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenHonig.at(i).Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < ListWeitereZutatenHonig.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - ListWeitereZutatenHonig.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - ListWeitereZutatenHonig.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenHonig.at(i).Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < ListWeitereZutatenHonig.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListWeitereZutatenHonig.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListWeitereZutatenHonig.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Honig"] = s;
    }

    if (ListWeitereZutatenZucker.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListWeitereZutatenZucker.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == ListWeitereZutatenZucker.at(i).Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListWeitereZutatenZucker.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListWeitereZutatenZucker.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListWeitereZutatenZucker.at(i).Name + "</td>";
            if (ListWeitereZutatenZucker.at(i).Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenZucker.at(i).Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < ListWeitereZutatenZucker.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - ListWeitereZutatenZucker.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - ListWeitereZutatenZucker.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenZucker.at(i).Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < ListWeitereZutatenZucker.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListWeitereZutatenZucker.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListWeitereZutatenZucker.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Zucker"] = s;
    }

    if (ListWeitereZutatenGewuerz.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListWeitereZutatenGewuerz.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == ListWeitereZutatenGewuerz.at(i).Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListWeitereZutatenGewuerz.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListWeitereZutatenGewuerz.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListWeitereZutatenGewuerz.at(i).Name + "</td>";
            if (ListWeitereZutatenGewuerz.at(i).Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenGewuerz.at(i).Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < ListWeitereZutatenGewuerz.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - ListWeitereZutatenGewuerz.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - ListWeitereZutatenGewuerz.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenGewuerz.at(i).Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < ListWeitereZutatenGewuerz.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListWeitereZutatenGewuerz.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListWeitereZutatenGewuerz.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Gewuerz"] = s;
    }

    if (ListWeitereZutatenFrucht.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListWeitereZutatenFrucht.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == ListWeitereZutatenFrucht.at(i).Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListWeitereZutatenFrucht.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListWeitereZutatenFrucht.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListWeitereZutatenFrucht.at(i).Name + "</td>";
            if (ListWeitereZutatenFrucht.at(i).Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenFrucht.at(i).Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < ListWeitereZutatenFrucht.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - ListWeitereZutatenFrucht.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - ListWeitereZutatenFrucht.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenFrucht.at(i).Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < ListWeitereZutatenFrucht.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListWeitereZutatenFrucht.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListWeitereZutatenFrucht.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Frucht"] = s;
    }

    if (ListWeitereZutatenSonstiges.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><td></td><td></td>";
        s += "<td align='center'>" + tr("benötigt") + "</td>";
        s += "<td align='center'>" + tr("vorhanden") + "</td>";
        s += "<td align='center'>" + tr("rest") + "</td></tr>";
        for (int i = 0; i < ListWeitereZutatenSonstiges.size(); ++i)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == ListWeitereZutatenSonstiges.at(i).Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < ListWeitereZutatenSonstiges.at(i).Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' alt='IO' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' alt='IO' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + ListWeitereZutatenSonstiges.at(i).Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + ListWeitereZutatenSonstiges.at(i).Name + "</td>";
            if (ListWeitereZutatenSonstiges.at(i).Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenSonstiges.at(i).Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < ListWeitereZutatenSonstiges.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - ListWeitereZutatenSonstiges.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - ListWeitereZutatenSonstiges.at(i).Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(ListWeitereZutatenSonstiges.at(i).Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < ListWeitereZutatenSonstiges.at(i).Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - ListWeitereZutatenSonstiges.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - ListWeitereZutatenSonstiges.at(i).Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        contextVariables["Sonstiges"] = s;
    }

    QDir databasePath = QDir(gSettings->databaseDir());
    contextVariables["AnhangTitel"] = tr("Anhänge");
    s = "";
    for (int i = 0; i < bh->modelAnhang()->rowCount(); ++i)
    {
        int sudId = bh->modelAnhang()->data(i, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            QString pfad = bh->modelAnhang()->data(i, "Pfad").toString();
            if (QDir::isRelativePath(pfad))
                pfad = QDir::cleanPath(databasePath.filePath(pfad));
            if (WdgAnhang::isImage(pfad))
                s += "<img style=\"max-width:90%;\" src=\"" + pfad + "\" alt=\"" + pfad + "\"></br></br>";
            else
                s += "<a href=\"" + pfad + "\">" + pfad + "</a></br></br>";
        }
    }
    contextVariables["Anhang"] = s;

    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTemplateAuswahl->currentIndex() == 0)
        {
            ui->webview->renderText(ui->tbTemplate->toPlainText(), contextVariables);
        }
        else
        {
            mTempCssFile.open();
            mTempCssFile.write(ui->tbTemplate->toPlainText().toUtf8());
            mTempCssFile.flush();
            contextVariables["Style"] = mTempCssFile.fileName();
            ui->webview->renderTemplate(contextVariables);
        }
    }
    else
    {
        ui->webview->renderTemplate(contextVariables);
    }
}
