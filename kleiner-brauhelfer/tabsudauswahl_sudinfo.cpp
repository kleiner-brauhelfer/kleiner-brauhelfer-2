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

void TabSudAuswahl::updateTemplateTags()
{
    const ProxyModel *proxyModel = static_cast<ProxyModel*>(ui->tableSudauswahl->model());
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();

    struct Rohstoff
    {
        QString Name;
        double Menge;
        int Einheit;
    };

    QString s;
    QList<int> ListSudID;
    QLocale locale;

    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        int SudID = proxyModel->data(index.row(), "ID").toInt();
        ListSudID.append(SudID);
    }

    mTemplateTags.clear();
    if (selection.count() == 1)
    {
        int sudRow = proxyModel->mapRowToSource(selection[0].row());
        WebView::erstelleTagListe(mTemplateTags, sudRow);

        int bewertung = proxyModel->data(selection[0].row(), "BewertungMax").toInt();
        if (bewertung > 0)
        {
            if (bewertung > Bewertung_MaxSterne)
                bewertung = Bewertung_MaxSterne;
            s = "";
            for (int i = 0; i < bewertung; i++)
                s += "<img class='star' style='padding:0px;margin:0px;' width='24' border=0>";
            for (int i = bewertung; i < Bewertung_MaxSterne; i++)
                s += "<img class='star_grey' style='padding:0px;margin:0px;' width='24' border=0>";
            mTemplateTags["Sterne"] = s;
        }
    }
    else
    {
        WebView::erstelleTagListe(mTemplateTags);
    }

    QList<Rohstoff> ListMalz;
    for (int row = 0; row < bh->modelMalzschuettung()->rowCount(); ++row)
    {
        int sudId = bh->modelMalzschuettung()->data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelMalzschuettung()->data(row, "Name").toString();
            eintrag.Menge = bh->modelMalzschuettung()->data(row, "erg_Menge").toDouble();
            bool found = false;
            for (Rohstoff& eintragListe : ListMalz)
            {
                if (eintrag.Name == eintragListe.Name)
                {
                    eintragListe.Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                ListMalz.append(eintrag);
        }
    }

    QList<Rohstoff> ListHopfen;
    for (int row = 0; row < bh->modelHopfengaben()->rowCount(); ++row)
    {
        int sudId = bh->modelHopfengaben()->data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelHopfengaben()->data(row, "Name").toString();
            eintrag.Menge = bh->modelHopfengaben()->data(row, "erg_Menge").toDouble();
            bool found = false;
            for (Rohstoff& eintragListe : ListHopfen)
            {
                if (eintrag.Name == eintragListe.Name)
                {
                    eintragListe.Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                ListHopfen.append(eintrag);
        }
    }

    QList<Rohstoff> ListHefe;
    for (int row = 0; row < bh->modelHefegaben()->rowCount(); ++row)
    {
        int sudId = bh->modelHefegaben()->data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelHefegaben()->data(row, "Name").toString();
            eintrag.Menge = bh->modelHefegaben()->data(row, "Menge").toDouble();
            bool found = false;
            for (Rohstoff& eintragListe : ListHefe)
            {
                if (eintrag.Name == eintragListe.Name)
                {
                    eintragListe.Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                ListHefe.append(eintrag);
        }
    }

    QList<Rohstoff> ListWeitereZutatenHonig;
    QList<Rohstoff> ListWeitereZutatenZucker;
    QList<Rohstoff> ListWeitereZutatenGewuerz;
    QList<Rohstoff> ListWeitereZutatenFrucht;
    QList<Rohstoff> ListWeitereZutatenSonstiges;
    for (int row = 0; row < bh->modelWeitereZutatenGaben()->rowCount(); ++row)
    {
        int sudId = bh->modelWeitereZutatenGaben()->data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = bh->modelWeitereZutatenGaben()->data(row, "Name").toString();
            eintrag.Menge = bh->modelWeitereZutatenGaben()->data(row, "erg_Menge").toDouble();
            eintrag.Einheit = bh->modelWeitereZutatenGaben()->data(row, "Einheit").toInt();
            bool found = false;
            QList<Rohstoff> *liste = nullptr;
            switch (bh->modelWeitereZutatenGaben()->data(row, "Typ").toInt())
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
            for (Rohstoff& eintragListe : *liste)
            {
                if (eintrag.Name == eintragListe.Name)
                {
                    eintragListe.Menge += eintrag.Menge;
                    found = true;
                    break;
                }
            }
            if (!found)
                liste->append(eintrag);
        }
    }

    QVariantMap ctxZutaten;

    if (ListMalz.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + " (" + tr("kg") + ")" + "</th>";
        s += "<th>" + tr("vorhanden") + " (" + tr("kg") + ")" + "</th>";
        s += "<th>" + tr("rest") + " (" + tr("kg") + ")" + "</th></tr>";
        for (const Rohstoff& eintrag : ListMalz)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelMalz()->rowCount(); o++)
            {
                if (bh->modelMalz()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelMalz()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 2) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 2) + "</td>";
            if (ist < eintrag.Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 2) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 2) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Malz"] = s;
    }

    if (ListHopfen.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + " (" + tr("g") + ")" + "</th>";
        s += "<th>" + tr("vorhanden") + " (" + tr("g") + ")" + "</th>";
        s += "<th>" + tr("rest") + " (" + tr("g") + ")" + "</th></tr>";
        for (const Rohstoff& eintrag : ListHopfen)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelHopfen()->rowCount(); o++)
            {
                if (bh->modelHopfen()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelHopfen()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 0) + "</td>";
            if (ist < eintrag.Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Hopfen"] = s;
    }

    if (ListHefe.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListHefe)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelHefe()->rowCount(); o++)
            {
                if (bh->modelHefe()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelHefe()->data(o, "Menge").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + "</td>";
            s += "<td align='center'>" + locale.toString(ist, 'f', 0) + "</td>";
            if (ist < eintrag.Menge)
                s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b></td>";
            else
                s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b></td>";
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Hefe"] = s;
    }

    if (ListWeitereZutatenHonig.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListWeitereZutatenHonig)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Honig"] = s;
    }

    if (ListWeitereZutatenZucker.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListWeitereZutatenZucker)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Zucker"] = s;
    }

    if (ListWeitereZutatenGewuerz.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListWeitereZutatenGewuerz)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Gewuerz"] = s;
    }

    if (ListWeitereZutatenFrucht.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListWeitereZutatenFrucht)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Frucht"] = s;
    }

    if (ListWeitereZutatenSonstiges.count() > 0)
    {
        s = "<table width='100%'>";
        s += "<tr><th></th><th></th>";
        s += "<th>" + tr("benötigt") + "</th>";
        s += "<th>" + tr("vorhanden") + "</th>";
        s += "<th>" + tr("rest") + "</th></tr>";
        for (const Rohstoff& eintrag : ListWeitereZutatenSonstiges)
        {
            double ist = 0;
            bool gefunden = false;
            for (int o=0; o < bh->modelWeitereZutaten()->rowCount(); o++)
            {
                if (bh->modelWeitereZutaten()->data(o, "Beschreibung").toString() == eintrag.Name)
                {
                    ist = bh->modelWeitereZutaten()->data(o, "MengeGramm").toDouble();
                    gefunden = true;
                    break;
                }
            }
            s += "<tr valign='middle'>";
            if (ist < eintrag.Menge)
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/nio.svg' width='16px' border=0></td>";
            else
                s += "<td><img style='padding:0px;margin:0px;' src='qrc:/images/io.svg' width='16px' border=0></td>";
            if (gefunden)
                s += "<td align='left'>" + eintrag.Name + "</td>";
            else
                s += "<td align='left' style='color: grey;'>" + eintrag.Name + "</td>";
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                s += "<td align='center'>" + locale.toString(ist  / 1000, 'f', 2) + " " + tr("kg") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString((ist - eintrag.Menge) / 1000, 'f', 2) + "</b> " + tr("kg") + "</td>";
            }
            else
            {
                s += "<td class='value' align='center'>" + locale.toString(eintrag.Menge, 'f', 0) + " " + tr("g") + "</td>";
                s += "<td align='center'>" + locale.toString(ist, 'f', 0) + " " + tr("g") + "</td>";
                if (ist < eintrag.Menge)
                    s += "<td align='center' style='color: red;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
                else
                    s += "<td align='center' style='color: green;'><b>" + locale.toString(ist - eintrag.Menge, 'f', 0) + "</b> " + tr("g") + "</td>";
            }
            s += "</tr>";
        }
        s += "</table>";
        ctxZutaten["Sonstiges"] = s;
    }

    mTemplateTags["Zutaten"] = ctxZutaten;

    QDir databasePath = QDir(gSettings->databaseDir());
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
    mTemplateTags["Anhang"] = s;

    ui->treeViewTemplateTags->clear();
    for (QVariantMap::const_iterator it = mTemplateTags.begin(); it != mTemplateTags.end(); ++it)
    {
        if (it.value().canConvert<QVariantMap>())
        {
            QVariantMap hash = it.value().toMap();
            QTreeWidgetItem *t = new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key()});
            for (QVariantMap::const_iterator it2 = hash.begin(); it2 != hash.end(); ++it2)
                t->addChild(new QTreeWidgetItem(t, {it2.key(), it2.value().toString()}));
            ui->treeViewTemplateTags->addTopLevelItem(t);
        }
        else
        {
            ui->treeViewTemplateTags->addTopLevelItem(new QTreeWidgetItem(ui->treeViewTemplateTags, {it.key(), it.value().toString()}));
        }
    }
}

void TabSudAuswahl::erstelleSudInfo()
{
    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTemplateAuswahl->currentIndex() == 0)
        {
            ui->webview->renderText(ui->tbTemplate->toPlainText(), mTemplateTags);
        }
        else
        {
            mTempCssFile.open();
            mTempCssFile.write(ui->tbTemplate->toPlainText().toUtf8());
            mTempCssFile.flush();
            mTemplateTags["Style"] = mTempCssFile.fileName();
            ui->webview->renderTemplate(mTemplateTags);
        }
    }
    else
    {
        ui->webview->renderTemplate(mTemplateTags);
    }
}
