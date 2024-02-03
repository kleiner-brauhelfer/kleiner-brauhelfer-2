#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTranslator>
#include <QFile>
#include <QLibraryInfo>
#include <QTabBar>
#include "widgets/widgetdecorator.h"
#include "settings.h"

extern Settings* gSettings;

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

    // set theme
    themeChanged(gSettings->theme() == Settings::Light ? "light" : "dark");

    ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tabWidget->tabBar()->addAction(ui->actionShowTabBarLabels);

    connect(gSettings, &Settings::themeChanged, this, &MainWindow::themeChanged);
    connect(gSettings, &Settings::languageChanged, this, &MainWindow::languageChanged);
    connect(gSettings, &Settings::databasePathChanged, this, &MainWindow::databasePathChanged);

    connect(ui->tabEinstellungen, &TabEinstellungen::restoreView, this, &MainWindow::restoreView);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::focusChanged);

    restoreView();
    loadViewSettings();

    WidgetDecorator::clearValueChanged();
}

MainWindow::~MainWindow()
{
    saveViewSettings();
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

void MainWindow::saveViewSettings()
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

void MainWindow::themeChanged(const QString &theme)
{
    QFile file(QStringLiteral(":/data/Styles/style_%1.qss").arg(theme));
    qDebug() << file.fileName();
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
    /*
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
    */
    qApp->exit(1000);
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
