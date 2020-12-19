#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QStyleFactory>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "definitionen.h"
#include "tababstract.h"
#include "dialogs/dlgbierspende.h"
#include "dialogs/dlgabout.h"
#include "dialogs/dlgcheckupdate.h"
#include "dialogs/dlgdatabasecleaner.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgtableview.h"
#include "dialogs/dlgconsole.h"
#include "dialogs/dlgrohstoffeabziehen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    ui->actionThemeSystem->setEnabled(gSettings->theme() != Settings::Theme::System);
    ui->actionThemeHell->setEnabled(gSettings->theme() != Settings::Theme::Bright);
    ui->actionThemeDunkel->setEnabled(gSettings->theme() != Settings::Theme::Dark);

  #if 0
    QString style = gSettings->style();
    for(const QString &key : QStyleFactory::keys())
    {
        QAction *action = new QAction(key, this);
        if (key == style)
            action->setEnabled(false);
        else
            connect(action, SIGNAL(triggered()), this, SLOT(changeStyle()));
        ui->menuStil->addAction(action);
    }
  #endif
    ui->menuStil->menuAction()->setVisible(!ui->menuStil->isEmpty());

    ui->actionSchriftart->setChecked(gSettings->useSystemFont());

    gSettings->beginGroup("MainWindow");
    restoreGeometry(gSettings->value("geometry").toByteArray());
    mDefaultState = saveState();
    restoreState(gSettings->value("state").toByteArray());
    gSettings->endGroup();

    mTabIndexZusammenfassung = ui->tabMain->indexOf(ui->tabZusammenfassung);
    gSettings->beginGroup("TabZusammenfassung");
    ui->actionReiterZusammenfassung->setChecked(gSettings->value("visible", true).toBool());
    gSettings->endGroup();
    on_actionReiterZusammenfassung_triggered(ui->actionReiterZusammenfassung->isChecked());

    mTabIndexEtikette = ui->tabMain->indexOf(ui->tabEtikette);
    gSettings->beginGroup("TabEtikette");
    ui->actionReiterEtikette->setChecked(gSettings->value("visible", true).toBool());
    gSettings->endGroup();
    on_actionReiterEtikette_triggered(ui->actionReiterEtikette->isChecked());

    mTabIndexBewertung = ui->tabMain->indexOf(ui->tabBewertung);
    gSettings->beginGroup("TabBewertung");
    ui->actionReiterBewertung->setChecked(gSettings->value("visible", true).toBool());
    gSettings->endGroup();
    on_actionReiterBewertung_triggered(ui->actionReiterBewertung->isChecked());

    mTabIndexBrauuebersicht = ui->tabMain->indexOf(ui->tabBrauuebersicht);
    gSettings->beginGroup("TabBrauuebersicht");
    ui->actionReiterBrauuebersicht->setChecked(gSettings->value("visible", true).toBool());
    gSettings->endGroup();
    on_actionReiterBrauuebersicht_triggered(ui->actionReiterBrauuebersicht->isChecked());

    mTabIndexDatenbank = ui->tabMain->indexOf(ui->tabDatenbank);
    gSettings->beginGroup("TabDatenbank");
    ui->actionReiterDatenbank->setChecked(gSettings->value("visible", false).toBool());
    gSettings->endGroup();
    on_actionReiterDatenbank_triggered(ui->actionReiterDatenbank->isChecked());

    gSettings->beginGroup("General");
    ui->actionBestaetigungBeenden->setChecked(gSettings->value("BeendenAbfrage", true).toBool());
    ui->actionCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
    ui->actionTooltips->setChecked(gSettings->value("TooltipsEnabled", true).toBool());
    BierCalc::faktorPlatoToBrix = gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble();
    gSettings->endGroup();
    ui->actionAnimationen->setChecked(gSettings->animationsEnabled());

    connect(ui->tabSudAuswahl, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));
    ui->tabBrauuebersicht->setModel(ui->tabSudAuswahl->model());
    connect(ui->tabBrauuebersicht, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()));
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(sudDataChanged(const QModelIndex&)));

    sudLoaded();

    if (ui->actionCheckUpdate->isChecked())
        checkForUpdate(false);
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (bh->isDirty())
    {
        int ret = QMessageBox::question(this, tr("Anwendung schließen?"),
                                  tr("Sollen die Änderungen vor dem Schließen gespeichert werden?"),
                                  QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            save();
            event->accept();
        }
        else if (ret == QMessageBox::No)
            event->accept();
        else
            event->ignore();
    }
    else
    {
        int ret = QMessageBox::Yes;
        if (ui->actionBestaetigungBeenden->isChecked())
        {
            ret = QMessageBox::question(this, tr("Anwendung schließen?"),
                                  tr("Soll die Anwendung geschlossen werden?"),
                                  QMessageBox::Cancel | QMessageBox::Yes,
                                  QMessageBox::Yes);
        }
        if (ret == QMessageBox::Yes)
            event->accept();
        else
            event->ignore();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    QMainWindow::keyPressEvent(event);
    int n = event->key() - Qt::Key::Key_F1;
    if (n >= 0 && n < ui->tabMain->count())
    {
        if (ui->tabMain->isTabEnabled(n))
            ui->tabMain->setCurrentIndex(n);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip &&!ui->actionTooltips->isChecked() )
        return true;
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::restart(int retCode)
{
    if (bh->isDirty())
    {
        int ret = QMessageBox::question(this, tr("Änderungen speichern?"),
                                  tr("Sollen die Änderungen gespeichert werden?"),
                                  QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
            save();
        else if (ret == QMessageBox::Cancel)
            return;
    }
    qApp->exit(retCode);
}

void MainWindow::save()
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setFocus();
    try
    {
        if (!bh->save())
        {
            QGuiApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Fehler beim Speichern"), bh->lastError());
        }
        QGuiApplication::restoreOverrideCursor();
    }
    catch (const std::exception& ex)
    {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fehler beim Speichern"), ex.what());
    }
    catch (...)
    {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fehler beim Speichern"), tr("Unbekannter Fehler."));
    }
}

void MainWindow::saveSettings()
{
    gSettings->beginGroup("MainWindow");
    gSettings->setValue("geometry", saveGeometry());
    gSettings->setValue("state", saveState());
    gSettings->endGroup();
    gSettings->beginGroup("TabZusammenfassung");
    gSettings->setValue("visible", ui->actionReiterZusammenfassung->isChecked());
    gSettings->endGroup();
    gSettings->beginGroup("TabEtikette");
    gSettings->setValue("visible", ui->actionReiterEtikette->isChecked());
    gSettings->endGroup();
    gSettings->beginGroup("TabBewertung");
    gSettings->setValue("visible", ui->actionReiterBewertung->isChecked());
    gSettings->endGroup();
    gSettings->beginGroup("TabBrauuebersicht");
    gSettings->setValue("visible", ui->actionReiterBrauuebersicht->isChecked());
    gSettings->endGroup();
    gSettings->beginGroup("TabDatenbank");
    gSettings->setValue("visible", ui->actionReiterDatenbank->isChecked());
    gSettings->endGroup();
    ui->tabSudAuswahl->saveSettings();
    ui->tabBrauuebersicht->saveSettings();
    ui->tabRezept->saveSettings();
    ui->tabBraudaten->saveSettings();
    ui->tabAbfuelldaten->saveSettings();
    ui->tabGaerverlauf->saveSettings();
    ui->tabZusammenfassung->saveSettings();
    ui->tabEtikette->saveSettings();
    ui->tabBewertung->saveSettings();
    ui->tabRohstoffe->saveSettings();
    ui->tabAusruestung->saveSettings();
    ui->tabDatenbank->saveSettings();
}

void MainWindow::restoreView(bool full)
{
    if (full)
        restoreState(mDefaultState);
    ui->tabSudAuswahl->restoreView(full);
    ui->tabBrauuebersicht->restoreView(full);
    ui->tabRezept->restoreView(full);
    ui->tabBraudaten->restoreView(full);
    ui->tabAbfuelldaten->restoreView(full);
    ui->tabGaerverlauf->restoreView(full);
    ui->tabZusammenfassung->restoreView(full);
    ui->tabEtikette->restoreView(full);
    ui->tabBewertung->restoreView(full);
    ui->tabRohstoffe->restoreView(full);
    ui->tabAusruestung->restoreView(full);
    ui->tabDatenbank->restoreView(full);
    DlgRohstoffAuswahl::restoreView(full);
    DlgTableView::restoreView(full);
}

void MainWindow::databaseModified()
{
    bool modified = bh->isDirty();
    QString title;
    if (modified)
        title = "* ";
    title += QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion();
    if (bh->sud()->isLoaded())
        title += " - " + bh->sud()->getSudname();
    setWindowTitle(title);
    ui->actionSpeichern->setEnabled(modified);
    ui->actionVerwerfen->setEnabled(modified);
}

void MainWindow::updateValues()
{
    bool loaded = bh->sud()->isLoaded();
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    databaseModified();
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabRezept), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBraudaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabAbfuelldaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabGaerverlauf), loaded);
    if (ui->actionReiterZusammenfassung->isChecked())
        ui->tabMain->setTabEnabled(mTabIndexZusammenfassung, loaded);
    if (ui->actionReiterEtikette->isChecked())
        ui->tabMain->setTabEnabled(mTabIndexEtikette, loaded);
    if (ui->actionReiterBewertung->isChecked())
        ui->tabMain->setTabEnabled(mTabIndexBewertung, loaded);
    ui->menuSud->setEnabled(loaded);
    ui->actionSudGebraut->setEnabled(status >= Brauhelfer::SudStatus::Gebraut);
    ui->actionSudAbgefuellt->setEnabled(status >= Brauhelfer::SudStatus::Abgefuellt);
    ui->actionSudVerbraucht->setEnabled(status >= Brauhelfer::SudStatus::Verbraucht);
    ui->actionHefeZugabeZuruecksetzen->setEnabled(status == Brauhelfer::SudStatus::Gebraut);
    ui->actionWeitereZutaten->setEnabled(status == Brauhelfer::SudStatus::Gebraut);
    if (ui->actionReiterZusammenfassung->isChecked())
        ui->tabMain->setTabText(mTabIndexZusammenfassung, status == Brauhelfer::SudStatus::Rezept && loaded ? tr("Spickzettel") : tr("Zusammenfassung"));
    if (!ui->tabMain->currentWidget()->isEnabled())
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
    ui->actionEingabefelderEntsperren->setChecked(false);
}

