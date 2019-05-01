#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QTextDocument>
#include <QDir>
#include <QFile>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodel.h"
#include "widgets/wdgwebvieweditable.h"
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

    QList<int> ListSudID;
    QLocale locale;

    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        int SudID = proxyModel->data(index.row(), "ID").toInt();
        ListSudID.append(SudID);
    }

    ui->webview->mTemplateTags.clear();
    if (selection.count() == 1)
    {
        int sudRow = proxyModel->mapRowToSource(selection[0].row());
        WdgWebViewEditable::erstelleTagListe(ui->webview->mTemplateTags, sudRow);

        int bewertung = proxyModel->data(selection[0].row(), "BewertungMax").toInt();
        if (bewertung > 0)
        {
            if (bewertung > Bewertung_MaxSterne)
                bewertung = Bewertung_MaxSterne;
            QString s = "";
            for (int i = 0; i < bewertung; i++)
                s += "<img class='star' style='padding:0px;margin:0px;' width='24' border=0>";
            for (int i = bewertung; i < Bewertung_MaxSterne; i++)
                s += "<img class='star_grey' style='padding:0px;margin:0px;' width='24' border=0>";
            ui->webview->mTemplateTags["Sterne"] = s;
        }
    }
    else
    {
        WdgWebViewEditable::erstelleTagListe(ui->webview->mTemplateTags);
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
    for (int sid = 0; sid < ListSudID.size(); ++sid)
    {
        int row = bh->modelSud()->getRowWithValue("ID", ListSudID[sid]);
        Rohstoff eintrag;
        eintrag.Name = bh->modelSud()->data(row, "AuswahlHefe").toString();
        eintrag.Menge = bh->modelSud()->data(row, "HefeAnzahlEinheiten").toDouble();
        if (eintrag.Name.isEmpty())
            continue;
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
        QVariantList liste;
        for (const Rohstoff& eintrag : ListMalz)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            map.insert("Menge", locale.toString(eintrag.Menge, 'f', 2));
            map.insert("Vorhanden", locale.toString(ist, 'f', 2));
            map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 2));
            map.insert("Einheit", tr("kg"));
            liste << map;
        }
        ctxZutaten["Malz"] = QVariantMap({{"Liste", liste}});
    }

    if (ListHopfen.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListHopfen)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
            map.insert("Vorhanden", locale.toString(ist, 'f', 0));
            map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
            map.insert("Einheit", tr("g"));
            liste << map;
        }
        ctxZutaten["Hopfen"] = QVariantMap({{"Liste", liste}});
    }

    if (ListHefe.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListHefe)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
            map.insert("Vorhanden", locale.toString(ist, 'f', 0));
            map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
            map.insert("Einheit", "");
            liste << map;
        }
        ctxZutaten["Hefe"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenHonig.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListWeitereZutatenHonig)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
            }
            else
            {
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
            }
            liste << map;
        }
        ctxZutaten["Honig"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenZucker.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListWeitereZutatenZucker)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
            }
            else
            {
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
            }
            liste << map;
        }
        ctxZutaten["Zucker"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenGewuerz.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListWeitereZutatenGewuerz)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
            }
            else
            {
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
            }
            liste << map;
        }
        ctxZutaten["Gewuerz"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenFrucht.count() > 0)
    {
        QVariantList liste;
        for (const Rohstoff& eintrag : ListWeitereZutatenFrucht)
        {
            QVariantMap map;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
            }
            else
            {
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
            }
            liste << map;
        }
        ctxZutaten["Frucht"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenSonstiges.count() > 0)
    {
        QVariantMap map;
        QVariantList liste;
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
            if (gefunden)
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            else
                map.insert("Class", "nichtgefunden");
            map.insert("Name", eintrag.Name);
            if (eintrag.Einheit == EWZ_Einheit_Kg)
            {
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
            }
            else
            {
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
            }
            liste << map;
        }
        ctxZutaten["Sonstiges"] = QVariantMap({{"Liste", liste}});
    }

    ui->webview->mTemplateTags["Zutaten"] = ctxZutaten;

    if (selection.count() == 1)
    {
        QDir databasePath = QDir(gSettings->databaseDir());
        QVariantList liste;
        for (int i = 0; i < bh->modelAnhang()->rowCount(); ++i)
        {
            int sudId = bh->modelAnhang()->data(i, "SudID").toInt();
            if (ListSudID.contains(sudId))
            {
                QVariantMap map;
                QString pfad = bh->modelAnhang()->data(i, "Pfad").toString();
                if (QDir::isRelativePath(pfad))
                    pfad = QDir::cleanPath(databasePath.filePath(pfad));
                map.insert("Pfad", pfad);
                map.insert("Bild", WdgAnhang::isImage(pfad) ? "1" : "");
                liste << map;
            }
        }
        if (!liste.empty())
            ui->webview->mTemplateTags["Anhang"] = QVariantMap({{"Liste", liste}});
    }

    ui->webview->updateTags();
    ui->webview->updateHtml();
}
