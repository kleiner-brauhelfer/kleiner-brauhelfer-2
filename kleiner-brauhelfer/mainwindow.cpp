#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTranslator>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QTabBar>
#include <QCloseEvent>
#include <QMessageBox>
#include "commands/undostack.h"
#include "dialogs/dlgcheckupdate.h"
#include "widgets/widgetdecorator.h"
#include "brauhelfer.h"
#include "settings.h"
#include "definitionen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

UndoStack* gUndoStack = nullptr;

QStringList MainWindow::HopfenTypname;
QStringList MainWindow::HefeTypname;
QStringList MainWindow::HefeTypFlTrName;
QStringList MainWindow::ZusatzTypname;
QStringList MainWindow::Einheiten;

static void installTranslator(QTranslator &translator, const QString &filename)
{
    QApplication* app = qApp;
    QLocale locale(gSettings->language());
    app->removeTranslator(&translator);
    if (translator.load(locale, filename, QStringLiteral("_"), app->applicationDirPath() + "/translations"))
        app->installTranslator(&translator);
    else if (translator.load(locale, filename, QStringLiteral("_"), QStringLiteral(":/translations")))
        app->installTranslator(&translator);
    else if (translator.load(locale, filename, QStringLiteral("_"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app->installTranslator(&translator);
}

void MainWindow::installTranslators()
{
    static QTranslator translator;
    static QTranslator translatorQt;
    installTranslator(translator, QStringLiteral("kbh"));
    installTranslator(translatorQt, QStringLiteral("qtbase"));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // undo stack
    gUndoStack = new UndoStack(this);
    gUndoStack->setEnabled(gSettings->valueInGroup("General", "UndoEnabled", true).toBool());

    // setup ui
    setupLabels();
    ui->setupUi(this);
    setupActions();

    // tooltip event filter
    qApp->installEventFilter(this);

    // set theme
    QIcon::setThemeSearchPaths({":/images/icons"});
    themeChanged(gSettings->theme() == Settings::Light ? "light" : "dark");

    connect(gSettings, &Settings::themeChanged, this, &MainWindow::themeChanged);
    connect(gSettings, &Settings::languageChanged, this, &MainWindow::languageChanged);
    connect(gSettings, &Settings::databasePathChanged, this, &MainWindow::databasePathChanged);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

    connect(bh, &Brauhelfer::modified, this, &MainWindow::databaseModified);

    restoreView();
    loadViewSettings();

    databaseModified();
    WidgetDecorator::clearValueChanged();

    // check for update
    if (gSettings->valueInGroup("General", "CheckUpdate", true).toBool())
        checkUpdate(false);
}

MainWindow::~MainWindow()
{
    saveSettings();
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);
    delete ui;
}

void MainWindow::setupActions()
{
    ui->actionSave->setShortcut(QKeySequence::Save);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveDatabase);
    connect(ui->actionDiscard, &QAction::triggered, this, &MainWindow::discardDatabase);

    if (gUndoStack->isEnabled())
    {
        //ui->menuBearbeiten->addAction(gUndoStack->createUndoAction(this, tr("Rückgängig")));
        //ui->menuBearbeiten->addAction(gUndoStack->createRedoAction(this, tr("Wiederherstellen")));
    }
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());

    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tabWidget->tabBar()->addAction(ui->actionShowTabBarLabels);

    connect(ui->tabEinstellungen, &TabEinstellungen::restoreView, this, &MainWindow::restoreView);
    connect(ui->tabEinstellungen, &TabEinstellungen::checkUpdate, this, &MainWindow::checkUpdate);
}

void MainWindow::setupLabels()
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

void MainWindow::loadViewSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    restoreGeometry(gSettings->value("geometry").toByteArray());
    ui->actionShowTabBarLabels->setChecked(gSettings->value("showLabels", true).toBool());
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());
    gSettings->endGroup();
}

void MainWindow::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("geometry", saveGeometry());
    gSettings->setValue("showLabels", ui->actionShowTabBarLabels->isChecked());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->endGroup();
}