void MainWindow::sudLoaded()
{
    updateValues();
    if (bh->sud()->isLoaded())
    {
        if (ui->tabMain->currentWidget() == ui->tabSudAuswahl || ui->tabMain->currentWidget() == ui->tabBrauuebersicht)
            ui->tabMain->setCurrentWidget(ui->tabRezept);
    }
}

void MainWindow::sudDataChanged(const QModelIndex& index)
{
    if (index.column() == ModelSud::ColStatus)
        updateValues();
}

void MainWindow::loadSud(int sudId)
{
    if (bh->sud()->id() == sudId)
    {
        ui->tabMain->setCurrentWidget(ui->tabRezept);
    }
    else
    {
        try
        {
            bh->sud()->load(sudId);
        }
        catch (const std::exception& ex)
        {
            QMessageBox::critical(this, tr("Fehler beim Laden"), ex.what());
        }
        catch (...)
        {
            QMessageBox::critical(this, tr("Fehler beim Laden"), tr("Unbekannter Fehler."));
        }
    }
}

void MainWindow::on_tabMain_currentChanged()
{
    TabAbstract* tab;
    for (int i = 0; i < ui->tabMain->count(); ++i)
    {
        tab = dynamic_cast<TabAbstract*>(ui->tabMain->widget(i));
        if (tab)
            tab->setTabActive(false);
    }
    tab = dynamic_cast<TabAbstract*>(ui->tabMain->currentWidget());
    if (tab)
        tab->setTabActive(true);
    setFocus();
}

