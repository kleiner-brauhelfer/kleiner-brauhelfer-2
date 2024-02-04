#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTranslator>
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
    // setup ui
    ui->setupUi(this);

    // tooltip event filter
    qApp->installEventFilter(this);

    // set theme
    themeChanged(gSettings->theme() == Settings::Light ? "light" : "dark");

    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tabWidget->tabBar()->addAction(ui->actionShowTabBarLabels);

    connect(gSettings, &Settings::themeChanged, this, &MainWindow::themeChanged);
    connect(gSettings, &Settings::languageChanged, this, &MainWindow::languageChanged);
    connect(gSettings, &Settings::databasePathChanged, this, &MainWindow::databasePathChanged);

    connect(ui->tabEinstellungen, &TabEinstellungen::restoreView, this, &MainWindow::restoreView);
    connect(ui->tabEinstellungen, &TabEinstellungen::checkUpdate, this, &MainWindow::checkUpdate);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

    restoreView();
    loadViewSettings();

    WidgetDecorator::clearValueChanged();

    // undo stack
    gUndoStack = new UndoStack(this);
    gUndoStack->setEnabled(gSettings->valueInGroup("General", "UndoEnabled", true).toBool());
    if (gUndoStack->isEnabled())
    {
        //ui->menuBearbeiten->addAction(gUndoStack->createUndoAction(this, tr("Rückgängig")));
        //ui->menuBearbeiten->addAction(gUndoStack->createRedoAction(this, tr("Wiederherstellen")));
    }
    else
    {
        //ui->menuBearbeiten->setEnabled(false);
    }

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
    QFile file(QStringLiteral(":/data/Styles/style_%1.qss").arg(theme));
    file.open(QFile::ReadOnly);
    setStyleSheet(file.readAll());
}

void MainWindow::languageChanged(const QString &language)
{
    Q_UNUSED(language)
    installTranslators();
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
    ui->toolBarUndo->setVisible(tab == Tab::Sude || tab == Tab::Lager || tab == Tab::Anlagen || tab == Tab::Bewertungen);
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
