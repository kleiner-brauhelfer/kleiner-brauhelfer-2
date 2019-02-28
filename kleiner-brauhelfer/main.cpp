#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"

// Modus, um Datenbankupdates zu testen.
// In diesem Modus wird eine Kopie der Datenbank erstellt.
// Diese Kopie wird aktualisiert ohne die ursprüngliche Datenbank zu verändern.
#define MODE_TEST_UPDATE

#if defined(QT_NO_DEBUG) && defined(MODE_TEST_UPDATE)
#error MODE_TEST_UPDATE in release build defined.
#endif

// global variables
extern Brauhelfer* bh;
extern Settings* gSettings;
Brauhelfer* bh = nullptr;
Settings* gSettings = nullptr;

static int chooseDatabase()
{
    QString text;
    QString dir;
    if (gSettings->databasePath().isEmpty())
    {
        text = QObject::tr("Es wurde noch keine Datenbankdatei ausgewählt. Soll eine neue angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = QApplication::applicationDirPath();
    }
    else
    {
        text = QObject::tr("Soll eine neue Datenbankdatei angelegt werden oder soll eine bereits vorhandene geöffnet werden?");
        dir = gSettings->databaseDir();
    }

    int ret = QMessageBox::question(nullptr, QApplication::applicationName(),
                                    text,
                                    QMessageBox::Open | QMessageBox::Yes | QMessageBox::Cancel,
                                    QMessageBox::Yes);
    if (ret == QMessageBox::Open)
    {
        QString databasePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Datenbankdatei auswählen"),
                                                            dir,
                                                            QObject::tr("Datenbank") + " (*.sqlite)");
        if (!databasePath.isEmpty())
        {
            gSettings->setDatabasePath(databasePath);
        }
        else
        {
            ret = QMessageBox::Cancel;
        }
    }
    else if (ret == QMessageBox::Yes)
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
        }
        else
        {
            ret = QMessageBox::Cancel;
        }
    }
    return ret;
}

static bool connectDatabase(bool &updated)
{
    updated = false;
    while (true)
    {
        // connect
        bh->setDatabasePath(gSettings->databasePath());
        if (bh->connectDatabase())
        {
            // check database version
            if (bh->databaseVersion() > bh->supportedDatabaseVersion)
            {
                QMessageBox::critical(nullptr, QApplication::applicationName(),
                                      QObject::tr("Die Datenbankversion ist zu neu für das Programm. Das Programm muss aktualisiert werden."));
            }
            else if (bh->databaseVersion() < bh->supportedDatabaseVersion)
            {
                int ret = QMessageBox::warning(nullptr, QApplication::applicationName(),
                                               QObject::tr("Die Datenbankdatei muss aktualisiert werden.") + " " +
                                               QObject::tr("Soll die Datenbankdatei jetzt aktualisiert werden?") + " " +
                                               QObject::tr("Achtung, die Änderungen können nicht rückgängig gemacht werden."),
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
                            updated = true;
                            return bh->isConnectedDatabase();
                        }
                        else
                            QMessageBox::critical(nullptr, QApplication::applicationName(), QObject::tr("Aktualisierung fehlgeschlagen."));
                    }
                    catch (const std::exception& ex)
                    {
                        QMessageBox::critical(nullptr, QObject::tr("SQL Fehler"), ex.what());
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
        if (chooseDatabase() == QMessageBox::Cancel)
        {
            bh->disconnectDatabase();
            break;
        }
    }
    return bh->isConnectedDatabase();
}

static void copyResources()
{
    QString settingsDir = gSettings->settingsDir();
    QDirIterator it(":/data", QDirIterator::Subdirectories);
    if (!QDir(settingsDir).exists())
        QDir().mkdir(settingsDir);
    while (it.hasNext())
    {
        it.next();
        if (it.fileName() == "kb_daten.sqlite")
            continue;
        QFile file(settingsDir + it.fileName());
        if (!file.exists())
        {
            QFile file2(it.filePath());
            if (file2.copy(file.fileName()))
                file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
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
        gSettings = new SettingsPortable();
    else
        gSettings = new Settings();

    // copy resources
    copyResources();

    // global brauhelfer class
    bh = new Brauhelfer();

    // run application
    int ret = -1;
    bool updated;
    do
    {
        if (connectDatabase(updated))
        {
            MainWindow w(nullptr, updated);
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