void MainWindow::on_actionNeuen_Sud_anlegen_triggered()
{
    ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
    ui->tabSudAuswahl->sudAnlegen();
}

void MainWindow::on_actionSud_kopieren_triggered()
{
    if (ui->tabMain->currentWidget() == ui->tabSudAuswahl)
    {
        ui->tabSudAuswahl->sudKopieren();
    }
    else
    {
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
        ui->tabSudAuswahl->sudKopieren(true);
    }
}

void MainWindow::on_actionSud_teilen_triggered()
{
    if (ui->tabMain->currentWidget() == ui->tabSudAuswahl)
    {
        ui->tabSudAuswahl->sudTeilen();
    }
    else
    {
        ui->tabSudAuswahl->sudTeilen(true);
    }
}

void MainWindow::on_actionSud_l_schen_triggered()
{
    if (ui->tabMain->currentWidget() == ui->tabSudAuswahl)
    {
        ui->tabSudAuswahl->sudLoeschen();
    }
    else
    {
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
        ui->tabSudAuswahl->sudLoeschen(true);
    }
}

void MainWindow::on_actionRezept_importieren_triggered()
{
    ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
    ui->tabSudAuswahl->rezeptImportieren();
}

void MainWindow::on_actionRezept_exportieren_triggered()
{
    if (ui->tabMain->currentWidget() == ui->tabSudAuswahl)
    {
        ui->tabSudAuswahl->rezeptExportieren();
    }
    else
    {
        ui->tabSudAuswahl->rezeptExportieren(true);
    }
}

