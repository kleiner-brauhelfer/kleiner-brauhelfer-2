#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QTextDocument>
#include <QDir>
#include <QFile>
#include <QtMath>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodel.h"
#include "templatetags.h"
#include "widgets/wdganhang.h"

extern Brauhelfer *bh;
extern Settings* gSettings;

void TabSudAuswahl::updateWebView()
{
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 1)
    {
        const ProxyModel *proxyModel = static_cast<ProxyModel*>(ui->tableSudauswahl->model());
        int sudRow = proxyModel->mapRowToSource(selection[0].row());
        TemplateTags::render(ui->webview, TemplateTags::TagAll, std::bind(&TabSudAuswahl::generateTemplateTags, this, std::placeholders::_1), sudRow);
    }
    else
    {
        TemplateTags::render(ui->webview, TemplateTags::TagAll, std::bind(&TabSudAuswahl::generateTemplateTags, this, std::placeholders::_1));
    }
}

void TabSudAuswahl::generateTemplateTags(QVariantMap& tags)
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

    QList<Rohstoff> ListMalz;
    ProxyModel modelMalzschuettung;
    modelMalzschuettung.setSourceModel(bh->modelMalzschuettung());
    for (int row = 0; row < modelMalzschuettung.rowCount(); ++row)
    {
        int sudId = modelMalzschuettung.data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelMalzschuettung.data(row, "Name").toString();
            eintrag.Menge = modelMalzschuettung.data(row, "erg_Menge").toDouble();
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
    ProxyModel modelHopfengaben;
    modelHopfengaben.setSourceModel(bh->modelHopfengaben());
    for (int row = 0; row < modelHopfengaben.rowCount(); ++row)
    {
        int sudId = modelHopfengaben.data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelHopfengaben.data(row, "Name").toString();
            eintrag.Menge = modelHopfengaben.data(row, "erg_Menge").toDouble();
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
    ProxyModel modelHefegaben;
    modelHefegaben.setSourceModel(bh->modelHefegaben());
    for (int row = 0; row < modelHefegaben.rowCount(); ++row)
    {
        int sudId = modelHefegaben.data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelHefegaben.data(row, "Name").toString();
            eintrag.Menge = modelHefegaben.data(row, "Menge").toDouble();
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
    ProxyModel modelWeitereZutatenGaben;
    modelWeitereZutatenGaben.setSourceModel(bh->modelWeitereZutatenGaben());
    for (int row = 0; row < modelWeitereZutatenGaben.rowCount(); ++row)
    {
        int sudId = modelWeitereZutatenGaben.data(row, "SudID").toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelWeitereZutatenGaben.data(row, "Name").toString();
            eintrag.Menge = modelWeitereZutatenGaben.data(row, "erg_Menge").toDouble();
            eintrag.Einheit = modelWeitereZutatenGaben.data(row, "Einheit").toInt();
            bool found = false;
            QList<Rohstoff> *liste = nullptr;
            switch (modelWeitereZutatenGaben.data(row, "Typ").toInt())
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
        ProxyModel modelMalz;
        modelMalz.setSourceModel(bh->modelMalz());
        modelMalz.setFilterKeyColumn(bh->modelMalz()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListMalz)
        {
            QVariantMap map;
            double ist = 0;
            modelMalz.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelMalz.rowCount() > 0)
            {
                ist = modelMalz.data(0, "Menge").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
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
        ProxyModel modelHopfen;
        modelHopfen.setSourceModel(bh->modelHopfen());
        modelHopfen.setFilterKeyColumn(bh->modelHopfen()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListHopfen)
        {
            QVariantMap map;
            double ist = 0;
            modelHopfen.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelHopfen.rowCount() > 0)
            {
                ist = modelHopfen.data(0, "Menge").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
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
        ProxyModel modelHefe;
        modelHefe.setSourceModel(bh->modelHefe());
        modelHefe.setFilterKeyColumn(bh->modelHefe()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListHefe)
        {
            QVariantMap map;
            double ist = 0;
            modelHefe.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelHefe.rowCount() > 0)
            {
                ist = modelHefe.data(0, "Menge").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
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
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListWeitereZutatenHonig)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, "MengeGramm").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
            map.insert("Name", eintrag.Name);
            switch (eintrag.Einheit)
            {
            case EWZ_Einheit_Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case EWZ_Einheit_g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
                break;
            case EWZ_Einheit_mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case EWZ_Einheit_Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Honig"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenZucker.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListWeitereZutatenZucker)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, "MengeGramm").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
            map.insert("Name", eintrag.Name);
            switch (eintrag.Einheit)
            {
            case EWZ_Einheit_Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case EWZ_Einheit_g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
                break;
            case EWZ_Einheit_mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case EWZ_Einheit_Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Zucker"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenGewuerz.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListWeitereZutatenGewuerz)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, "MengeGramm").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
            map.insert("Name", eintrag.Name);
            switch (eintrag.Einheit)
            {
            case EWZ_Einheit_Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case EWZ_Einheit_g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
                break;
            case EWZ_Einheit_mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case EWZ_Einheit_Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Gewuerz"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenFrucht.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListWeitereZutatenFrucht)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, "MengeGramm").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
            map.insert("Name", eintrag.Name);
            switch (eintrag.Einheit)
            {
            case EWZ_Einheit_Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case EWZ_Einheit_g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
                break;
            case EWZ_Einheit_mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case EWZ_Einheit_Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Frucht"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutatenSonstiges.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        for (const Rohstoff& eintrag : ListWeitereZutatenSonstiges)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, "MengeGramm").toDouble();
                map.insert("Class", ist < eintrag.Menge ? "nichtvorhanden" : "vorhanden");
            }
            else
            {
                map.insert("Class", "nichtgefunden");
            }
            map.insert("Name", eintrag.Name);
            switch (eintrag.Einheit)
            {
            case EWZ_Einheit_Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case EWZ_Einheit_g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
                map.insert("Einheit", tr("g"));
                break;
            case EWZ_Einheit_mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case EWZ_Einheit_Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Sonstiges"] = QVariantMap({{"Liste", liste}});
    }

    tags["Zutaten"] = ctxZutaten;
}
