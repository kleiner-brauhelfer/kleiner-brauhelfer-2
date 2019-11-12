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
#include "brauhelfer.h"
#include "settings.h"

// global variables
extern Brauhelfer* bh;
extern Settings* gSettings;
Brauhelfer* bh = nullptr;
Settings* gSettings = nullptr;

static QFile* logFile = nullptr;

static bool chooseDatabase()
{
    QString text;
    QString dir;
    if (gSettings->databasePath().isEmpty())
    {
        text = QObject::tr("Es wurde noch keine Datenbankdatei ausgewählt. Soll eine neue angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = gSettings->settingsDir();
    }
    else
    {
        text = QObject::tr("Soll eine neue Datenbankdatei angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = gSettings->databaseDir();
    }

    int ret = QMessageBox::question(nullptr, QApplication::applicationName(), text,
                                    QObject::tr("Anlegen"), QObject::tr("Öffnen"), QObject::tr("Abbrechen"));
    if (ret == 1)
    {
        QString databasePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Datenbankdatei auswählen"),
                                                            dir,
                                                            QObject::tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
        if (!databasePath.isEmpty())
        {
            gSettings->setDatabasePath(databasePath);
            return true;
        }
    }
    else if (ret == 0)
    {
        QString databasePath = QFileDialog::getSaveFileName(nullptr, QObject::tr("Datenbankdatei anlegen"),
                                                            dir + "/kb_daten.sqlite",
                                                            QObject::tr("Datenbank") + " (*.sqlite)");
        if (!databasePath.isEmpty())
        {
            QFile file(databasePath);
            QFile file2(":/data/kb_daten.sqlite");
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
                int ret = QMessageBox::warning(nullptr, QApplication::applicationName(),
                                               QObject::tr("Die Datenbankdatei muss aktualisiert werden (version %1 -> %2).").arg(version).arg(bh->supportedDatabaseVersion) + "\n\n" +
                                               QObject::tr("Soll die Datenbankdatei jetzt aktualisiert werden?") + " " +
                                               QObject::tr("ACHTUNG, die Änderungen können nicht rückgängig gemacht werden!"),
                                               QMessageBox::Yes | QMessageBox::No,
                                               QMessageBox::Yes);
                if (ret == QMessageBox::Yes)
                {
                    try
                    {
                        if (bh->updateDatabase())
                        {
                            continue;
                        }
                        else
                        {
                            QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Aktualisierung fehlgeschlagen."));
                        }
                    }
                    catch (const std::exception& ex)
                    {
                        QMessageBox::critical(nullptr, QObject::tr("SQL Fehler"), ex.what());
                    }
                    catch (...)
                    {
                        QMessageBox::critical(nullptr, QObject::tr("SQL Fehler"), QObject::tr("Unbekannter Fehler."));
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
                                  QObject::tr("Die Datenbankdatei \"%1\" konnte nicht geöffnet werden.").arg(bh->databasePath()));
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

static void copyResources()
{
    QString dataDir = gSettings->dataDir();
    bool update = gSettings->isNewProgramVersion();
    QDirIterator it(":/data", QDirIterator::Subdirectories);
    if (!QDir(dataDir).exists())
    {
        if (!QDir().mkpath(dataDir))
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                  QObject::tr("Der Ordner \"%1\" konnte nicht erstellt werden.").arg(dataDir));
    }
    while (it.hasNext())
    {
        it.next();
        if (it.fileName() == "kb_daten.sqlite")
            continue;
        QFile fileResource(it.filePath());
        QFile fileLocal(dataDir + it.fileName());
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
            QByteArray hashResource = getHash(fileResource.fileName());
            QByteArray hashLocal = getHash(fileLocal.fileName());
            if (hashLocal != hashResource)
            {
                QMessageBox::StandardButton ret = QMessageBox::question(nullptr, QApplication::applicationName(),
                                                QObject::tr("Die Ressourcendatei \"%1\" ist verschieden von der lokalen Datei.\n"
                                                            "Die Datei wurde entweder manuell editiert oder durch ein Update verändert.\n\n"
                                                            "Soll die lokale Datei ersetzt werden?").arg(it.fileName()));
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
}

static void messageHandlerFileOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!logFile->isOpen())
        logFile->open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out(logFile);
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
    out << QDateTime::currentDateTime().toString("dd.MM.yy hh::mm::ss.zzz") << " | ";
    out << msg;
    if (context.file)
        out << " | " << context.file << ":" << context.line << ", " << context.function;
    out << endl;
}

int main(int argc, char *argv[])
{
    int ret = EXIT_FAILURE;

    QApplication a(argc, argv);

    // set application name, organization and version
    a.setOrganizationName(QString(ORGANIZATION));
    a.setApplicationName(QString(TARGET));
    a.setApplicationVersion(QString(VERSION));

    // load settings
    if (QFile::exists(a.applicationDirPath() + "/portable"))
        gSettings = new Settings(QCoreApplication::applicationDirPath());
    else
        gSettings = new Settings();

    qInfo("--- Application start ---");
    qInfo() << "Version:" << QCoreApplication::applicationVersion();

    // logging
    int logLevel = gSettings->logLevel();
    if (logLevel > 0)
    {
        qInfo() << "Log level:" << logLevel;
        if (logLevel < 1000)
        {
            //log in file
            logFile = new QFile(gSettings->settingsDir() + "/logfile.txt");
            qInstallMessageHandler(messageHandlerFileOutput);
        }
        else
        {
            // log in console
           logLevel -= 1000;
        }
        switch (logLevel)
        {
        case 1:
            QLoggingCategory::setFilterRules("*.debug=false");
            break;
        case 2:
            break;
        case 3:
            QLoggingCategory::setFilterRules("SqlTableModel.info=true");
            break;
        case 4:
        default:
            QLoggingCategory::setFilterRules("SqlTableModel.info=true\nSqlTableModel.debug=true");
            break;
        case 999:
            QLoggingCategory::setFilterRules("*.info=true\n*.debug=true");
            break;
        }
    }

    // install translation
    QTranslator translatorQt;
    translatorQt.load(QLocale::system(), "qt", "_", "translations");
    if (translatorQt.isEmpty())
        translatorQt.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!translatorQt.isEmpty())
        a.installTranslator(&translatorQt);

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
                if (!gSettings->useSystemFont())
                    w.setFont(gSettings->font);
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

    qInfo("--- Application end (%d)---", ret);

    // close log
    if (logFile)
    {
        logFile->close();
        delete logFile;
    }

    return ret;
}
