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
        int SudID = proxyModel->data(index.row(), ModelSud::ColID).toInt();
        ListSudID.append(SudID);
    }

    QList<Rohstoff> ListMalz;
    ProxyModel modelMalzschuettung;
    modelMalzschuettung.setSourceModel(bh->modelMalzschuettung());
    for (int row = 0; row < modelMalzschuettung.rowCount(); ++row)
    {
        int sudId = modelMalzschuettung.data(row, ModelMalzschuettung::ColSudID).toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelMalzschuettung.data(row, ModelMalzschuettung::ColName).toString();
            eintrag.Menge = modelMalzschuettung.data(row, ModelMalzschuettung::Colerg_Menge).toDouble();
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
        int sudId = modelHopfengaben.data(row, ModelHopfengaben::ColSudID).toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelHopfengaben.data(row, ModelHopfengaben::ColName).toString();
            eintrag.Menge = modelHopfengaben.data(row, ModelHopfengaben::Colerg_Menge).toDouble();
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
        int sudId = modelHefegaben.data(row, ModelHefegaben::ColSudID).toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelHefegaben.data(row, ModelHefegaben::ColName).toString();
            eintrag.Menge = modelHefegaben.data(row, ModelHefegaben::ColMenge).toDouble();
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
        int sudId = modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColSudID).toInt();
        if (ListSudID.contains(sudId))
        {
            Rohstoff eintrag;
            eintrag.Name = modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColName).toString();
            eintrag.Menge = modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble();
            eintrag.Einheit = modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt();
            bool found = false;
            QList<Rohstoff> *liste = nullptr;
            switch (modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColTyp).toInt())
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
        modelMalz.setFilterKeyColumn(ModelMalz::ColBeschreibung);
        for (const Rohstoff& eintrag : ListMalz)
        {
            QVariantMap map;
            double ist = 0;
            modelMalz.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelMalz.rowCount() > 0)
            {
                ist = modelMalz.data(0, ModelMalz::ColMenge).toDouble();
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
        modelHopfen.setFilterKeyColumn(ModelHopfen::ColBeschreibung);
        for (const Rohstoff& eintrag : ListHopfen)
        {
            QVariantMap map;
            double ist = 0;
            modelHopfen.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelHopfen.rowCount() > 0)
            {
                ist = modelHopfen.data(0, ModelHopfen::ColMenge).toDouble();
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
        modelHefe.setFilterKeyColumn(ModelHefe::ColBeschreibung);
        for (const Rohstoff& eintrag : ListHefe)
        {
            QVariantMap map;
            double ist = 0;
            modelHefe.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelHefe.rowCount() > 0)
            {
                ist = modelHefe.data(0, ModelHefe::ColMenge).toDouble();
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
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutatenHonig)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeGramm).toDouble();
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
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutatenZucker)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeGramm).toDouble();
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
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutatenGewuerz)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeGramm).toDouble();
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
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutatenFrucht)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeGramm).toDouble();
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
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutatenSonstiges)
        {
            QVariantMap map;
            double ist = 0;
            modelWeitereZutaten.setFilterRegExp(QString("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
            if (modelWeitereZutaten.rowCount() > 0)
            {
                ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeGramm).toDouble();
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
