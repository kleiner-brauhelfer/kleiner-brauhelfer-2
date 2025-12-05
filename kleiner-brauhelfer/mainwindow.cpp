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
#include "dialogs/dlgabout.h"
#include "dialogs/dlgausruestung.h"
#include "dialogs/dlgbewertungen.h"
#include "dialogs/dlgbrauuebersicht.h"
#include "dialogs/dlgcheckupdate.h"
#include "dialogs/dlgdatabasecleaner.h"
#include "dialogs/dlgdatenbank.h"
#include "dialogs/dlgeinstellungen.h"
#include "dialogs/dlgetikett.h"
#include "dialogs/dlghilfe.h"
#include "dialogs/dlgimportexport.h"
#include "dialogs/dlgmaischplanmalz.h"
#include "dialogs/dlgphmalz.h"
#include "dialogs/dlgprintout.h"
#include "dialogs/dlgrestextrakt.h"
#include "dialogs/dlgrichtexteditor.h"
#include "dialogs/dlgrohstoffauswahl.h"
#include "dialogs/dlgrohstoffe.h"
#include "dialogs/dlgrohstoffeabziehen.h"
#include "dialogs/dlgrohstoffvorlage.h"
#include "dialogs/dlgsudteilen.h"
#include "dialogs/dlgtableview.h"
#include "dialogs/dlguebernahmerezept.h"
#include "dialogs/dlgverdampfung.h"
#include "dialogs/dlgvolumen.h"
#include "dialogs/dlgwasseraufbereitung.h"
#include "dialogs/dlgwasserprofile.h"
#include "widgets/widgetdecorator.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

UndoStack* gUndoStack = nullptr;

QStringList MainWindow::HopfenTypname;
QStringList MainWindow::HefeTypname;
QStringList MainWindow::HefeTypFlTrName;
QStringList MainWindow::ZusatzTypname;
QStringList MainWindow::Einheiten;
QList<QPair<QString, int> > MainWindow::AnlageTypname;

MainWindow* MainWindow::getInstance()
{
    for (QWidget* wdg: qApp->topLevelWidgets())
        if (MainWindow* mainWin = qobject_cast<MainWindow*>(wdg))
            return mainWin;
    return nullptr;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mSud(new SudObject(bh))
{
    Qt::ColorScheme theme = gSettings->theme();
    QIcon::setThemeSearchPaths({":/images/icons"});
    initTheme(theme);

    initLabels();
    ui->setupUi(this);
    ui->tabRezept->setup(mSud);
    ui->tabBraudaten->setup(mSud);
    ui->tabAbfuelldaten->setup(mSud);
    ui->tabGaerverlauf->setup(mSud);
    qApp->installEventFilter(this);

    connect(gSettings, &Settings::themeChanged, this, &MainWindow::themeChanged);

    gUndoStack = new UndoStack(this);
    gUndoStack->setEnabled(gSettings->valueInGroup("General", "UndoEnabled", true).toBool());
    if (gUndoStack->isEnabled())
    {
        ui->toolBarSave->addAction(gUndoStack->createRedoAction(this, tr("Redo")));
        ui->toolBarSave->addAction(gUndoStack->createUndoAction(this, tr("Undo")));
        ui->toolBarSave->addSeparator();
    }

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, gSettings->palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, gSettings->palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("MainWindow");
    restoreGeometry(gSettings->value("geometry").toByteArray());
    mDefaultState = saveState();
    restoreState(gSettings->value("state").toByteArray());
    ui->splitterHelp->setSizes({900, 100});
    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());
    gSettings->endGroup();

    if (ui->toolBarSave->isHidden())
        ui->toolBarSave->setVisible(true);
    if (ui->toolBar->isHidden())
        ui->toolBar->setVisible(true);
    if (ui->toolBarLeft->isHidden())
        ui->toolBarLeft->setVisible(true);

    gSettings->beginGroup("General");
    BierCalc::faktorPlatoToBrix = gSettings->value("RefraktometerKorrekturfaktor", 1.03).toDouble();
    gSettings->endGroup();

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

    connect(gSettings, &Settings::languageChanged, this, [this](){restart(1001);});
    connect(gSettings, &Settings::databasePathChanged, this, [this](){restart(1000);});
    connect(gSettings, &Settings::modulesChanged, this, &MainWindow::modulesChanged);

    connect(ui->tabSudAuswahl, &TabSudAuswahl::clicked, this, &MainWindow::loadSud);

    connect(bh, &Brauhelfer::modified, this, &MainWindow::databaseModified);
    connect(bh, &Brauhelfer::discarded, this, &MainWindow::updateValues);
    connect(mSud, &SudObject::loadedChanged, this, &MainWindow::sudLoaded);
    connect(mSud, &SudObject::dataChanged, this, &MainWindow::sudDataChanged);

    initActions();

    ui->tabSudAuswahl->setupToolbar(ui->toolBarSudauswahl);
    connect(ui->tabMain, &QTabWidget::currentChanged, this, [this](){ui->toolBarSudauswahl->setVisible(ui->tabMain->currentWidget() == ui->tabSudAuswahl);});

    if (gSettings->valueInGroup("General", "CheckUpdate", true).toBool())
        checkForUpdate(false);

    if (gSettings->modulesFirstTime)
        ui->actionSettings->trigger();

    modulesChanged(Settings::ModuleAlle);
    sudLoaded();
}

