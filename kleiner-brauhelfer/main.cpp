#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QDirIterator>
#include <QFile>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "iSpindel/ispindel.h"

// Modus, um Datenbankupdates zu testen.
// In diesem Modus wird eine Kopie der Datenbank erstellt.
// Diese Kopie wird aktualisiert ohne die ursprüngliche Datenbank zu verändern.
//#define MODE_TEST_UPDATE

#if defined(QT_NO_DEBUG) && defined(MODE_TEST_UPDATE)
#error MODE_TEST_UPDATE in release build defined.
#endif

// global variables
extern Brauhelfer* bh;
extern Settings* gSettings;
Brauhelfer* bh = nullptr;
Settings* gSettings = nullptr;

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
        // connect
        bh->setDatabasePath(gSettings->databasePath());
        if (bh->connectDatabase())
        {
            // check database version
            int version = bh->databaseVersion();
            if (version > bh->supportedDatabaseVersion)
            {
                QMessageBox::critical(nullptr, QApplication::applicationName(),
                                      QObject::tr("Die Datenbankversion (%1) ist zu neu für das Programm. Das Programm muss aktualisiert werden.").arg(version));
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
                  #ifdef MODE_TEST_UPDATE
                    QFile fileOrg(gSettings->databasePath());
                    QFile fileUpdate(gSettings->databasePath() + "_update.sqlite");
                    fileUpdate.remove();
                    if (fileOrg.copy(fileUpdate.fileName()))
                        fileUpdate.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
                    bh->setDatabasePath(fileUpdate.fileName());
                    bh->connectDatabase();
                  #endif
                    try
                    {
                        if (bh->updateDatabase())
                        {
                            return bh->isConnectedDatabase();
                        }
                        else
                            QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Aktualisierung fehlgeschlagen."));
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

int main(int argc, char *argv[])
{
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

    // install translation
    QTranslator translatorQt;
    translatorQt.load(QLocale::system(), "qt", "_", "translations");
    if (translatorQt.isEmpty())
        translatorQt.load(QLocale::system(), "qt", "_", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!translatorQt.isEmpty())
        a.installTranslator(&translatorQt);

    // copy resources
    copyResources();

    // global brauhelfer class
    bh = new Brauhelfer();

    // run application
    int ret = -1;
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
            ret = a.exec();
        }
        else
        {
            ret = -1;
        }
    }
    while(ret == 1000);

    // clean up
    delete bh;
    delete gSettings;

    return ret;
}