void MainWindow::on_actionSpeichern_triggered()
{
    save();
}

void MainWindow::on_actionVerwerfen_triggered()
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setFocus();
    try
    {
        bh->discard();
        QGuiApplication::restoreOverrideCursor();
    }
    catch (const std::exception& ex)
    {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fehler beim Verwerfen"), ex.what());
    }
    catch (...)
    {
        QGuiApplication::restoreOverrideCursor();
        QMessageBox::critical(this, tr("Fehler beim Verwerfen"), tr("Unbekannter Fehler."));
    }
}

void MainWindow::on_actionDruckvorschau_triggered()
{
    TabAbstract* tab = dynamic_cast<TabAbstract*>(ui->tabMain->currentWidget());
    if (tab)
        tab->printPreview();
}

void MainWindow::on_actionDrucken_triggered()
{
    TabAbstract* tab = dynamic_cast<TabAbstract*>(ui->tabMain->currentWidget());
    if (tab)
        tab->toPdf();
}

void MainWindow::on_actionBereinigen_triggered()
{
    DlgDatabaseCleaner dlg(this);
    dlg.exec();
}

void MainWindow::on_actionBeenden_triggered()
{
    close();
}

void MainWindow::on_actionSudGebraut_triggered()
{
    bh->sud()->setStatus(static_cast<int>(Brauhelfer::SudStatus::Rezept));
    if (bh->sud()->modelSchnellgaerverlauf()->rowCount() == 1)
        bh->sud()->modelSchnellgaerverlauf()->removeRow(0);
    if (bh->sud()->modelHauptgaerverlauf()->rowCount() == 1)
        bh->sud()->modelHauptgaerverlauf()->removeRow(0);
    if (bh->sud()->modelNachgaerverlauf()->rowCount() == 1)
        bh->sud()->modelNachgaerverlauf()->removeRow(0);
    DlgRohstoffeAbziehen dlg(false, this);
    dlg.exec();
}

void MainWindow::on_actionSudAbgefuellt_triggered()
{
    bh->sud()->setStatus(static_cast<int>(Brauhelfer::SudStatus::Gebraut));
    if (bh->sud()->modelNachgaerverlauf()->rowCount() == 1)
        bh->sud()->modelNachgaerverlauf()->removeRow(0);
}

void MainWindow::on_actionSudVerbraucht_triggered()
{
    bh->sud()->setStatus(static_cast<int>(Brauhelfer::SudStatus::Abgefuellt));
}

void MainWindow::on_actionHefeZugabeZuruecksetzen_triggered()
{
    ProxyModel *model = bh->sud()->modelHefegaben();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        bool zugegeben = model->data(row, ModelHefegaben::ColZugegeben).toBool();
        if (zugegeben)
        {
            model->setData(row, ModelHefegaben::ColZugegeben, false);
            DlgRohstoffeAbziehen dlg(false, Brauhelfer::RohstoffTyp::Hefe,
                                     model->data(row, ModelHefegaben::ColName).toString(),
                                     model->data(row, ModelHefegaben::ColMenge).toDouble(),
                                     this);
            dlg.exec();
        }
    }
}

void MainWindow::on_actionWeitereZutaten_triggered()
{
    ProxyModel *model = bh->sud()->modelWeitereZutatenGaben();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        Brauhelfer::ZusatzStatus status = static_cast<Brauhelfer::ZusatzStatus>(model->data(row, ModelWeitereZutatenGaben::ColZugabestatus).toInt());
        bool zugegeben = status != Brauhelfer::ZusatzStatus::NichtZugegeben;
        if (zugegeben)
        {
            model->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben));
            DlgRohstoffeAbziehen dlg(false, Brauhelfer::RohstoffTyp::Zusatz,
                                     model->data(row, ModelWeitereZutatenGaben::ColName).toString(),
                                     model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                                     this);
            dlg.exec();
        }
    }
}