MainWindow::~MainWindow()
{
    saveSettings();
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);
    delete ui;
    delete mSud;
}

void MainWindow::initActions()
{
    connect(ui->actionSpeichern, &QAction::triggered, this, &MainWindow::saveDatabase);
    connect(ui->actionVerwerfen, &QAction::triggered, this, &MainWindow::discardDatabase);

    connect(ui->actionRohstoffe, &QAction::triggered, this, [this](){DlgAbstract::showDialog<DlgRohstoffe>(this, ui->actionRohstoffe);});
    connect(ui->actionAusruestung, &QAction::triggered, this, [this](){
        DlgAusruestung* dlg = DlgAbstract::showDialog<DlgAusruestung>(this, ui->actionAusruestung);
        dlg->select(mSud->getAnlage());
    });
    connect(ui->actionBrauUebersicht, &QAction::triggered, this, [this](){
        DlgBrauUebersicht* dlg = DlgAbstract::showDialog<DlgBrauUebersicht>(this, ui->actionBrauUebersicht);
        dlg->select(mSud->id());
    });
    connect(ui->actionPrintout, &QAction::triggered, this, [this](){
        DlgPrintout* dlg = DlgAbstract::showDialog<DlgPrintout>(this, ui->actionPrintout);
        dlg->select(mSud->id());
    });
    connect(ui->actionEtikett, &QAction::triggered, this, [this](){
        DlgEtikett* dlg = DlgAbstract::showDialog<DlgEtikett>(this, ui->actionEtikett);
        dlg->select(mSud->id());
    });
    connect(ui->actionBewertungen, &QAction::triggered, this, [this](){
        DlgBewertungen* dlg = DlgAbstract::showDialog<DlgBewertungen>(this, ui->actionBewertungen);
        dlg->select(mSud->id());
    });

    connect(ui->actionEingabefelderEntsperren, &QAction::triggered, this, &MainWindow::eingabefelderEntsperren);
    connect(ui->actionSettings, &QAction::triggered, this, [this](){
        DlgEinstellungen* dlg = DlgAbstract::showDialog<DlgEinstellungen>(this, ui->actionSettings);
        connect(dlg, &DlgEinstellungen::restoreViewTriggered, this, &MainWindow::restoreView, Qt::UniqueConnection);
        connect(dlg, &DlgEinstellungen::checkUpdateTriggered, this, &MainWindow::checkForUpdate, Qt::UniqueConnection);
        return dlg;
    });
    connect(ui->actionDatenbank, &QAction::triggered, this, [this](){DlgAbstract::showDialog<DlgDatenbank>(this, ui->actionDatenbank);});
    connect(ui->actionHilfe, &QAction::triggered, this, [this](){
        DlgHilfe* dlg = DlgAbstract::showDialog<DlgHilfe>(this, ui->actionHilfe);
        dlg->setHomeUrl(QStringLiteral(URL_HILFE));
    });
    connect(ui->actionUeber, &QAction::triggered, this, [this](){
        DlgAbout dlg(this);
        dlg.exec();
        ui->actionUeber->setChecked(false);
    });
}

