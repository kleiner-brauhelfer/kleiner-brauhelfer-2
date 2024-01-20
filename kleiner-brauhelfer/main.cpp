#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QMessageBox>
#if QT_NETWORK_LIB
#include <QSslSocket>
#endif
#include "brauhelfer.h"
#include "settings.h"
#include "widgets/webview.h"
#include "dialogs/dlgconsole.h"

// Modus, um Datenbankupdates zu testen.
// In diesem Modus wird eine Kopie der Datenbank erstellt.
// Diese Kopie wird aktualisiert ohne die ursprüngliche Datenbank zu verändern.
//#define MODE_TEST_UPDATE

#if defined(QT_NO_DEBUG) && defined(MODE_TEST_UPDATE)
#error MODE_TEST_UPDATE in release build defined.
#endif

// global variables
Brauhelfer* bh = nullptr;
Settings* gSettings = nullptr;

static QFile* logFile = nullptr;
static QtMessageHandler defaultMessageHandler;

static bool chooseDatabase()
{
    QString text;
    QString dir;
    if (gSettings->databasePath().isEmpty())
    {
        text = QObject::tr("Es wurde noch keine Datenbank ausgewählt. Soll eine neue angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = gSettings->settingsDir();
    }
    else
    {
        text = QObject::tr("Soll eine neue Datenbank angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = gSettings->databaseDir();
    }

    QMessageBox::StandardButton ret = QMessageBox::question(nullptr, QApplication::applicationName(), text, QMessageBox::Save | QMessageBox::Open | QMessageBox::Cancel);
    if (ret == QMessageBox::Open)
    {
        QString databasePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Datenbank auswählen"),
                                                            dir,
                                                            QObject::tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
        if (!databasePath.isEmpty())
        {
            gSettings->setDatabasePath(databasePath);
            return true;
        }
    }
    else if (ret == QMessageBox::Save)
    {
        QString databasePath = QFileDialog::getSaveFileName(nullptr, QObject::tr("Datenbank anlegen"),
                                                            dir + "/kb_daten.sqlite",
                                                            QObject::tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
        if (!databasePath.isEmpty())
        {
            QFile file(databasePath);
            QFile file2(QStringLiteral(":/data/kb_daten.sqlite"));
            file.remove();
            if (file2.copy(file.fileName()))
                file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            gSettings->setDatabasePath(databasePath);
            return true;
        }
    }
    return false;
}

static bool connectDatabase()
{
    while (true)
    {
        // global brauhelfer class
        if (bh)
            delete bh;
        bh = new Brauhelfer(gSettings->databasePath());

        // connect
        if (bh->connectDatabase())
        {
            // check database version
            int version = bh->databaseVersion();
            if (version < 0)
            {
                QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Die Datenbank ist ungultig."));
            }
            else if (version > bh->supportedDatabaseVersion)
            {
                QMessageBox::critical(nullptr, QApplication::applicationName(),
                                      QObject::tr("Die Datenbankversion (%1) ist zu neu für das Programm. Das Programm muss aktualisiert werden.").arg(version));
            }
            else if (version < bh->supportedDatabaseVersionMinimal)
            {
                QMessageBox::critical(nullptr, QApplication::applicationName(),
                                      QObject::tr("Die Datenbankversion (%1) ist zu alt für das Programm. Die Datenbank muss zuerst mit dem kleinen-brauhelfer v1.4.4.6 aktualisiert werden.").arg(version));
            }
            else if (version < bh->supportedDatabaseVersion)
            {
                QMessageBox::StandardButton ret = QMessageBox::warning(nullptr, QApplication::applicationName(),
                                                                       QObject::tr("Die Datenbank muss aktualisiert werden (Version %1 -> %2).").arg(version).arg(bh->supportedDatabaseVersion) + "\n\n" +
                                                                       QObject::tr("Soll die Datenbank jetzt aktualisiert werden?") + " " +
                                                                       QObject::tr("ACHTUNG, die Änderungen können nicht rückgängig gemacht werden!"),
                                                                       QMessageBox::Yes | QMessageBox::No,
                                                                       QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                {
                    QFile fileOrg(gSettings->databasePath());

                    // create backup of database
                    ret = QMessageBox::question(nullptr, QApplication::applicationName(), QObject::tr("Soll eine Sicherheitskopie der Datenbank angelegt werden?"));
                    if (ret == QMessageBox::Yes)
                    {
                        QString fileNameBackup = QFileDialog::getSaveFileName(nullptr, QObject::tr("Sicherheitskopie anlegen"),
                                                                              gSettings->databasePath(),
                                                                              QObject::tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
                        if (!fileNameBackup.isEmpty())
                        {
                            QFile fileBackup(fileNameBackup);
                            if (!fileOrg.copy(fileBackup.fileName()))
                            {
                                QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Sicherheitskopie konnte nicht erstellt werden."));
                                continue;
                            }
                        }
                    }

                    // create copy of database and update copy
                    QFile fileUpdate(gSettings->databasePath() + "_update.sqlite");
                    fileUpdate.remove();
                    if (fileOrg.copy(fileUpdate.fileName()))
                    {
                        fileUpdate.setPermissions(QFile::ReadOwner | QFile::WriteOwner);

                        // connect and update copy
                        bh->setDatabasePath(fileUpdate.fileName());
                        bh->connectDatabase();
                        if (bh->updateDatabase())
                        {
                            // delete bh to remove file
                            delete bh;
                            bh = nullptr;
                          #ifdef MODE_TEST_UPDATE
                            bh = new Brauhelfer(fileUpdate.fileName());
                            return bh->connectDatabase();
                          #else
                            // copy back
                            fileOrg.remove();
                            if (fileUpdate.copy(fileOrg.fileName()))
                            {
                                fileOrg.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
                                fileUpdate.remove();
                                continue;
                            }
                            else
                            {
                                QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Datenbank konnte nicht wiederhergestellt werden."));
                            }
                          #endif
                        }
                        else
                        {
                            fileUpdate.remove();
                            QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Aktualisierung fehlgeschlagen.\n\n") + bh->lastError());
                        }
                    }
                    else
                    {
                        QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Sicherheitskopie konnte nicht erstellt werden."));
                    }
                }
            }
            else
            {
                break;
            }
        }
        else if (!gSettings->databasePath().isEmpty())
        {
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                  QObject::tr("Die Datenbank \"%1\" konnte nicht geöffnet werden.").arg(bh->databasePath()));
        }

        // choose other database
        if (!chooseDatabase())
        {
            bh->disconnectDatabase();
            break;
        }
    }
    return bh->isConnectedDatabase();
}

static QByteArray getHash(const QString &fileName)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    hash.addData(file.readAll());
    return hash.result();
}

static void saveResourcesHash()
{
    gSettings->beginGroup("ResourcesHashes");
    QDirIterator it(QStringLiteral(":/data"), QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        if (it.fileName() == QStringLiteral("kb_daten.sqlite"))
            continue;
        if (it.fileInfo().isDir())
            continue;
        QByteArray hash = getHash(it.filePath());
        gSettings->setValue(it.filePath(), hash);
    }
    gSettings->endGroup();
}

static QByteArray getPreviousHash(const QString &fileName)
{
    return gSettings->valueInGroup("ResourcesHashes", fileName).toByteArray();
}

static void copyResources()
{
    bool update = gSettings->isNewProgramVersion();

    // create data directory
    QString dataDir = gSettings->dataDir(0);
    if (!QDir(dataDir).exists())
    {
        if (!QDir().mkpath(dataDir))
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                  QObject::tr("Der Ordner \"%1\" konnte nicht erstellt werden.").arg(dataDir));
    }

    // copy resources
    QDirIterator it(QStringLiteral(":/data"), QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        if (it.fileName() == QStringLiteral("kb_daten.sqlite"))
            continue;

        // create directory
        QString filePath = dataDir + it.filePath().mid(7);
        if (it.fileInfo().isDir())
        {
            QDir().mkpath(filePath);
            continue;
        }

        // copy file
        QFile fileResource(it.filePath());
        QFile fileLocal(filePath);
        if (!fileLocal.exists())
        {
            if (fileResource.copy(fileLocal.fileName()))
                fileLocal.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            else
                QMessageBox::critical(nullptr, QApplication::applicationName(),
                                      QObject::tr("Der Datei \"%1\" konnte nicht erstellt werden.").arg(fileLocal.fileName()));
        }
        else if (update)
        {
            QByteArray hashLocal = getHash(fileLocal.fileName());
            if (hashLocal != getHash(it.filePath()))
            {
                QMessageBox::StandardButton ret = QMessageBox::Yes;
                if (hashLocal != getPreviousHash(it.filePath()))
                {
                    ret = QMessageBox::question(nullptr, QApplication::applicationName(),
                                                QObject::tr("Die Ressourcendatei \"%1\" ist verschieden von der lokalen Datei.\n"
                                                            "Die Datei wurde entweder manuell editiert oder durch ein Update verändert.\n\n"
                                                            "Soll die lokale Datei ersetzt werden?").arg(it.fileName()));
                }
                if (ret == QMessageBox::Yes)
                {
                    fileLocal.remove();
                    if (fileResource.copy(fileLocal.fileName()))
                        fileLocal.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
                    else
                        QMessageBox::critical(nullptr, QApplication::applicationName(),
                                              QObject::tr("Der Datei \"%1\" konnte nicht erstellt werden.").arg(fileLocal.fileName()));
                }
            }
        }
    }

    // store hashes for next update
    if (update)
    {
        saveResourcesHash();
    }
}

static void checkWebView()
{
    WebView::setSupported(gSettings->valueInGroup("General", "WebViewEnabled", true).toBool());
}

static void checkSSL()
{
  #if QT_NETWORK_LIB
    QString buildVersion = QSslSocket::sslLibraryBuildVersionString();
    QString rutimeVersion = QSslSocket::sslLibraryVersionString();
    qInfo() << "SSL:" << "needed:" << buildVersion << "installed:" << rutimeVersion;
    if (!QSslSocket::supportsSsl())
    {
        qWarning() << "SSL not supported";
        if (gSettings->isNewProgramVersion())
        {
            QMessageBox::warning(nullptr, QApplication::applicationName(),
                                 QObject::tr("SSL wird nicht unterstüzt.\nVersion benötigt: %1\nVersion installiert: %2").arg(buildVersion, rutimeVersion));
        }
    }
  #endif
}

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    defaultMessageHandler(type, context, msg);
    QString entry;
    if (logFile || DlgConsole::Dialog)
    {
        QTextStream out(&entry);
        switch (type)
        {
        case QtDebugMsg:
            out << "DEBUG | ";
            break;
        case QtInfoMsg:
            out << "INFO  | ";
            break;
        case QtWarningMsg:
            out << "WARN  | ";
            break;
        case QtCriticalMsg:
            out << "ERROR | ";
            break;
        case QtFatalMsg:
            out << "FATAL | ";
            break;
        }
        out << QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yy hh::mm::ss.zzz")) << " | ";
        out << context.category << " | " << msg;
    }
    if (logFile)
    {
        if (!logFile->isOpen())
            logFile->open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream out(logFile);
        out << entry;
        if (context.file)
            out << " | " << context.file << ":" << context.line << ", " << context.function;
        out << Qt::endl;
    }
    if (DlgConsole::Dialog)
    {
        DlgConsole::Dialog->append(entry);
    }
}

static void installTranslator(QApplication &a, QTranslator &translator, const QString &filename)
{
    QLocale locale(gSettings->language());
    a.removeTranslator(&translator);
    if (translator.load(locale, filename, QStringLiteral("_"), a.applicationDirPath() + "/translations"))
        a.installTranslator(&translator);
    else if (translator.load(locale, filename, QStringLiteral("_"), QStringLiteral(":/translations")))
        a.installTranslator(&translator);
    else if (translator.load(locale, filename, QStringLiteral("_"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        a.installTranslator(&translator);
}

int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;

    QApplication a(argc, argv);

    // set application name, organization and version
    a.setOrganizationName(QStringLiteral(ORGANIZATION));
    a.setApplicationName(QStringLiteral(TARGET));
    a.setApplicationVersion(QStringLiteral(VERSION));

    // load settings
    if (QFile::exists(a.applicationDirPath() + "/portable"))
        gSettings = new Settings(QCoreApplication::applicationDirPath());
    else
        gSettings = new Settings();

    // logging
    defaultMessageHandler = qInstallMessageHandler(messageHandler);
    int logLevel = gSettings->logLevel();
    if (logLevel > 100)
        logFile = new QFile(gSettings->settingsDir() + "/logfile.txt");
    gSettings->initLogLevel(logLevel);

    // modules
    gSettings->initModules();

    qInfo("--- Application start ---");
    qInfo() << "Version:" << QCoreApplication::applicationVersion();
    if (logLevel > 0)
        qInfo() << "Log level:" << logLevel;

    // language
    QTranslator translatorQt, translatorKbh;
    installTranslator(a, translatorQt, QStringLiteral("qtbase"));
    installTranslator(a, translatorKbh, QStringLiteral("kbh"));

    // locale
    bool useLanguageLocale = gSettings->valueInGroup("General", "UseLanguageLocale", false).toBool();
    if (useLanguageLocale)
        QLocale::setDefault(QLocale(gSettings->language()));

    // do some checks
    checkSSL();
    checkWebView();

    try
    {
        // copy resources
        copyResources();

        // run application
        do
        {
            if (connectDatabase())
            {
                MainWindow w(nullptr);
                a.setStyle(QStyleFactory::create(gSettings->style()));
                a.setPalette(gSettings->palette);
                a.setFont(gSettings->font);
                w.show();
                try
                {
                    ret = a.exec();
                }
                catch (const std::exception& ex)
                {
                    qCritical() << "Program error:" << ex.what();
                    QMessageBox::critical(nullptr, QObject::tr("Programmfehler"), ex.what());
                    ret = EXIT_FAILURE;
                }
                catch (...)
                {
                    qCritical() << "Program error: unknown";
                    QMessageBox::critical(nullptr, QObject::tr("Programmfehler"), QObject::tr("Unbekannter Fehler."));
                    ret = EXIT_FAILURE;
                }
                if (ret == 1001)
                {
                    installTranslator(a, translatorQt, QStringLiteral("qtbase"));
                    installTranslator(a, translatorKbh, QStringLiteral("kbh"));
                    bool useLanguageLocale = gSettings->valueInGroup("General", "UseLanguageLocale", false).toBool();
                    QLocale::setDefault(useLanguageLocale ? QLocale(gSettings->language()) : QLocale::system());
                    ret = 1000;
                }
            }
            else
            {
                ret = EXIT_FAILURE;
            }
        }
        while(ret == 1000);
    }
    catch (const std::exception& ex)
    {
        qCritical() << "Program error:" << ex.what();
        QMessageBox::critical(nullptr, QObject::tr("Programmfehler"), ex.what());
        ret = EXIT_FAILURE;
    }
    catch (...)
    {
        qCritical() << "Program error: unknown";
        QMessageBox::critical(nullptr, QObject::tr("Programmfehler"), QObject::tr("Unbekannter Fehler."));
        ret = EXIT_FAILURE;
    }

    // clean up
    if (bh)
        delete bh;
    delete gSettings;

    qInfo("--- Application end (%d) ---", ret);

    // close log
    if (logFile)
    {
        logFile->close();
        delete logFile;
    }

    return ret;
}
