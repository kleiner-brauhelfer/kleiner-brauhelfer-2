#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QStyleFactory>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "commands/undostack.h"
#include "biercalc.h"
#include "definitionen.h"
#include "tababstract.h"
#include "dialogs/dlgabout.h"
#include "dialogs/dlgausruestung.h"
#include "dialogs/dlgbrauuebersicht.h"
#include "dialogs/dlgcheckupdate.h"
#include "dialogs/dlgdatabasecleaner.h"
#include "dialogs/dlgdatenbank.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgtableview.h"
#include "dialogs/dlgmodule.h"
#include "dialogs/dlgconsole.h"
#include "dialogs/dlgrohstoffe.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "dialogs/dlghilfe.h"
#include "widgets/iconthemed.h"
#include "widgets/widgetdecorator.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

UndoStack* gUndoStack = nullptr;

QStringList MainWindow::HopfenTypname;
QStringList MainWindow::HefeTypname;
QStringList MainWindow::HefeTypFlTrName;
QStringList MainWindow::ZusatzTypname;
QStringList MainWindow::Einheiten;

MainWindow* MainWindow::getInstance()
{
    for (QWidget* wdg: qApp->topLevelWidgets())
        if (MainWindow* mainWin = qobject_cast<MainWindow*>(wdg))
            return mainWin;
    return nullptr;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initLabels();
    ui->setupUi(this);
    qApp->installEventFilter(this);

    Qt::ColorScheme theme = gSettings->theme();
    ui->actionThemeHell->setEnabled(theme != Qt::ColorScheme::Light);
    ui->actionThemeDunkel->setEnabled(theme != Qt::ColorScheme::Dark);
    if (theme == Qt::ColorScheme::Dark)
    {
        ui->tabMain->setTabIcon(0, IconThemed(QStringLiteral("tabsudauswahl"), false));
        ui->tabMain->setTabIcon(1, IconThemed(QStringLiteral("tabrezept"), false));
        ui->tabMain->setTabIcon(2, IconThemed(QStringLiteral("tabbraudaten"), false));
        ui->tabMain->setTabIcon(3, IconThemed(QStringLiteral("tababfuellen"), false));
        ui->tabMain->setTabIcon(4, IconThemed(QStringLiteral("tabgaerverlauf"), false));
        ui->tabMain->setTabIcon(5, IconThemed(QStringLiteral("tabzusammenfassung"), false));
        ui->tabMain->setTabIcon(6, IconThemed(QStringLiteral("tabetikette"), false));
        ui->tabMain->setTabIcon(7, IconThemed(QStringLiteral("tabbewertung"), false));

        const QList<QAction*> actions = findChildren<QAction*>();
        for (QAction* action : actions)
        {
            QString name = action->whatsThis();
            QIcon icon = action->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                action->setIcon(icon);
            }
        }
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    gUndoStack = new UndoStack(this);
    gUndoStack->setEnabled(gSettings->valueInGroup("General", "UndoEnabled", true).toBool());
    if (gUndoStack->isEnabled())
    {
        ui->menuBearbeiten->addAction(gUndoStack->createUndoAction(this, tr("Rückgängig")));
        ui->menuBearbeiten->addAction(gUndoStack->createRedoAction(this, tr("Wiederherstellen")));
    }
    else
    {
        ui->menuBearbeiten->setEnabled(false);
    }

  #if 0
    QString style = gSettings->style();
    for(const QString &key : QStyleFactory::keys())
    {
        QAction *action = new QAction(key, this);
        if (key == style)
            action->setEnabled(false);
        else
            connect(action, &QAction::triggered, this, &MainWindow::changeStyle);
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

    gSettings->beginGroup("General");
    ui->actionBestaetigungBeenden->setChecked(gSettings->value("BeendenAbfrage", true).toBool());
    ui->actionCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
    ui->actionTooltips->setChecked(gSettings->value("TooltipsEnabled", true).toBool());
    ui->actionZahlenformat->setChecked(gSettings->value("UseLanguageLocale", false).toBool());
    BierCalc::faktorPlatoToBrix = gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble();
    gSettings->endGroup();
    ui->actionAnimationen->setChecked(gSettings->animationsEnabled());

    connect(qApp, &QApplication::focusChanged, this, [](QWidget *old, QWidget *now){
        if (!old || !now)
            return;
        if (strcmp(old->metaObject()->className(),"QtPrivate::QCalendarView") == 0)
            return;
        WidgetDecorator::clearValueChanged();
    });

    connect(gSettings, &Settings::modulesChanged, this, &MainWindow::modulesChanged);

    connect(ui->tabSudAuswahl, &TabSudAuswahl::clicked, this, &MainWindow::loadSud);

    connect(bh, &Brauhelfer::modified, this, &MainWindow::databaseModified);
    connect(bh, &Brauhelfer::discarded, this, &MainWindow::updateValues);
    connect(bh->sud(), &SudObject::loadedChanged, this, &MainWindow::sudLoaded);
    connect(bh->sud(), &SudObject::dataChanged, this, &MainWindow::sudDataChanged);

    connect(ui->actionRohstoffe, &QAction::triggered, this, &MainWindow::showDialogRohstoffe);
    connect(ui->actionBrauUebersicht, &QAction::triggered, this, &MainWindow::showDialogBrauUebersicht);
    connect(ui->actionAusruestung, &QAction::triggered, this, &MainWindow::showDialogAusruestung);

    if (ui->actionCheckUpdate->isChecked())
        checkForUpdate(false);

    if (gSettings->modulesFirstTime)
        on_actionModule_triggered();

    modulesChanged(Settings::ModuleAlle);
    sudLoaded();
    on_tabMain_currentChanged();
}

MainWindow::~MainWindow()
{
    closeDialogs();
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
            saveDatabase();
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

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip && !ui->actionTooltips->isChecked() )
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
            saveDatabase();
        else if (ret == QMessageBox::Cancel)
            return;
    }
    qApp->exit(retCode);
}