QAction* MainWindow::getAction(const QString& name) const
{
    if (name == "actionAusruestung")
        return ui->actionAusruestung;
    return nullptr;
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
        if (gSettings->valueInGroup("General", "BeendenAbfrage", true).toBool())
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
    if (event->type() == QEvent::ToolTip && !gSettings->valueInGroup("General", "TooltipsEnabled", true).toBool())
        return true;
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::focusChanged(QWidget *old, QWidget *now)
{
    if (old && now && strcmp(old->metaObject()->className(),"QtPrivate::QCalendarView") != 0)
        WidgetDecorator::clearValueChanged();
    if (now && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}

void MainWindow::initTheme(Qt::ColorScheme theme)
{
    QString themeName = theme == Qt::ColorScheme::Dark ? "dark" : "light";
    QIcon::setThemeName(themeName);
}

void MainWindow::themeChanged(Qt::ColorScheme theme)
{
    initTheme(theme);
    restart();
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
            QGuiApplication::restoreOverrideCursor();
        }
        else
        {
            QGuiApplication::restoreOverrideCursor();
            QMessageBox::critical(this, tr("Fehler beim Speichern"), bh->lastError());
        }
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
    int ret = QMessageBox::question(this, tr("Änderungen verwerfen?"), tr("Sollen alle Änderungen verworfen werden?"));
    if (ret != QMessageBox::Yes)
        return;
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
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->endGroup();
    ui->tabSudAuswahl->saveSettings();
    ui->tabRezept->saveSettings();
    ui->tabBraudaten->saveSettings();
    ui->tabAbfuelldaten->saveSettings();
    ui->tabGaerverlauf->saveSettings();
}

void MainWindow::restoreView()
{
    restoreState(mDefaultState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
    ui->tabSudAuswahl->restoreView();
    ui->tabRezept->restoreView();
    ui->tabBraudaten->restoreView();
    ui->tabAbfuelldaten->restoreView();
    ui->tabGaerverlauf->restoreView();
    DlgAbout::restoreView();
    DlgAusruestung::restoreView();
    DlgBewertungen::restoreView();
    DlgBrauUebersicht::restoreView();
    DlgCheckUpdate::restoreView();
    DlgDatabaseCleaner::restoreView();
    DlgDatenbank::restoreView();
    DlgEinstellungen::restoreView();
    DlgEtikett::restoreView();
    DlgHilfe::restoreView();
    DlgImportExport::restoreView();
    DlgMaischplanMalz::restoreView();
    DlgPhMalz::restoreView();
    DlgPrintout::restoreView();
    DlgRestextrakt::restoreView();
    DlgRichTextEditor::restoreView();
    DlgRohstoffAuswahl::restoreView();
    DlgRohstoffe::restoreView();
    DlgRohstoffeAbziehen::restoreView();
    DlgRohstoffVorlage::restoreView();
    DlgSudTeilen::restoreView();
    DlgTableView::restoreView();
    DlgUebernahmeRezept::restoreView();
    DlgVerdampfung::restoreView();
    DlgVolumen::restoreView();
    DlgWasseraufbereitung::restoreView();
    DlgWasserprofile::restoreView();
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
    if (mSud->isLoaded())
        checkSud(mSud);
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
                ui->tabMain->insertTab(nextIndex, ui->tabGaerverlauf, QIcon::fromTheme("tabgaerverlauf"), tr("Gärverlauf"));
        }
        else
            ui->tabMain->removeTab(index);
    }
    if (gSettings->isModuleEnabled(Settings::ModuleGaerverlauf))
        nextIndex++;
    if (modules.testFlag(Settings::ModuleBrauuebersicht))
        ui->actionBrauUebersicht->setVisible(gSettings->isModuleEnabled(Settings::ModuleBrauuebersicht));
    if (modules.testFlag(Settings::ModuleAusruestung))
        ui->actionAusruestung->setVisible(gSettings->isModuleEnabled(Settings::ModuleAusruestung));
    if (modules.testFlag(Settings::ModuleAusdruck))
        ui->actionPrintout->setVisible(gSettings->isModuleEnabled(Settings::ModuleAusdruck));
    if (modules.testFlag(Settings::ModuleEtikett))
        ui->actionEtikett->setVisible(gSettings->isModuleEnabled(Settings::ModuleEtikett));
    if (modules.testFlag(Settings::ModuleBewertung))
        ui->actionBewertungen->setVisible(gSettings->isModuleEnabled(Settings::ModuleBewertung));
}

