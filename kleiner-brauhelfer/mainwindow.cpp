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
#include "dialogs/dlgabout.h"
#include "dialogs/dlgcheckupdate.h"
#include "dialogs/dlgdatabasecleaner.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->installEventFilter(this);

    mTabIndexDatenbank = ui->tabMain->indexOf(ui->tabDatenbank);

    ui->actionThemeSystem->setEnabled(gSettings->theme() != Settings::Theme::System);
    ui->actionThemeHell->setEnabled(gSettings->theme() != Settings::Theme::Bright);
    ui->actionThemeDunkel->setEnabled(gSettings->theme() != Settings::Theme::Dark);

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

    ui->actionSchriftart->setChecked(gSettings->useSystemFont());

    gSettings->beginGroup("MainWindow");
    restoreGeometry(gSettings->value("geometry").toByteArray());
    mDefaultState = saveState();
    restoreState(gSettings->value("state").toByteArray());
    gSettings->endGroup();

    gSettings->beginGroup("TabDatenbank");
    ui->actionReiterDatenbank->setChecked(gSettings->value("visible", false).toBool());
    gSettings->endGroup();
    on_actionReiterDatenbank_triggered(ui->actionReiterDatenbank->isChecked());

    gSettings->beginGroup("General");
    ui->actionBestaetigungBeenden->setChecked(gSettings->value("BeendenAbfrage", true).toBool());
    ui->actionCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
    ui->actionTooltips->setChecked(gSettings->value("TooltipsEnabled", true).toBool());
    BierCalc::faktorBrixToPlato = gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble();
    gSettings->endGroup();

    ui->statusBar->showMessage(bh->databasePath());

    connect(ui->tabSudAuswahl, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));
    ui->tabBrauUebersicht->setModel(ui->tabSudAuswahl->model());
    connect(ui->tabBrauUebersicht, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()));
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(sudDataChanged(const QModelIndex&)));

    sudLoaded();

    if (gSettings->isNewProgramVersion())
        restoreView(true);

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
            ret = QMessageBox::question(this, tr("Anwendung schliessen?"),
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

void MainWindow::restart()
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
    qApp->exit(1000);
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
    gSettings->beginGroup("TabDatenbank");
    gSettings->setValue("visible", ui->actionReiterDatenbank->isChecked());
    gSettings->endGroup();
    ui->tabSudAuswahl->saveSettings();
    ui->tabBrauUebersicht->saveSettings();
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

void MainWindow::restoreView(bool onUpdate)
{
    if (!onUpdate)
    {
        restoreState(mDefaultState);
    }
    ui->tabSudAuswahl->restoreView();
    ui->tabBrauUebersicht->restoreView();
    ui->tabRezept->restoreView();
    ui->tabBraudaten->restoreView();
    ui->tabAbfuelldaten->restoreView();
    ui->tabGaerverlauf->restoreView();
    ui->tabZusammenfassung->restoreView();
    ui->tabEtikette->restoreView();
    ui->tabBewertung->restoreView();
    ui->tabRohstoffe->restoreView();
    ui->tabAusruestung->restoreView();
    ui->tabDatenbank->restoreView();
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
    int status = bh->sud()->getStatus();
    databaseModified();
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabRezept), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBraudaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabAbfuelldaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabGaerverlauf), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabZusammenfassung), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabEtikette), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBewertung), loaded);
    ui->menuSud->setEnabled(loaded);
    ui->actionSudGebraut->setEnabled(status >= Sud_Status_Gebraut);
    ui->actionSudAbgefuellt->setEnabled(status >= Sud_Status_Abgefuellt);
    ui->actionSudVerbraucht->setEnabled(status >= Sud_Status_Verbraucht);
    ui->actionHefeZugabeZuruecksetzen->setEnabled(status == Sud_Status_Gebraut);
    ui->actionWeitereZutaten->setEnabled(status == Sud_Status_Gebraut);
    ui->tabMain->setTabText(ui->tabMain->indexOf(ui->tabZusammenfassung),
                            status == Sud_Status_Rezept && loaded ? tr("Spickzettel") : tr("Zusammenfassung"));
    if (!ui->tabMain->currentWidget()->isEnabled())
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
    ui->actionEingabefelderEntsperren->setChecked(false);
}

void MainWindow::sudLoaded()
{
    updateValues();
    if (bh->sud()->isLoaded())
    {
        if (ui->tabMain->currentWidget() == ui->tabSudAuswahl || ui->tabMain->currentWidget() == ui->tabBrauUebersicht)
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

void MainWindow::on_actionOeffnen_triggered()
{
    QString databasePath = QFileDialog::getOpenFileName(this, tr("Datenbankdatei auswählen"),
                                                    gSettings->databasePath(),
                                                    tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
    if (!databasePath.isEmpty())
    {
        gSettings->setDatabasePath(databasePath);
        restart();
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
    bh->sud()->setStatus(Sud_Status_Rezept);
}

void MainWindow::on_actionSudAbgefuellt_triggered()
{
    bh->sud()->setStatus(Sud_Status_Gebraut);
}

void MainWindow::on_actionSudVerbraucht_triggered()
{
    bh->sud()->setStatus(Sud_Status_Abgefuellt);
}

void MainWindow::on_actionHefeZugabeZuruecksetzen_triggered()
{
    ProxyModel *model = bh->sud()->modelHefegaben();
    for (int row = 0; row < model->rowCount(); ++row)
        model->setData(row, ModelHefegaben::ColZugegeben, 0);
}

void MainWindow::on_actionWeitereZutaten_triggered()
{
    ProxyModel *model = bh->sud()->modelWeitereZutatenGaben();
    for (int row = 0; row < model->rowCount(); ++row)
        model->setData(row, ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_nichtZugegeben);
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
    restoreView();
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
        }
        else
        {
            ui->actionSchriftart->setChecked(true);
        }
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
    QString url;
    QDate since;
    gSettings->beginGroup("General");
    url = gSettings->value("CheckUpdateUrl", URL_CHEKUPDATE).toString();
    if (!force)
        since = gSettings->value("CheckUpdateLastDate").toDate();
    gSettings->endGroup();

    DlgCheckUpdate *dlg = new DlgCheckUpdate(url, since, this);
    connect(dlg, SIGNAL(finished()), this, SLOT(checkMessageFinished()));
    dlg->checkForUpdate();
}

void MainWindow::checkMessageFinished()
{
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
}

void MainWindow::on_actionCheckUpdate_triggered(bool checked)
{
    gSettings->beginGroup("General");
    gSettings->setValue("CheckUpdate", checked);
    gSettings->endGroup();
    if (checked)
        checkForUpdate(true);
}

void MainWindow::on_actionTooltips_triggered(bool checked)
{
    gSettings->beginGroup("General");
    gSettings->setValue("TooltipsEnabled", checked);
    gSettings->endGroup();
}

void MainWindow::on_actionSpende_triggered()
{
    QDesktopServices::openUrl(QUrl(URL_SPENDE));
}

void MainWindow::on_actionUeber_triggered()
{
    DlgAbout dlg(this);
    dlg.exec();
}