void MainWindow::restoreView()
{
    ui->actionShowTabBarLabels->setChecked(true);

    ui->splitterHelp->setSizes({900, 100});
    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
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
    ui->toolBarSave->setEnabled(modified);
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

void MainWindow::themeChanged(const QString &theme)
{
    QIcon::setThemeName(theme);
    QFile file(QStringLiteral(":/data/Styles/style_%1.qss").arg(theme));
    file.open(QFile::ReadOnly);
    setStyleSheet(file.readAll());
}

void MainWindow::languageChanged(const QString &language)
{
    Q_UNUSED(language)
    installTranslators();
    setupLabels();
    ui->retranslateUi(this);
}

void MainWindow::databasePathChanged(const QString& path)
{
    Q_UNUSED(path)
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
    qApp->exit(1000);
}

void MainWindow::checkUpdate(bool force)
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
    connect(dlg, &DlgCheckUpdate::checkUpdatefinished, this, &MainWindow::checkUpdateFinished);
    dlg->checkForUpdate();
#else
    Q_UNUSED(force)
#endif
}

void MainWindow::checkUpdateFinished()
{
#if QT_NETWORK_LIB
    DlgCheckUpdate *dlg = qobject_cast<DlgCheckUpdate*>(sender());
    if (!dlg)
        return;
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
#endif
}

void MainWindow::on_tabWidget_currentChanged(int tab)
{
    ui->toolBarUndo->setVisible(gUndoStack->isEnabled() && (tab == Tab::Sude || tab == Tab::Lager || tab == Tab::Anlagen || tab == Tab::Bewertungen));
    ui->toolBarSudauswahl->setVisible(tab == Tab::Sudauswahl);
    ui->toolBarLager->setVisible(tab == Tab::Lager);
    ui->toolBarAnlagen->setVisible(tab == Tab::Anlagen);
    ui->toolBarBewertungen->setVisible(tab == Tab::Bewertungen);
}

void MainWindow::on_actionShowTabBarLabels_toggled(bool showLabels)
{
    if (showLabels)
    {
        ui->tabWidget->setTabText(Tab::Sudauswahl, ui->tabSudauswahl->windowTitle());
        ui->tabWidget->setTabText(Tab::Sude, ui->tabSude->windowTitle());
        ui->tabWidget->setTabText(Tab::Lager, ui->tabLager->windowTitle());
        ui->tabWidget->setTabText(Tab::Anlagen, ui->tabAnlagen->windowTitle());
        ui->tabWidget->setTabText(Tab::Brauubersicht, ui->tabBrauuebersicht->windowTitle());
        ui->tabWidget->setTabText(Tab::Bewertungen, ui->tabBewertungen->windowTitle());
        ui->tabWidget->setTabText(Tab::Einstellungen, ui->tabEinstellungen->windowTitle());
        ui->tabWidget->setTabText(Tab::Ueber, ui->tabUeber->windowTitle());
        ui->tabWidget->setIconSize(QSize(16,16));
    }
    else
    {
        ui->tabWidget->setTabText(Tab::Sudauswahl, QString());
        ui->tabWidget->setTabText(Tab::Sude, QString());
        ui->tabWidget->setTabText(Tab::Lager, QString());
        ui->tabWidget->setTabText(Tab::Anlagen, QString());
        ui->tabWidget->setTabText(Tab::Brauubersicht, QString());
        ui->tabWidget->setTabText(Tab::Bewertungen, QString());
        ui->tabWidget->setTabText(Tab::Einstellungen, QString());
        ui->tabWidget->setTabText(Tab::Ueber, QString());
        ui->tabWidget->setIconSize(QSize(20,20));
    }
}

void MainWindow::focusChanged(QWidget *old, QWidget *now)
{
    if (old && now && strcmp(old->metaObject()->className(),"QtPrivate::QCalendarView") != 0)
        WidgetDecorator::clearValueChanged();
    if (now && now != ui->tbHelp && !qobject_cast<QSplitter*>(now))
        ui->tbHelp->setHtml(now->toolTip());
}