void MainWindow::databaseModified()
{
    bool modified = bh->isDirty();
    QString title;
    if (modified)
        title = QStringLiteral("* ");
    if (mSud->isLoaded())
        title += mSud->getSudname() + " - ";
    title += QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion();
    setWindowTitle(title);
    ui->actionSpeichern->setEnabled(modified);
    ui->actionVerwerfen->setEnabled(modified);
}

void MainWindow::updateValues()
{
    bool loaded = mSud->isLoaded();
    databaseModified();
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabRezept), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabBraudaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabAbfuelldaten), loaded);
    ui->tabMain->setTabEnabled(ui->tabMain->indexOf(ui->tabGaerverlauf), loaded);
    ui->actionEingabefelderEntsperren->setChecked(false);
    ui->actionEingabefelderEntsperren->setEnabled(loaded && static_cast<Brauhelfer::SudStatus>(mSud->getStatus()) != Brauhelfer::SudStatus::Rezept);
    if (!loaded)
        ui->tabMain->setCurrentWidget(ui->tabSudAuswahl);
}

void MainWindow::sudLoaded()
{
    updateValues();
    if (mSud->isLoaded())
    {
        checkSud(mSud);
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
    if (mSud->id() == sudId)
    {
        ui->tabMain->setCurrentWidget(ui->tabRezept);
    }
    else
    {
        try
        {
            mSud->load(sudId);
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

void MainWindow::eingabefelderEntsperren()
{
    if (ui->actionEingabefelderEntsperren->isChecked())
    {
        int ret = QMessageBox::question(this, tr("Eingabefelder entsperren?"),
                                  tr("Vorsicht! Die Entsperrung aller Eingabefeldern kann zu inkonsistenten Daten führen und sollte mit Vorsicht verwendet werden."),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes);
        if (ret == QMessageBox::Yes)
        {
            gSettings->ForceEnabled = true;
        }
    }
    else
    {
        gSettings->ForceEnabled = false;
    }
    ui->tabRezept->checkEnabled();
    ui->tabBraudaten->checkEnabled();
    ui->tabAbfuelldaten->checkEnabled();
    ui->tabGaerverlauf->checkEnabled();
    ui->actionEingabefelderEntsperren->setChecked(gSettings->ForceEnabled);
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
    AnlageTypname = QList<QPair<QString, int> >({
        {tr("Standard"), static_cast<int>(Brauhelfer::AnlageTyp::Standard)},
        {tr("Grainfather G30"), static_cast<int>(Brauhelfer::AnlageTyp::GrainfatherG30)},
        {tr("Grainfather G70"), static_cast<int>(Brauhelfer::AnlageTyp::GrainfatherG70)},
        {tr("Braumeister 10L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister10)},
        {tr("Braumeister 20L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister20)},
        {tr("Braumeister 50L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister50)},
        {tr("Braumeister 200L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister200)},
        {tr("Braumeister 500L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister500)},
        {tr("Braumeister 1000L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister1000)},
        {tr("Brauheld Pro 30L"), static_cast<int>(Brauhelfer::AnlageTyp::BrauheldPro30)}
    });

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
    model->setHeaderData(ModelSud::Colerg_Alkohol, Qt::Horizontal, tr("Alkohol") + "\n(%vol)");
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
    model->setHeaderData(ModelSchnellgaerverlauf::ColAlc, Qt::Horizontal, tr("Alkohol") + "\n(%vol)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelHauptgaerverlauf();
    model->setHeaderData(ModelHauptgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelHauptgaerverlauf::ColRestextrakt, Qt::Horizontal, tr("SRE") + "\n(°P)");
    model->setHeaderData(ModelHauptgaerverlauf::ColTemp, Qt::Horizontal, tr("Temperatur") + "\n(°C)");
    model->setHeaderData(ModelHauptgaerverlauf::ColAlc, Qt::Horizontal, tr("Alkohol") + "\n(%vol)");
    model->setHeaderData(ModelHauptgaerverlauf::ColsEVG, Qt::Horizontal, tr("sEVG") + "\n(%)");
    model->setHeaderData(ModelHauptgaerverlauf::ColtEVG, Qt::Horizontal, tr("tEVG") + "\n(%)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelNachgaerverlauf();
    model->setHeaderData(ModelNachgaerverlauf::ColZeitstempel, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelNachgaerverlauf::ColDruck, Qt::Horizontal, tr("Druck") + "\n(bar)");
    model->setHeaderData(ModelNachgaerverlauf::ColTemp, Qt::Horizontal, tr("Temperatur") + "\n(°C)");
    model->setHeaderData(ModelNachgaerverlauf::ColCO2, Qt::Horizontal, tr("CO2") + "\n(g/L)");
    model->setHeaderData(ModelSchnellgaerverlauf::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));

    model = bh->modelBewertungen();
    model->setHeaderData(ModelBewertungen::ColSudName, Qt::Horizontal, tr("Sud"));
    model->setHeaderData(ModelBewertungen::ColDatum, Qt::Horizontal, tr("Datum"));
    model->setHeaderData(ModelBewertungen::ColWoche, Qt::Horizontal, tr("Woche"));
    model->setHeaderData(ModelBewertungen::ColSterne, Qt::Horizontal, tr("Bewertung"));

    model = bh->modelAusruestung();
    model->setHeaderData(ModelAusruestung::ColName, Qt::Horizontal, tr("Bezeichnung"));
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

void MainWindow::checkSud(SudObject *sud)
{
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(sud->getStatus());
    if (!gSettings->isModuleEnabled(Settings::ModuleSpeise))
    {
        if (sud->getSpeisemenge() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Speisemenge zurücksetzen?"),
                                          tr("Das Modul \"Speiseberechnung\" ist deaktiviert, aber es wurde für diesen Sud eine Speisemenge angegeben.") + " " +
                                          tr("Soll diese zurückgesetzt werden (empfohlen)?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    sud->setSpeisemenge(0.0);
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
        if (sud->getSchnellgaerprobeAktiv())
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Schnellgärprobe deaktivieren?"),
                                          tr("Das Modul \"Schnellgärprobe\" ist deaktiviert, aber es wurde für diesen Sud die Schnellgärprobe aktiviert.") + " " +
                                          tr("Soll diese deaktiviert werden (empfohlen)?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    sud->setSchnellgaerprobeAktiv(false);
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
        if (sud->getAnlageData(ModelAusruestung::ColKorrekturWasser).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Nachgussmenge) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    sud->setAnlageData(ModelAusruestung::ColKorrekturWasser, 0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Korrekturwert"),
                                     tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Nachgussmenge) angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
        if (sud->getAnlageData(ModelAusruestung::ColKorrekturFarbe).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Farbwert) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    sud->setAnlageData(ModelAusruestung::ColKorrekturFarbe, 0.0);
            }
            else
            {
                QMessageBox::warning(this, tr("Korrekturwert"),
                                     tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Farbwert) angegeben.") + " " +
                                     tr("Der Sud wird nicht korrekt dargestellt."));
            }
        }
        if (sud->getAnlageData(ModelAusruestung::ColKorrekturMenge).toDouble() != 0.0)
        {
            if (status < Brauhelfer::SudStatus::Abgefuellt)
            {
                int ret = QMessageBox::question(this, tr("Korrekturwert zurücksetzen?"),
                                          tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage ein Korrekturwert (Sollmenge) angegeben.") + " " +
                                          tr("Soll dieser zurückgesetzt werden?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                    sud->setAnlageData(ModelAusruestung::ColKorrekturMenge, 0.0);
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
            if (sud->getAnlageData(ModelAusruestung::ColKosten).toDouble() != 0.0)
            {
                if (status < Brauhelfer::SudStatus::Abgefuellt)
                {
                    int ret = QMessageBox::question(this, tr("Betriebskosten zurücksetzen?"),
                                              tr("Das Modul \"Ausrüstung\" ist deaktiviert, aber es wurde für diese Anlage Betriebskosten angegeben.") + " " +
                                              tr("Soll diese zurückgesetzt werden?"),
                                              QMessageBox::Yes | QMessageBox::No,
                                              QMessageBox::Yes);
                    if (ret == QMessageBox::Yes)
                        sud->setAnlageData(ModelAusruestung::ColKosten, 0.0);
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