void MainWindow::on_actionEingabefelderEntsperren_changed()
{
    if (ui->actionEingabefelderEntsperren->isChecked())
    {
        int ret = QMessageBox::question(this, tr("Eingabefelder entsperren?"),
                                  tr("Vorsicht! Eingabefelder entsperren kann zu inkonsistenten Daten führen und sollte mit Bedacht eingesetzt werden."),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            gSettings->ForceEnabled = true;
            ui->tabRezept->checkEnabled();
            ui->tabBraudaten->checkEnabled();
            ui->tabAbfuelldaten->checkEnabled();
            ui->tabGaerverlauf->checkEnabled();
        }
        else
        {
            ui->actionEingabefelderEntsperren->setChecked(false);
        }
    }
    else
    {
        gSettings->ForceEnabled = false;
        ui->tabRezept->checkEnabled();
        ui->tabBraudaten->checkEnabled();
        ui->tabAbfuelldaten->checkEnabled();
        ui->tabGaerverlauf->checkEnabled();
    }
}

void MainWindow::on_actionWiederherstellen_triggered()
{
    restoreView(true);
}

void MainWindow::on_actionThemeSystem_triggered()
{
    gSettings->setTheme(Settings::Theme::System);
    restart();
}

void MainWindow::on_actionThemeHell_triggered()
{
    gSettings->setTheme(Settings::Theme::Bright);
    restart();
}

void MainWindow::on_actionThemeDunkel_triggered()
{
    gSettings->setTheme(Settings::Theme::Dark);
    restart();
}

void MainWindow::changeStyle()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
    {
        gSettings->setStyle(action->text());
        restart();
    }
}

void MainWindow::on_actionSchriftart_triggered(bool checked)
{
    if (checked)
    {
        gSettings->setUseSystemFont(true);
        restart();
    }
    else
    {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, gSettings->font, this);
        if (ok)
        {
            gSettings->setUseSystemFont(false);
            gSettings->setFont(font);
            setFont(font);
            restart();
        }
        else
        {
            ui->actionSchriftart->setChecked(true);
        }
    }
}