void MainWindow::saveDatabase()
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setFocus();
    try
    {
        if (bh->save())
        {
            gUndoStack->clear();
        }
        else
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

void MainWindow::discardDatabase()
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    setFocus();
    try
    {
        WidgetDecorator::focusRequired = false;
        bh->discard();
        WidgetDecorator::focusRequired = true;
        gUndoStack->clear();
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

void MainWindow::saveSettings()
{
    gSettings->beginGroup("MainWindow");
    gSettings->setValue("geometry", saveGeometry());
    gSettings->setValue("state", saveState());
    gSettings->endGroup();
    ui->tabSudAuswahl->saveSettings();
    ui->tabRezept->saveSettings();
    ui->tabBraudaten->saveSettings();
    ui->tabAbfuelldaten->saveSettings();
    ui->tabGaerverlauf->saveSettings();
    ui->tabZusammenfassung->saveSettings();
    ui->tabEtikette->saveSettings();
    ui->tabBewertung->saveSettings();
}

void MainWindow::restoreView()
{
    closeDialogs();
    restoreState(mDefaultState);
    ui->tabSudAuswahl->restoreView();
    ui->tabRezept->restoreView();
    ui->tabBraudaten->restoreView();
    ui->tabAbfuelldaten->restoreView();
    ui->tabGaerverlauf->restoreView();
    ui->tabZusammenfassung->restoreView();
    ui->tabEtikette->restoreView();
    ui->tabBewertung->restoreView();
    DlgModule::restoreView();
    DlgRohstoffe::restoreView();
    DlgAusruestung::restoreView();
    DlgBrauUebersicht::restoreView();
    DlgDatenbank::restoreView();
    DlgRohstoffAuswahl::restoreView();
    DlgTableView::restoreView();
    DlgConsole::restoreView();
    DlgHilfe::restoreView();
}

void MainWindow::closeDialogs()
{
    DlgAbstract::closeDialog<DlgModule>();
    DlgAbstract::closeDialog<DlgConsole>();
    DlgAbstract::closeDialog<DlgBrauUebersicht>();
    DlgAbstract::closeDialog<DlgRohstoffe>();
    DlgAbstract::closeDialog<DlgAusruestung>();
    DlgAbstract::closeDialog<DlgDatenbank>();
    DlgAbstract::closeDialog<DlgHilfe>();
}

void MainWindow::modulesChanged(Settings::Modules modules)
{
    updateTabs(modules);
    updateValues();
    ui->tabSudAuswahl->modulesChanged(modules);
    ui->tabRezept->modulesChanged(modules);
    ui->tabBraudaten->modulesChanged(modules);
    ui->tabAbfuelldaten->modulesChanged(modules);
    ui->tabGaerverlauf->modulesChanged(modules);
    ui->tabZusammenfassung->modulesChanged(modules);
    ui->tabEtikette->modulesChanged(modules);
    ui->tabBewertung->modulesChanged(modules);
    checkLoadedSud();
}

void MainWindow::updateTabs(Settings::Modules modules)
{
    int nextIndex = 4;
    if (modules.testFlag(Settings::ModuleGaerverlauf))
    {
        int index = ui->tabMain->indexOf(ui->tabGaerverlauf);
        if (gSettings->isModuleEnabled(Settings::ModuleGaerverlauf))
        {
            if (index < 0)
                ui->tabMain->insertTab(nextIndex, ui->tabGaerverlauf, IconThemed(QStringLiteral("tabgaerverlauf"), gSettings->theme() == Qt::ColorScheme::Light), tr("Gärverlauf"));
        }
        else
            ui->tabMain->removeTab(index);
    }
    if (gSettings->isModuleEnabled(Settings::ModuleGaerverlauf))
        nextIndex++;
    if (modules.testFlag(Settings::ModuleAusdruck))
    {
        int index = ui->tabMain->indexOf(ui->tabZusammenfassung);
        if (gSettings->isModuleEnabled(Settings::ModuleAusdruck))
        {
            if (index < 0)
                ui->tabMain->insertTab(nextIndex, ui->tabZusammenfassung, IconThemed(QStringLiteral("tabzusammenfassung"), gSettings->theme() == Qt::ColorScheme::Light), tr("Ausdruck"));
        }
        else
            ui->tabMain->removeTab(index);
    }
    if (gSettings->isModuleEnabled(Settings::ModuleAusdruck))
        nextIndex++;
    if (modules.testFlag(Settings::ModuleEtikette))
    {
        int index = ui->tabMain->indexOf(ui->tabEtikette);
        if (gSettings->isModuleEnabled(Settings::ModuleEtikette))
        {
            if (index < 0)
                ui->tabMain->insertTab(nextIndex, ui->tabEtikette, IconThemed(QStringLiteral("tabetikette"), gSettings->theme() == Qt::ColorScheme::Light), tr("Etikett"));
        }
        else
            ui->tabMain->removeTab(index);
    }
    if (gSettings->isModuleEnabled(Settings::ModuleEtikette))
        nextIndex++;
    if (modules.testFlag(Settings::ModuleBewertung))
    {
        int index = ui->tabMain->indexOf(ui->tabBewertung);
        if (gSettings->isModuleEnabled(Settings::ModuleBewertung))
        {
            if (index < 0)
                ui->tabMain->insertTab(nextIndex, ui->tabBewertung, IconThemed(QStringLiteral("tabbewertung"), gSettings->theme() == Qt::ColorScheme::Light), tr("Bewertung"));
        }
        else
            ui->tabMain->removeTab(index);
    }
    if (gSettings->isModuleEnabled(Settings::ModuleBewertung))
        nextIndex++;
    if (modules.testFlag(Settings::ModuleBrauuebersicht))
        ui->actionBrauUebersicht->setVisible(gSettings->isModuleEnabled(Settings::ModuleBrauuebersicht));
    if (modules.testFlag(Settings::ModuleAusruestung))
        ui->actionAusruestung->setVisible(gSettings->isModuleEnabled(Settings::ModuleAusruestung));
}

void MainWindow::databaseModified()
{
    bool modified = bh->isDirty();
    QString title;
    if (modified)
        title = QStringLiteral("* ");
    if (bh->sud()->isLoaded())
        title += bh->sud()->getSudname() + " - ";
    title += QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion();
    setWindowTitle(title);
    ui->actionSpeichern->setEnabled(modified);
    ui->actionVerwerfen->setEnabled(modified);
}

void MainWindow::updateValues()
{
    bool loaded = bh->sud()->isLoaded();
    databaseModified();
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabRezept), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBraudaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabAbfuelldaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabGaerverlauf), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabZusammenfassung), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabEtikette), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBewertung), loaded);
    ui->menuSud->setEnabled(loaded);
    if (loaded)
    {
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
        ui->actionSudGebraut->setEnabled(status >= Brauhelfer::SudStatus::Gebraut);
        ui->actionSudAbgefuellt->setEnabled(status >= Brauhelfer::SudStatus::Abgefuellt);
        ui->actionSudVerbraucht->setEnabled(status >= Brauhelfer::SudStatus::Verbraucht);
        ui->actionHefeZugabeZuruecksetzen->setEnabled(status == Brauhelfer::SudStatus::Gebraut);
        ui->actionWeitereZutaten->setEnabled(status == Brauhelfer::SudStatus::Gebraut);
    }
    if (!ui->tabMain->currentWidget()->isEnabled())
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
    ui->actionEingabefelderEntsperren->setChecked(false);
}

