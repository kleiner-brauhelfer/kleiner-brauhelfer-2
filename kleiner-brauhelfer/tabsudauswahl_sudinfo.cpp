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
#include "mainwindow.h"

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
        TemplateTags::render(ui->webview, std::bind(&TabSudAuswahl::generateTemplateTags, this, std::placeholders::_1), sudRow);
        ui->webview->setPdfName(proxyModel->data(selection[0].row(), ModelSud::ColSudname).toString());
    }
    else
    {
        TemplateTags::render(ui->webview, std::bind(&TabSudAuswahl::generateTemplateTags, this, std::placeholders::_1), -1);
        ui->webview->setPdfName(QStringLiteral("Rohstoffe"));
    }
}

void TabSudAuswahl::generateTemplateTags(QVariantMap& tags)
{
    const ProxyModel *proxyModel = static_cast<ProxyModel*>(ui->tableSudauswahl->model());

    struct Rohstoff
    {
        QString Name;
        QString Typ;
        double Menge;
        Brauhelfer::Einheit Einheit;
    };

    QList<int> ListSudID;
    QLocale locale;

    ListSudID.reserve(ui->tableSudauswahl->selectionModel()->selectedRows().count());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        int SudID = proxyModel->data(index.row(), ModelSud::ColID).toInt();
        ListSudID.append(SudID);
    }

    QVector<Rohstoff> ListMalz;
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
            eintrag.Einheit = Brauhelfer::Einheit::Kg;
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

    QVector<Rohstoff> ListHopfen;
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
            eintrag.Einheit = Brauhelfer::Einheit::g;
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

    QVector<Rohstoff> ListHefe;
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
            eintrag.Einheit = Brauhelfer::Einheit::Stk;
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

    QVector<Rohstoff> ListWeitereZutaten;
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
            QVector<Rohstoff> *liste = nullptr;
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColTyp).toInt());
            if (typ == Brauhelfer::ZusatzTyp::Hopfen)
            {
                int idx = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, eintrag.Name, ModelHopfen::ColTyp).toInt();
                eintrag.Typ = MainWindow::HopfenTypname[idx];
                liste = &ListHopfen;
            }
            else
            {
                eintrag.Typ = MainWindow::ZusatzTypname[static_cast<int>(typ)];
                liste = &ListWeitereZutaten;
            }
            eintrag.Einheit = static_cast<Brauhelfer::Einheit>(modelWeitereZutatenGaben.data(row, ModelWeitereZutatenGaben::ColEinheit).toInt());
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
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        ctxZutaten[QStringLiteral("Lager")] = true;

    if (ListMalz.count() > 0)
    {
        QVariantList liste;
        liste.reserve(ListMalz.count());
        ProxyModel modelMalz;
        modelMalz.setSourceModel(bh->modelMalz());
        modelMalz.setFilterKeyColumn(ModelMalz::ColName);
        for (const Rohstoff& eintrag : ListMalz)
        {
            QVariantMap map;
            map.insert(QStringLiteral("Name"), eintrag.Name);
            map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 2));
            map.insert(QStringLiteral("Einheit"), "kg");
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                double ist = 0;
                double diff = 0;
                modelMalz.setFilterRegularExpression(QStringLiteral("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
                if (modelMalz.rowCount() > 0)
                {
                    ist = modelMalz.data(0, ModelMalz::ColMenge).toDouble();
                    diff = qRound((ist - eintrag.Menge) * 100) / 100.0;
                    map.insert(QStringLiteral("Class"), diff < 0 ? "nichtvorhanden" : "vorhanden");
                    map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                }
                else
                {
                    map.insert(QStringLiteral("Class"), "nichtgefunden");
                }
                map.insert(QStringLiteral("Vorhanden"), locale.toString(ist, 'f', 2));
            }
            liste << map;
        }
        ctxZutaten[QStringLiteral("Malz")] = QVariantMap({{"Liste", liste}});
    }

    if (ListHopfen.count() > 0)
    {
        QVariantList liste;
        liste.reserve(ListHopfen.count());
        ProxyModel modelHopfen;
        modelHopfen.setSourceModel(bh->modelHopfen());
        modelHopfen.setFilterKeyColumn(ModelHopfen::ColName);
        for (const Rohstoff& eintrag : ListHopfen)
        {
            QVariantMap map;
            map.insert(QStringLiteral("Name"), eintrag.Name);
            map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 1));
            map.insert(QStringLiteral("Einheit"), "g");
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                double ist = 0;
                double diff = 0;
                modelHopfen.setFilterRegularExpression(QStringLiteral("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
                if (modelHopfen.rowCount() > 0)
                {
                    ist = modelHopfen.data(0, ModelHopfen::ColMenge).toDouble();
                    diff = qRound((ist - eintrag.Menge) * 10) / 10.0;
                    map.insert(QStringLiteral("Class"), diff < 0 ? "nichtvorhanden" : "vorhanden");
                    map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 1));
                }
                else
                {
                    map.insert(QStringLiteral("Class"), "nichtgefunden");
                }
                map.insert(QStringLiteral("Vorhanden"), locale.toString(ist, 'f', 1));
            }
            liste << map;
        }
        ctxZutaten[QStringLiteral("Hopfen")] = QVariantMap({{"Liste", liste}});
    }

    if (ListHefe.count() > 0)
    {
        QVariantList liste;
        liste.reserve(ListHefe.count());
        ProxyModel modelHefe;
        modelHefe.setSourceModel(bh->modelHefe());
        modelHefe.setFilterKeyColumn(ModelHefe::ColName);
        for (const Rohstoff& eintrag : ListHefe)
        {
            QVariantMap map;
            map.insert(QStringLiteral("Name"), eintrag.Name);
            map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 0));
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                int ist = 0;
                int diff = 0;
                modelHefe.setFilterRegularExpression(QStringLiteral("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
                if (modelHefe.rowCount() > 0)
                {
                    ist = modelHefe.data(0, ModelHefe::ColMenge).toInt();
                    diff = ist - eintrag.Menge;
                    map.insert(QStringLiteral("Class"), diff < 0 ? "nichtvorhanden" : "vorhanden");
                    map.insert(QStringLiteral("Rest"), QString::number(diff));
                }
                else
                {
                    map.insert(QStringLiteral("Class"), "nichtgefunden");
                }
                map.insert(QStringLiteral("Vorhanden"), QString::number(ist));
            }
            liste << map;
        }
        ctxZutaten[QStringLiteral("Hefe")] = QVariantMap({{"Liste", liste}});
    }

    if (ListWeitereZutaten.count() > 0)
    {
        QVariantList liste;
        ProxyModel modelWeitereZutaten;
        modelWeitereZutaten.setSourceModel(bh->modelWeitereZutaten());
        modelWeitereZutaten.setFilterKeyColumn(ModelWeitereZutaten::ColName);
        for (const Rohstoff& eintrag : ListWeitereZutaten)
        {
            QVariantMap map;
            map.insert(QStringLiteral("Name"), eintrag.Name);
            map.insert(QStringLiteral("Typ"), eintrag.Typ);
            double ist = 0;
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                double diff = 0;
                modelWeitereZutaten.setFilterRegularExpression(QStringLiteral("^%1$").arg(QRegularExpression::escape(eintrag.Name)));
                if (modelWeitereZutaten.rowCount() > 0)
                {
                    ist = modelWeitereZutaten.data(0, ModelWeitereZutaten::ColMengeNormiert).toDouble();
                    switch (eintrag.Einheit)
                    {
                    case Brauhelfer::Einheit::Kg:
                        diff = qRound((ist - eintrag.Menge) / 1000 * 100) / 100.0;
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::g:
                        diff = qRound((ist - eintrag.Menge) * 10) / 10.0;
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::mg:
                        diff = qRound((ist - eintrag.Menge) * 1000 * 10) / 10.0;
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::Stk:
                        diff = qRound((ist - qCeil(eintrag.Menge)));
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 0));
                        break;
                    case Brauhelfer::Einheit::l:
                        diff = qRound((ist - eintrag.Menge) / 1000 * 100) / 100.0;
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                        break;
                    case Brauhelfer::Einheit::ml:
                        diff = qRound((ist - eintrag.Menge) * 10) / 10.0;
                        map.insert(QStringLiteral("Rest"), locale.toString(diff, 'f', 2));
                        break;
                    }
                    map.insert(QStringLiteral("Class"), diff < 0 ? "nichtvorhanden" : "vorhanden");
                }
                else
                {
                    map.insert(QStringLiteral("Class"), "nichtgefunden");
                }
            }
            int einheit = static_cast<int>(eintrag.Einheit);
            if (einheit >= 0 && einheit < MainWindow::Einheiten.count())
            {
                map.insert(QStringLiteral("Einheit"), MainWindow::Einheiten[einheit]);
                switch (eintrag.Einheit)
                {
                case Brauhelfer::Einheit::Kg:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge / 1000, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist / 1000, 'f', 2));
                    break;
                case Brauhelfer::Einheit::g:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist, 'f', 2));
                    break;
                case Brauhelfer::Einheit::mg:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge * 1000, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist * 1000, 'f', 2));
                    break;
                case Brauhelfer::Einheit::Stk:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist, 'f', 0));
                    break;
                case Brauhelfer::Einheit::l:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge / 1000, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist / 1000, 'f', 2));
                    break;
                case Brauhelfer::Einheit::ml:
                    map.insert(QStringLiteral("Menge"), locale.toString(eintrag.Menge, 'f', 2));
                    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
                        map.insert(QStringLiteral("Vorhanden"), locale.toString(ist, 'f', 2));
                    break;
                }
            }
            liste << map;
        }
        ctxZutaten[QStringLiteral("Zusatz")] = QVariantMap({{"Liste", liste}});
    }

    tags[QStringLiteral("Zutaten")] = ctxZutaten;
}