void MainWindow::on_actionOeffnen_triggered()
{
    QString databasePath = QFileDialog::getOpenFileName(this, tr("Datenbank auswählen"),
                                                    gSettings->databasePath(),
                                                    tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
    if (!databasePath.isEmpty())
    {
        gSettings->setDatabasePath(databasePath);
        restart();
    }
}

void MainWindow::on_actionReiterZusammenfassung_triggered(bool checked)
{
    if (checked)
    {
        bool loaded = bh->sud()->isLoaded();
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
        ui->tabMain->insertTab(mTabIndexZusammenfassung, ui->tabZusammenfassung, status == Brauhelfer::SudStatus::Rezept && loaded ? tr("Spickzettel") : tr("Zusammenfassung"));
        ui->tabMain->setTabEnabled(mTabIndexZusammenfassung, loaded);
        mTabIndexEtikette++;
        mTabIndexBewertung++;
        mTabIndexBrauuebersicht++;
        mTabIndexDatenbank++;
    }
    else
    {
        ui->tabMain->removeTab(mTabIndexZusammenfassung);
        mTabIndexEtikette--;
        mTabIndexBewertung--;
        mTabIndexBrauuebersicht--;
        mTabIndexDatenbank--;
    }
}

void MainWindow::on_actionReiterEtikette_triggered(bool checked)
{
    if (checked)
    {
        ui->tabMain->insertTab(mTabIndexEtikette, ui->tabEtikette, tr("Etikette"));
        ui->tabMain->setTabEnabled(mTabIndexEtikette, bh->sud()->isLoaded());
        mTabIndexBewertung++;
        mTabIndexBrauuebersicht++;
        mTabIndexDatenbank++;
    }
    else
    {
        ui->tabMain->removeTab(mTabIndexEtikette);
        mTabIndexBewertung--;
        mTabIndexBrauuebersicht--;
        mTabIndexDatenbank--;
    }
}

void MainWindow::on_actionReiterBewertung_triggered(bool checked)
{
    if (checked)
    {
        ui->tabMain->insertTab(mTabIndexBewertung, ui->tabBewertung, tr("Bewertung"));
        ui->tabMain->setTabEnabled(mTabIndexBewertung, bh->sud()->isLoaded());
        mTabIndexBrauuebersicht++;
        mTabIndexDatenbank++;
    }
    else
    {
        ui->tabMain->removeTab(mTabIndexBewertung);
        mTabIndexBrauuebersicht--;
        mTabIndexDatenbank--;
    }
}

void MainWindow::on_actionReiterBrauuebersicht_triggered(bool checked)
{
    if (checked)
    {
        ui->tabMain->insertTab(mTabIndexBrauuebersicht, ui->tabBrauuebersicht, tr("Brauübersicht"));
        mTabIndexDatenbank++;
    }
    else
    {
        ui->tabMain->removeTab(mTabIndexBrauuebersicht);
        mTabIndexDatenbank--;
    }
}

void MainWindow::on_actionReiterDatenbank_triggered(bool checked)
{
    if (checked)
        ui->tabMain->insertTab(mTabIndexDatenbank, ui->tabDatenbank, tr("Datenbank"));
    else
        ui->tabMain->removeTab(mTabIndexDatenbank);
}

void MainWindow::on_actionBestaetigungBeenden_triggered(bool checked)
{
    gSettings->beginGroup("General");
    gSettings->setValue("BeendenAbfrage", checked);
    gSettings->endGroup();
}

void MainWindow::checkForUpdate(bool force)
{
  #if QT_NETWORK_LIB
    QString url;
    QDate since;
    gSettings->beginGroup("General");
    url = gSettings->value("CheckUpdateUrl", URL_CHEKUPDATE).toString();
    if (!force)
        since = gSettings->value("CheckUpdateLastDate").toDate();
    gSettings->endGroup();

    DlgCheckUpdate *dlg = new DlgCheckUpdate(url, since, this);
    connect(dlg, SIGNAL(checkUpdatefinished()), this, SLOT(checkMessageFinished()));
    dlg->checkForUpdate();
  #else
    Q_UNUSED(force)
  #endif
}

void MainWindow::checkMessageFinished()
{
  #if QT_NETWORK_LIB
    DlgCheckUpdate *dlg = qobject_cast<DlgCheckUpdate*>(sender());
    if (dlg)
    {
        if (dlg->hasUpdate())
        {
            dlg->exec();
            gSettings->beginGroup("General");
            if (dlg->ignoreUpdate())
                gSettings->setValue("CheckUpdateLastDate", QDate::currentDate());
            gSettings->setValue("CheckUpdate", dlg->doCheckUpdate());
            ui->actionCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
            gSettings->endGroup();
        }
        dlg->deleteLater();
    }
  #endif
}

void MainWindow::on_actionCheckUpdate_triggered(bool checked)
{
  #if QT_NETWORK_LIB
    gSettings->beginGroup("General");
    gSettings->setValue("CheckUpdate", checked);
    gSettings->endGroup();
    if (checked)
        checkForUpdate(true);
  #else
    Q_UNUSED(checked)
  #endif
}

void MainWindow::on_actionTooltips_triggered(bool checked)
{
    gSettings->beginGroup("General");
    gSettings->setValue("TooltipsEnabled", checked);
    gSettings->endGroup();
}

void MainWindow::on_actionAnimationen_triggered(bool checked)
{
    gSettings->setAnimationsEnabled(checked);
}

void MainWindow::on_actionDeutsch_triggered()
{
    gSettings->setLanguage("de");
    restart(1001);
}

void MainWindow::on_actionEnglisch_triggered()
{
    gSettings->setLanguage("en");
    restart(1001);
}

void MainWindow::on_actionSchwedisch_triggered()
{
    gSettings->setLanguage("sv");
    restart(1001);
}

void MainWindow::on_actionNiederlaendisch_triggered()
{
    gSettings->setLanguage("nl");
    restart(1001);
}

void MainWindow::on_actionSpende_triggered()
{
    QDesktopServices::openUrl(QUrl(URL_SPENDE));
}

void MainWindow::on_actionBierspende_triggered()
{
    DlgBierspende dlg(this);
    dlg.exec();
}

void MainWindow::on_actionUeber_triggered()
{
    DlgAbout dlg(this);
    dlg.exec();
}

void MainWindow::on_actionLog_triggered()
{
    if (DlgConsole::Dialog)
    {
        DlgConsole::Dialog->raise();
        DlgConsole::Dialog->activateWindow();
    }
    else
    {
        DlgConsole::Dialog = new DlgConsole(this);
        DlgConsole::Dialog->show();
    }
}
