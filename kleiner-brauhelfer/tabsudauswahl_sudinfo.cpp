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
#include "tabrohstoffe.h"
#include "widgets/wdganhang.h"

extern Brauhelfer *bh;
extern Settings* gSettings;

void TabSudAuswahl::updateWebView()
{
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 0)
    {
        ui->webview->clear();
    }
    else if (selection.count() == 1)
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
        QString Typ;
        double Menge;
        Brauhelfer::ZusatzEinheit Einheit;
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
            eintrag.Einheit = Brauhelfer::ZusatzEinheit::Kg;
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
            int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, eintrag.Name, ModelHopfen::ColTyp).toInt();
            if (idx >= 0 && idx < TabRohstoffe::HopfenTypname.count())
                eintrag.Typ = TabRohstoffe::HopfenTypname[idx];
            eintrag.Menge = modelHopfengaben.data(row, ModelHopfengaben::Colerg_Menge).toDouble();
            eintrag.Einheit = Brauhelfer::ZusatzEinheit::g;
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
            int idx = bh->modelHefe()->getValueFromSameRow(ModelHefe::ColBeschreibung, eintrag.Name, ModelHefe::ColTypOGUG).toInt();
            if (idx >= 0 && idx < TabRohstoffe::HefeTypname.count())
                eintrag.Typ = TabRohstoffe::HefeTypname[idx];
            eintrag.Menge = modelHefegaben.data(row, ModelHefegaben::ColMenge).toDouble();
            eintrag.Einheit = Brauhelfer::ZusatzEinheit::Stk;
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

    QList<Rohstoff> ListWeitereZutaten;
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
            QList<Rohstoff> *liste = nullptr;
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            {
                int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColBeschreibung, eintrag.Name, ModelHopfen::ColTyp).toInt();
                eintrag.Typ = TabRohstoffe::HopfenTypname[idx];
                liste = &ListHopfen;
            }
            else
            {
                eintrag.Typ = TabRohstoffe::ZusatzTypname[static_cast<int>(typ)];
                liste = &ListWeitereZutaten;
            }
            eintrag.Einheit = static_cast<Brauhelfer::ZusatzEinheit>(modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
            bool found = false;
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
            map.insert("Typ", eintrag.Typ);
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
            map.insert("Typ", eintrag.Typ);
            map.insert("Menge", locale.toString(eintrag.Menge, 'f', 0));
            map.insert("Vorhanden", locale.toString(ist, 'f', 0));
            map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 0));
            map.insert("Einheit", "");
            liste << map;
        }
        ctxZutaten["Hefe"] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutaten.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColBeschreibung);
        for (const Rohstoff& eintrag : ListWeitereZutaten)
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
            map.insert("Typ", eintrag.Typ);
            switch (eintrag.Einheit)
            {
            case Brauhelfer::ZusatzEinheit::Kg:
                map.insert("Menge", locale.toString(eintrag.Menge / 1000, 'f', 2));
                map.insert("Vorhanden", locale.toString(ist / 1000, 'f', 2));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) / 1000, 'f', 2));
                map.insert("Einheit", tr("kg"));
                break;
            case Brauhelfer::ZusatzEinheit::g:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 1));
                map.insert("Rest", locale.toString(ist - eintrag.Menge, 'f', 1));
                map.insert("Einheit", tr("g"));
                break;
            case Brauhelfer::ZusatzEinheit::mg:
                map.insert("Menge", locale.toString(eintrag.Menge * 1000, 'f', 0));
                map.insert("Vorhanden", locale.toString(ist * 1000, 'f', 0));
                map.insert("Rest", locale.toString((ist - eintrag.Menge) * 1000, 'f', 0));
                map.insert("Einheit", tr("mg"));
                break;
            case Brauhelfer::ZusatzEinheit::Stk:
                map.insert("Menge", locale.toString(eintrag.Menge, 'f', 1));
                map.insert("Vorhanden", locale.toString(ist, 'f', 0));
                map.insert("Rest", locale.toString(ist - qCeil(eintrag.Menge), 'f', 0));
                map.insert("Einheit", tr("Stk."));
                break;
            }
            liste << map;
        }
        ctxZutaten["Zusatz"] = QVariantMap({{"Liste", liste}});
    }

    tags["Zutaten"] = ctxZutaten;
}
