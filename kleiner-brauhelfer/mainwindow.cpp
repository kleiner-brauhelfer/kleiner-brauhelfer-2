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
#include "dialogs/dlgabout.h"
#include "dialogs/dlgmessage.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
    mDefaultGeometry = saveGeometry();
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
    ui->actionCheckUpdate->setChecked(gSettings->value("CheckUpdates", true).toBool());
    gSettings->endGroup();

    ui->statusBar->showMessage(bh->databasePath());

    connect(ui->tabContentSudAuswahl, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));
    ui->tabContentBrauUebersicht->setModel(ui->tabContentSudAuswahl->model());
    connect(ui->tabContentBrauUebersicht, SIGNAL(clicked(int)), this, SLOT(loadSud(int)));

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()));
    connect(bh, SIGNAL(discarded()), this, SLOT(discarded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->modelSud(), SIGNAL(modified()), this, SLOT(sudModified()));

    sudLoaded();

    if (gSettings->isNewProgramVersion())
        restoreView(true);

    if (ui->actionCheckUpdate->isChecked())
        checkMessage();
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
        int ret = QMessageBox::question(this, tr("Anwendung schliessen?"),
                                  tr("Sollen die Änderungen vor dem Schliessen gespeichert werden?"),
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
    setFocus();
    bh->save();
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
    ui->tabContentSudAuswahl->saveSettings();
    ui->tabContentBrauUebersicht->saveSettings();
    ui->tabContentRezept->saveSettings();
    ui->tabContentBraudaten->saveSettings();
    ui->tabContentAbfuelldaten->saveSettings();
    ui->tabContentGaerverlauf->saveSettings();
    ui->tabContentZusammenfasszung->saveSettings();
    ui->tabContentEtikette->saveSettings();
    ui->tabContentBewertung->saveSettings();
    ui->tabContentRohstoffe->saveSettings();
    ui->tabContentAusruestung->saveSettings();
    ui->tabContentDatenbank->saveSettings();
}

void MainWindow::restoreView(bool onUpdate)
{
    if (!onUpdate)
    {
        QPoint position = pos();
        restoreGeometry(mDefaultGeometry);
        restoreState(mDefaultState);
        move(position);
    }
    ui->tabContentSudAuswahl->restoreView();
    ui->tabContentBrauUebersicht->restoreView();
    ui->tabContentRezept->restoreView();
    ui->tabContentBraudaten->restoreView();
    ui->tabContentAbfuelldaten->restoreView();
    ui->tabContentGaerverlauf->restoreView();
    ui->tabContentZusammenfasszung->restoreView();
    ui->tabContentEtikette->restoreView();
    ui->tabContentBewertung->restoreView();
    ui->tabContentRohstoffe->restoreView();
    ui->tabContentAusruestung->restoreView();
    ui->tabContentDatenbank->restoreView();
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

void MainWindow::discarded()
{
    bool loaded = bh->sud()->isLoaded();
    databaseModified();
    sudModified();
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabRezept), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBraudaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabAbfuelldaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabGaerverlauf), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabZusammenfassung), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabEtikette), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBewertung), loaded);
    ui->tabMain->setTabText(ui->tabMain->indexOf(ui->tabZusammenfassung),
                            bh->sud()->getStatus() == Sud_Status_Rezept && loaded ? tr("Spickzettel") : tr("Zusammenfassung"));
    if (!ui->tabMain->currentWidget()->isEnabled())
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
}

void MainWindow::sudLoaded()
{
    discarded();
    if (bh->sud()->isLoaded())
    {
        if (ui->tabMain->currentWidget() == ui->tabSudAuswahl || ui->tabMain->currentWidget() == ui->tabBrauUebersicht)
            ui->tabMain->setCurrentWidget(ui->tabRezept);
    }
}

void MainWindow::sudModified()
{
    if (bh->sud()->isLoaded())
    {
        int status = bh->sud()->getStatus();
        ui->menuSud->setEnabled(true);
        ui->actionSudGebraut->setEnabled(status >= Sud_Status_Gebraut);
        ui->actionSudAbgefuellt->setEnabled(status >= Sud_Status_Abgefuellt);
        ui->actionSudVerbraucht->setEnabled(status >= Sud_Status_Verbraucht);
        ui->actionHefeZugabeZuruecksetzen->setEnabled(status >= Sud_Status_Gebraut);
        ui->actionWeitereZutaten->setEnabled(status >= Sud_Status_Gebraut);
    }
    else
    {
        ui->menuSud->setEnabled(false);
    }
}

void MainWindow::loadSud(int sudId)
{
    if (bh->sud()->id() == sudId)
        ui->tabMain->setCurrentWidget(ui->tabRezept);
    else
        bh->sud()->load(sudId);
}

void MainWindow::on_tabMain_currentChanged()
{
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
    setFocus();
    bh->discard();
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
    int col = model->fieldIndex("Zugegeben");
    for (int row = 0; row < model->rowCount(); ++row)
        model->setData(model->index(row, col), 0);
}

void MainWindow::on_actionWeitereZutaten_triggered()
{
    ProxyModel *model = bh->sud()->modelWeitereZutatenGaben();
    int col = model->fieldIndex("Zugabestatus");
    for (int row = 0; row < model->rowCount(); ++row)
        model->setData(model->index(row, col), EWZ_Zugabestatus_nichtZugegeben);
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

void MainWindow::checkMessage()
{
    DlgMessage *dlg = new DlgMessage(this, URL_MESSAGE);
    connect(dlg, SIGNAL(finished()), this, SLOT(checkMessageFinished()));
    dlg->getMessage();
}

void MainWindow::checkMessageFinished()
{
    DlgMessage *dlg = qobject_cast<DlgMessage*>(sender());
    if (dlg)
    {
        if (dlg->hasMessage())
            dlg->exec();
        dlg->deleteLater();
    }
}

void MainWindow::on_actionCheckUpdate_triggered(bool checked)
{
    gSettings->beginGroup("General");
    gSettings->setValue("CheckUpdates", checked);
    gSettings->endGroup();
    if (checked)
        checkMessage();
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