void MainWindow::sudLoaded()
{
    updateValues();
    if (bh->sud()->isLoaded())
    {
        checkLoadedSud();
        if (ui->tabMain->currentWidget() == ui->tabSudAuswahl)
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
    {
        tab->setTabActive(true);
        ui->actionDrucken->setEnabled(tab->isPrintable());
        ui->actionDruckvorschau->setEnabled(tab->isPrintable());
    }
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
    saveDatabase();
}

void MainWindow::on_actionVerwerfen_triggered()
{
    discardDatabase();
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
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(false, Brauhelfer::RohstoffTyp::Hefe,
                                         model->data(row, ModelHefegaben::ColName).toString(),
                                         model->data(row, ModelHefegaben::ColMenge).toDouble(),
                                         this);
                dlg.exec();
            }
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
            if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            {
                DlgRohstoffeAbziehen dlg(false, Brauhelfer::RohstoffTyp::Zusatz,
                                         model->data(row, ModelWeitereZutatenGaben::ColName).toString(),
                                         model->data(row, ModelWeitereZutatenGaben::Colerg_Menge).toDouble(),
                                         this);
                dlg.exec();
            }
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

void MainWindow::on_actionAnsichtWiederherstellen_triggered()
{
    restoreView();
}

void MainWindow::on_actionThemeHell_triggered()
{
    gSettings->setTheme(Qt::ColorScheme::Light);
    restart();
}

void MainWindow::on_actionThemeDunkel_triggered()
{
    gSettings->setTheme(Qt::ColorScheme::Dark);
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

void MainWindow::on_actionBestaetigungBeenden_triggered(bool checked)
{
    gSettings->setValueInGroup("General", "BeendenAbfrage", checked);
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
    connect(dlg, &DlgCheckUpdate::checkUpdatefinished, this, &MainWindow::checkMessageFinished);
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

void MainWindow::initLabels()
{
    SqlTableModel *model;

    HopfenTypname = QStringList({"", tr("aroma"), tr("bitter"), tr("universal")});
    HefeTypname = QStringList({"", tr("obergärig"), tr("untergärig")});
    HefeTypFlTrName = QStringList({"", tr("trocken"), tr("flüssig")});
    ZusatzTypname = QStringList({tr("Honig"), tr("Zucker"), tr("Gewürz"), tr("Frucht"), tr("Sonstiges"), tr("Kraut"), tr("Wasseraufbereitung"), tr("Klärmittel")});
    Einheiten = QStringList({"kg", "g", "mg", tr("Stk."), "L", "mL"});

    model = bh->modelSud();
    model->setHeaderData(ModelSud::ColID, Qt::Horizontal, tr("Sud ID"));
    model->setHeaderData(ModelSud::ColSudname, Qt::Horizontal, tr("Sudname"));
    model->setHeaderData(ModelSud::ColSudnummer, Qt::Horizontal, tr("Sudnummer"));
    model->setHeaderData(ModelSud::ColKategorie, Qt::Horizontal, tr("Kategorie"));
    model->setHeaderData(ModelSud::ColBraudatum, Qt::Horizontal, tr("Braudatum"));
    model->setHeaderData(ModelSud::ColAbfuelldatum, Qt::Horizontal, tr("Abfülldatum"));
    model->setHeaderData(ModelSud::ColErstellt, Qt::Horizontal, tr("Erstellt"));
    model->setHeaderData(ModelSud::ColGespeichert, Qt::Horizontal, tr("Gespeichert"));
    model->setHeaderData(ModelSud::ColWoche, Qt::Horizontal, tr("Woche"));
    model->setHeaderData(ModelSud::ColBewertungMittel, Qt::Horizontal, tr("Bewertung"));
    model->setHeaderData(ModelSud::ColMenge, Qt::Horizontal, tr("Menge") + "\n(L)");
    model->setHeaderData(ModelSud::ColSW, Qt::Horizontal, tr("SW") + "\n(°P)");
    model->setHeaderData(ModelSud::ColIBU, Qt::Horizontal, tr("Bittere") + "\n(IBU)");
    model->setHeaderData(ModelSud::Colerg_AbgefuellteBiermenge, Qt::Horizontal, tr("Menge") + "\n(L)");
    model->setHeaderData(ModelSud::Colerg_Sudhausausbeute, Qt::Horizontal, tr("SHA") + "\n(%)");
    model->setHeaderData(ModelSud::ColSWIst, Qt::Horizontal, tr("SW") + "\n(°P)");
    model->setHeaderData(ModelSud::ColSREIst, Qt::Horizontal, tr("Restextrakt") + "\n(°P)");
    model->setHeaderData(ModelSud::Colerg_S_Gesamt, Qt::Horizontal, tr("Schüttung") + "\n(kg)");
    model->setHeaderData(ModelSud::Colerg_Preis, Qt::Horizontal, tr("Kosten") + QStringLiteral("\n(%1/l)").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelSud::Colerg_Alkohol, Qt::Horizontal, tr("Alkohol") + "\n(%)");
    model->setHeaderData(ModelSud::ColsEVG, Qt::Horizontal, tr("sEVG") + "\n(%)");
    model->setHeaderData(ModelSud::ColtEVG, Qt::Horizontal, tr("tEVG") + "\n(%)");
    model->setHeaderData(ModelSud::Colerg_EffektiveAusbeute, Qt::Horizontal, tr("Eff. SHA") + "\n(%)");
    model->setHeaderData(ModelSud::ColVerdampfungsrateIst, Qt::Horizontal, tr("Verdampfungsrate") + "\n(l/h)");
    model->setHeaderData(ModelSud::ColAusbeuteIgnorieren, Qt::Horizontal, tr("Für Durchschnitt Ignorieren"));

    model = bh->modelMalz();
    model->setHeaderData(ModelMalz::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelMalz::ColMenge, Qt::Horizontal, tr("Menge") + "\n(kg)");
    model->setHeaderData(ModelMalz::ColFarbe, Qt::Horizontal, tr("Farbe") + "\n(EBC)");
    model->setHeaderData(ModelMalz::ColpH, Qt::Horizontal, tr("pH"));
    model->setHeaderData(ModelMalz::ColMaxProzent, Qt::Horizontal, tr("Max. Anteil") + "\n(%)");
    model->setHeaderData(ModelMalz::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelMalz::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelMalz::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelMalz::ColPreis, Qt::Horizontal, tr("Preis") + QStringLiteral("\n(%1/kg)").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelMalz::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelMalz::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelMalz::ColLink, Qt::Horizontal, tr("Link"));

    model = bh->modelHopfen();
    model->setHeaderData(ModelHopfen::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelHopfen::ColMenge, Qt::Horizontal, tr("Menge") + "\n(g)");
    model->setHeaderData(ModelHopfen::ColAlpha, Qt::Horizontal, tr("Alpha") + "\n(%)");
    model->setHeaderData(ModelHopfen::ColPellets, Qt::Horizontal, tr("Pellets"));
    model->setHeaderData(ModelHopfen::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelHopfen::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelHopfen::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelHopfen::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelHopfen::ColPreis, Qt::Horizontal, tr("Preis") + QStringLiteral("\n(%1/kg)").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelHopfen::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelHopfen::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelHopfen::ColLink, Qt::Horizontal, tr("Link"));

    model = bh->modelHefe();
    model->setHeaderData(ModelHefe::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelHefe::ColMenge, Qt::Horizontal, tr("Menge"));
    model->setHeaderData(ModelHefe::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelHefe::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelHefe::ColTypOGUG, Qt::Horizontal, tr("OG/UG"));
    model->setHeaderData(ModelHefe::ColTypTrFl, Qt::Horizontal, tr("Trocken/Flüssig"));
    model->setHeaderData(ModelHefe::ColWuerzemenge, Qt::Horizontal, tr("Würzemenge") + "\n(L)");
    model->setHeaderData(ModelHefe::ColSedimentation, Qt::Horizontal, tr("Sedimentation"));
    model->setHeaderData(ModelHefe::ColEVG, Qt::Horizontal, tr("Vergärungsgrad"));
    model->setHeaderData(ModelHefe::ColTemperatur, Qt::Horizontal, tr("Temperatur"));
    model->setHeaderData(ModelHefe::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelHefe::ColPreis, Qt::Horizontal, tr("Preis") + QStringLiteral("\n(%1)").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelHefe::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelHefe::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelHefe::ColLink, Qt::Horizontal, tr("Link"));

    model = bh->modelWeitereZutaten();
    model->setHeaderData(ModelWeitereZutaten::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelWeitereZutaten::ColMenge, Qt::Horizontal, tr("Menge"));
    model->setHeaderData(ModelWeitereZutaten::ColEinheit, Qt::Horizontal, tr("Einheit"));
    model->setHeaderData(ModelWeitereZutaten::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelWeitereZutaten::ColAusbeute, Qt::Horizontal, tr("Ausbeute") + "\n(%)");
    model->setHeaderData(ModelWeitereZutaten::ColUnvergaerbar, Qt::Horizontal, tr("Unvergärbar"));
    model->setHeaderData(ModelWeitereZutaten::ColFarbe, Qt::Horizontal, tr("Farbe") + "\n(EBC)");
    model->setHeaderData(ModelWeitereZutaten::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelWeitereZutaten::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelWeitereZutaten::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelWeitereZutaten::ColPreis, Qt::Horizontal, tr("Preis") + tr("\n(%1/[kg/L/Stk.])").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelWeitereZutaten::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelWeitereZutaten::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelWeitereZutaten::ColLink, Qt::Horizontal, tr("Link"));

    model = bh->modelWasser();
    model->setHeaderData(ModelWasser::ColName, Qt::Horizontal, tr("Wasserprofil"));
    model->setHeaderData(ModelWasser::ColRestalkalitaet, Qt::Horizontal, tr("Restalkalität") + "\n(°dH)");

    model = bh->modelSchnellgaerverlauf();
    model->setHeaderData(ModelSchnellgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelSchnellgaerverlauf::ColRestextrakt, Qt::Horizontal, tr("SRE") + "\n(°P)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColTemp, Qt::Horizontal, tr("Temperatur") + "\n(°C)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColAlc, Qt::Horizontal, tr("Alkohol") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelHauptgaerverlauf();
    model->setHeaderData(ModelHauptgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelHauptgaerverlauf::ColRestextrakt, Qt::Horizontal, tr("SRE") + "\n(°P)");
    model->setHeaderData(ModelHauptgaerverlauf::ColTemp, Qt::Horizontal, tr("Temperatur") + "\n(°C)");
    model->setHeaderData(ModelHauptgaerverlauf::ColAlc, Qt::Horizontal, tr("Alkohol") + "\n(%)");
    model->setHeaderData(ModelHauptgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG") + "\n(%)");
    model->setHeaderData(ModelHauptgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelNachgaerverlauf();
    model->setHeaderData(ModelNachgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelNachgaerverlauf::ColDruck, Qt::Horizontal, tr("Druck") + "\n(bar)");
    model->setHeaderData(ModelNachgaerverlauf::ColTemp, Qt::Horizontal, tr("Temperatur") + "\n(°C)");
    model->setHeaderData(ModelNachgaerverlauf::ColCO2, Qt::Horizontal, tr("CO2") + "\n(g/L)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelAusruestung();
    model->setHeaderData(ModelAusruestung::ColName, Qt::Horizontal, tr("Anlage"));
    model->setHeaderData(ModelAusruestung::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelAusruestung::ColVermoegen, Qt::Horizontal, tr("Vermögen") + "\n(L)");
    model->setHeaderData(ModelAusruestung::ColAnzahlSude, Qt::Horizontal, tr("Anzahl Sude"));
    model->setHeaderData(ModelAusruestung::ColAnzahlGebrauteSude, Qt::Horizontal, tr("Anzahl gebraute Sude"));

    model = bh->modelGeraete();
    model->setHeaderData(ModelGeraete::ColBezeichnung, Qt::Horizontal, tr("Bezeichnung"));

    model = bh->modelTags();
    model->setHeaderData(ModelTags::ColKey, Qt::Horizontal, tr("Tag"));
    model->setHeaderData(ModelTags::ColValue, Qt::Horizontal, tr("Wert"));
    model->setHeaderData(ModelTags::ColGlobal, Qt::Horizontal, tr("Global"));
}

void MainWindow::checkLoadedSud()
{
    if (!bh->sud()->isLoaded())
        return;
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (!gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        if (bh->sud()->getSpeisemenge() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Speisemenge zurücksetzen?"),
                                          tr("Das Modul \"Speiseberechnung\" ist deaktiviert, aber es wurde für diesen Sud eine Speisemenge angegeben.") + " " +
                                          tr("Soll diese zurückgesetzt werden (empfohlen)?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    bh->sud()->setSpeisemenge(0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Speisemenge"),
                                     tr("Das Modul \"Speiseberechnung\" ist deaktiviert, aber es wurde für diesen Sud eine Speisemenge angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
    }
    if (!gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe))
    {
        if (bh->sud()->getSchnellgaerprobeAktiv())
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Schnellgärprobe deaktivieren?"),
                                          tr("Das Modul \"Schnellgärprobe\" ist deaktiviert, aber es wurde für diesen Sud die Schnellgärprobe aktiviert.") + " " +
                                          tr("Soll diese deaktiviert werden (empfohlen)?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    bh->sud()->setSchnellgaerprobeAktiv(false);
            }
            else
            {
                QMessageBox::warning(this, tr("Schnellgärprobe"),
                                     tr("Das Modul \"Schnellgärprobe\" ist deaktiviert, aber es wurde für diesen Sud die Schnellgärprobe aktiviert.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
    }
    if (!gSettings->isModuleEnabled(Settings::ModuleAusruestung))
    {
        if (bh->sud()->getAnlageData(ModelAusruestung::ColKorrekturWasser).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Nachgussmenge) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    bh->sud()->setAnlageData(ModelAusruestung::ColKorrekturWasser, 0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Korrekturwert"),
                                     tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Nachgussmenge) angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
        if (bh->sud()->getAnlageData(ModelAusruestung::ColKorrekturFarbe).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Farbwert) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    bh->sud()->setAnlageData(ModelAusruestung::ColKorrekturFarbe, 0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Korrekturwert"),
                                     tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Farbwert) angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
        if (bh->sud()->getAnlageData(ModelAusruestung::ColKorrekturMenge).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Sollmenge) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    bh->sud()->setAnlageData(ModelAusruestung::ColKorrekturMenge, 0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Korrekturwert"),
                                     tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Sollmenge) angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
        if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        {
            if (bh->sud()->getAnlageData(ModelAusruestung::ColKosten).toDouble() != 0.0)
            {
                if (status < Brauhelfer::SudStatus::Abgefuellt)
                {
                    int ret = QMessageBox::question(this, tr("Betriebskosten zurücksetzen?"),
                                              tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage Betriebskosten angegeben.") + " " +
                                              tr("Soll diese zurückgesetzt werden?"),
                                              QMessageBox::Yes | QMessageBox::No,
                                              QMessageBox::Yes);
                    if (ret == QMessageBox::Yes)
                        bh->sud()->setAnlageData(ModelAusruestung::ColKosten, 0.0);
                }
                else
                {
                    QMessageBox::warning(this, tr("Betriebskosten"),
                                         tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage Betriebskosten angegeben.") + " " +
                                         tr("Der Preis wird nicht korrekt berechnet."));
                }
            }
        }
    }
}

void MainWindow::on_actionCheckUpdate_triggered(bool checked)
{
  #if QT_NETWORK_LIB
    gSettings->setValueInGroup("General", "CheckUpdate", checked);
    if (checked)
        checkForUpdate(true);
  #else
    Q_UNUSED(checked)
  #endif
}

void MainWindow::on_actionTooltips_triggered(bool checked)
{
    gSettings->setValueInGroup("General", "TooltipsEnabled", checked);
}

void MainWindow::on_actionAnimationen_triggered(bool checked)
{
    gSettings->setAnimationsEnabled(checked);
}

void MainWindow::on_actionDeutsch_triggered()
{
    gSettings->setLanguage(QStringLiteral("de"));
    restart(1001);
}

void MainWindow::on_actionEnglisch_triggered()
{
    gSettings->setLanguage(QStringLiteral("en"));
    restart(1001);
}

void MainWindow::on_actionSchwedisch_triggered()
{
    gSettings->setLanguage(QStringLiteral("sv"));
    restart(1001);
}

void MainWindow::on_actionNiederlaendisch_triggered()
{
    gSettings->setLanguage(QStringLiteral("nl"));
    restart(1001);
}

void MainWindow::on_actionZahlenformat_triggered(bool checked)
{
    gSettings->setValueInGroup("General", "UseLanguageLocale", checked);
    restart(1001);
}

void MainWindow::on_actionModule_triggered()
{
    DlgAbstract::showDialog<DlgModule>(this);
}

void MainWindow::on_actionHilfe_triggered()
{
    DlgHilfe* dlg = DlgAbstract::showDialog<DlgHilfe>(this);
    dlg->setHomeUrl(QStringLiteral(URL_HILFE));
}

void MainWindow::on_actionUeber_triggered()
{
    DlgAbout dlg(this);
    dlg.exec();
}

void MainWindow::on_actionLog_triggered()
{
    DlgAbstract::showDialog<DlgConsole>(this, ui->actionLog);
}

void MainWindow::on_actionDatenbank_triggered()
{
    DlgAbstract::showDialog<DlgDatenbank>(this, ui->actionDatenbank);
}

DlgRohstoffe* MainWindow::showDialogRohstoffe()
{
    return DlgAbstract::showDialog<DlgRohstoffe>(this, ui->actionRohstoffe);
}

DlgBrauUebersicht* MainWindow::showDialogBrauUebersicht()
{
    return DlgAbstract::showDialog<DlgBrauUebersicht>(this, ui->actionBrauUebersicht);
}

DlgAusruestung* MainWindow::showDialogAusruestung()
{
    return DlgAbstract::showDialog<DlgAusruestung>(this, ui->actionAusruestung);
}
