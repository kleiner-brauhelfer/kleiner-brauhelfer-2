#include "database.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "brauhelfer.h"

Database::Database() :
    mVersion(-1)
{
    mDb = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    if (mDb->open())
        mDb->close();
}

void Database::createTables(Brauhelfer* bh)
{
    modelSud = new ModelSud(bh, *mDb);
    modelRasten = new ModelRasten(bh, *mDb);
    modelMalzschuettung = new ModelMalzschuettung(bh, *mDb);
    modelHopfengaben = new ModelHopfengaben(bh, *mDb);
    modelWeitereZutatenGaben = new ModelWeitereZutatenGaben(bh, *mDb);
    modelSchnellgaerverlauf = new ModelSchnellgaerverlauf(bh, *mDb);
    modelHauptgaerverlauf = new ModelHauptgaerverlauf(bh, *mDb);
    modelNachgaerverlauf = new ModelNachgaerverlauf(bh, *mDb);
    modelBewertungen = new ModelBewertungen(bh, *mDb);
    modelMalz = new ModelMalz(bh, *mDb);
    modelHopfen = new ModelHopfen(bh, *mDb);
    modelHefe = new ModelHefe(bh, *mDb);
    modelWeitereZutaten = new ModelWeitereZutaten(bh, *mDb);
    modelAnhang = new SqlTableModel(bh, *mDb);
    modelAusruestung = new ModelAusruestung(bh, *mDb);
    modelWasser = new ModelWasser(bh, *mDb);
    modelFlaschenlabel = new SqlTableModel(bh, *mDb);
    modelFlaschenlabelTags = new ModelFlaschenlabelTags(bh, *mDb);
    modelSud->createConnections();
    if (mDb->open())
        mDb->close();
}

void Database::setTables()
{
    modelSud->setTable("Sud");
    modelSud->setSortByFieldName("Braudatum", Qt::DescendingOrder);
    modelRasten->setTable("Rasten");
    modelMalzschuettung->setTable("Malzschuettung");
    modelHopfengaben->setTable("Hopfengaben");
    modelWeitereZutatenGaben->setTable("WeitereZutatenGaben");
    modelSchnellgaerverlauf->setTable("Schnellgaerverlauf");
    modelSchnellgaerverlauf->setSortByFieldName("Zeitstempel", Qt::AscendingOrder);
    modelHauptgaerverlauf->setTable("Hauptgaerverlauf");
    modelHauptgaerverlauf->setSortByFieldName("Zeitstempel", Qt::AscendingOrder);
    modelNachgaerverlauf->setTable("Nachgaerverlauf");
    modelNachgaerverlauf->setSortByFieldName("Zeitstempel", Qt::AscendingOrder);
    modelBewertungen->setTable("Bewertungen");
    modelBewertungen->setSortByFieldName("Datum", Qt::AscendingOrder);
    modelMalz->setTable("Malz");
    modelHopfen->setTable("Hopfen");
    modelHefe->setTable("Hefe");
    modelWeitereZutaten->setTable("WeitereZutaten");
    modelAnhang->setTable("Anhang");
    modelAusruestung->setTable("Ausruestung");
    modelWasser->setTable("Wasser");
    modelFlaschenlabel->setTable("Flaschenlabel");
    modelFlaschenlabelTags->setTable("FlaschenlabelTags");
}

Database::~Database()
{
    QString connectionName = mDb->connectionName();
    disconnect();
    QSqlDatabase::removeDatabase(connectionName);
    delete mDb;
    delete modelSud;
    delete modelRasten;
    delete modelMalzschuettung;
    delete modelHopfengaben;
    delete modelWeitereZutatenGaben;
    delete modelSchnellgaerverlauf;
    delete modelHauptgaerverlauf;
    delete modelNachgaerverlauf;
    delete modelBewertungen;
    delete modelMalz;
    delete modelHopfen;
    delete modelHefe;
    delete modelWeitereZutaten;
    delete modelAnhang;
    delete modelAusruestung;
    delete modelWasser;
    delete modelFlaschenlabel;
    delete modelFlaschenlabelTags;
}

bool Database::connect(const QString &dbPath, bool readonly)
{
    if (!isConnected())
    {
        if (QFile::exists(dbPath))
        {
            mDb->setDatabaseName(dbPath);
            if (readonly)
                mDb->setConnectOptions("QSQLITE_OPEN_READONLY");
            if (mDb->open())
            {
                QSqlQuery query;
                if (query.exec("SELECT db_Version FROM Global"))
                {
                    if (query.first())
                    {
                        int version = query.value(0).toInt();
                        if (version > 0)
                        {
                            mVersion = version;
                            setTables();
                            return true;
                        }
                    }
                }
            }
            disconnect();
        }
    }
    return false;
}

void Database::disconnect()
{
    if (isConnected())
    {
        modelSud->clear();
        modelRasten->clear();
        modelMalzschuettung->clear();
        modelHopfengaben->clear();
        modelWeitereZutatenGaben->clear();
        modelSchnellgaerverlauf->clear();
        modelHauptgaerverlauf->clear();
        modelNachgaerverlauf->clear();
        modelBewertungen->clear();
        modelMalz->clear();
        modelHopfen->clear();
        modelHefe->clear();
        modelWeitereZutaten->clear();
        modelAnhang->clear();
        modelAusruestung->clear();
        modelWasser->clear();
        modelFlaschenlabel->clear();
        modelFlaschenlabelTags->clear();
        mDb->close();
        mVersion = -1;
    }
}

bool Database::isConnected() const
{
    return mDb->isOpen();
}

bool Database::isDirty() const
{
    return modelSud->isDirty() |
           modelRasten->isDirty() |
           modelMalzschuettung->isDirty() |
           modelHopfengaben->isDirty() |
           modelWeitereZutatenGaben->isDirty() |
           modelSchnellgaerverlauf->isDirty() |
           modelHauptgaerverlauf->isDirty() |
           modelNachgaerverlauf->isDirty() |
           modelBewertungen->isDirty() |
           modelMalz->isDirty() |
           modelHopfen->isDirty() |
           modelHefe->isDirty() |
           modelWeitereZutaten->isDirty() |
           modelAnhang->isDirty() |
           modelAusruestung->isDirty() |
           modelWasser->isDirty() |
           modelFlaschenlabel->isDirty() |
           modelFlaschenlabelTags->isDirty();
}

void Database::select()
{
    modelMalz->select();
    modelHopfen->select();
    modelHefe->select();
    modelWeitereZutaten->select();
    modelWasser->select();
    modelAusruestung->select();
    modelRasten->select();
    modelMalzschuettung->select();
    modelHopfengaben->select();
    modelWeitereZutatenGaben->select();
    modelSchnellgaerverlauf->select();
    modelHauptgaerverlauf->select();
    modelNachgaerverlauf->select();
    modelBewertungen->select();
    modelAnhang->select();
    modelFlaschenlabel->select();
    modelFlaschenlabelTags->select();
    modelSud->select();
}

int Database::version() const
{
    return mVersion;
}

void Database::save()
{
    modelMalz->submitAll();
    modelHopfen->submitAll();
    modelHefe->submitAll();
    modelWeitereZutaten->submitAll();
    modelWasser->submitAll();
    modelAusruestung->submitAll();
    modelRasten->submitAll();
    modelMalzschuettung->submitAll();
    modelHopfengaben->submitAll();
    modelWeitereZutatenGaben->submitAll();
    modelSchnellgaerverlauf->submitAll();
    modelHauptgaerverlauf->submitAll();
    modelNachgaerverlauf->submitAll();
    modelBewertungen->submitAll();
    modelAnhang->submitAll();
    modelFlaschenlabel->submitAll();
    modelFlaschenlabelTags->submitAll();
    modelSud->submitAll();
}

void Database::discard()
{
    modelMalz->revertAll();
    modelHopfen->revertAll();
    modelHefe->revertAll();
    modelWeitereZutaten->revertAll();
    modelWasser->revertAll();
    modelAusruestung->revertAll();
    modelRasten->revertAll();
    modelMalzschuettung->revertAll();
    modelHopfengaben->revertAll();
    modelWeitereZutatenGaben->revertAll();
    modelSchnellgaerverlauf->revertAll();
    modelHauptgaerverlauf->revertAll();
    modelNachgaerverlauf->revertAll();
    modelBewertungen->revertAll();
    modelAnhang->revertAll();
    modelFlaschenlabel->revertAll();
    modelFlaschenlabelTags->revertAll();
    modelSud->revertAll();
}

QSqlQuery Database::sqlExec(const QString &query)
{
    QSqlQuery sqlQuery = mDb->exec(query);
    QSqlError lastError = mDb->lastError();
    if (lastError.isValid())
    {
        QString str = "Query: " + query +
                "\nError: " + lastError.databaseText() +
                "\n" + lastError.driverText();
        throw std::runtime_error(str.toStdString().c_str());
    }
    return sqlQuery;
}

void Database::update()
{
    int version = mVersion;
    try
    {
        if (version == 24)
        {
            version = 2000;
            mDb->transaction();

            // Ausruestung
            //  - neue Spalte 'Typ'
            //                'VerlustNachKochen'
            //  - Spalte gelöscht 'AnlagenID'
            sqlExec("ALTER TABLE Ausruestung RENAME TO TempTable");
            sqlExec("CREATE TABLE Ausruestung ("
                "ID INTEGER PRIMARY KEY,"
                "Name TEXT,"
                "Typ INTEGER DEFAULT 0,"
                "Sudhausausbeute REAL DEFAULT 60,"
                "Verdampfungsziffer REAL DEFAULT 10,"
                "KorrekturWasser REAL DEFAULT 0,"
                "KorrekturFarbe REAL DEFAULT 0,"
                "VerlustNachKochen REAL DEFAULT 0,"
                "Maischebottich_Hoehe REAL DEFAULT 0,"
                "Maischebottich_Durchmesser REAL DEFAULT 0,"
                "Maischebottich_MaxFuellhoehe REAL DEFAULT 0,"
                "Sudpfanne_Hoehe REAL DEFAULT 0,"
                "Sudpfanne_Durchmesser REAL DEFAULT 0,"
                "Sudpfanne_MaxFuellhoehe REAL DEFAULT 0,"
                "Kosten REAL DEFAULT 0)");
            sqlExec("INSERT INTO Ausruestung ("
                "Name,"
                "Sudhausausbeute,"
                "Verdampfungsziffer,"
                "KorrekturWasser,"
                "KorrekturFarbe,"
                "Maischebottich_Hoehe,"
                "Maischebottich_Durchmesser,"
                "Maischebottich_MaxFuellhoehe,"
                "Sudpfanne_Hoehe,"
                "Sudpfanne_Durchmesser,"
                "Sudpfanne_MaxFuellhoehe,"
                "Kosten"
                ") SELECT "
                "Name,"
                "Sudhausausbeute,"
                "Verdampfungsziffer,"
                "KorrekturWasser,"
                "KorrekturFarbe,"
                "Maischebottich_Hoehe,"
                "Maischebottich_Durchmesser,"
                "Maischebottich_MaxFuellhoehe,"
                "Sudpfanne_Hoehe,"
                "Sudpfanne_Durchmesser,"
                "Sudpfanne_MaxFuellhoehe,"
                "Kosten"
                " FROM TempTable");
            sqlExec("DROP TABLE TempTable");

            // Wasser
            //  - neue Spalte 'Name'
            sqlExec("ALTER TABLE Wasser RENAME TO TempTable");
            sqlExec("CREATE TABLE Wasser ("
                "ID INTEGER PRIMARY KEY,"
                "Name TEXT,"
                "Calcium REAL DEFAULT 0,"
                "Magnesium REAL DEFAULT 0,"
                "Saeurekapazitaet REAL DEFAULT 0)");
            sqlExec("INSERT INTO Wasser ("
                "Calcium,"
                "Magnesium,"
                "Saeurekapazitaet"
                ") SELECT "
                "Calcium,"
                "Magnesium,"
                "Saeurekapazitaet"
                " FROM TempTable");
            sqlExec("UPDATE Wasser SET Name='Profil 1'");
            sqlExec("DROP TABLE TempTable");

            // Sud
            //  - neue Spalte 'Wasserprofil'
            //  - Spalte gelöscht 'Anstelldatum'
            //                    'AktivTab'
            //                    'Bewertung'
            //                    'BewertungText'
            //                    'AktivTab_Gaerverlauf'
            //                    'SWVorHopfenseihen'
            //                    'BewertungMaxSterne'
            //                    'NeuBerechnen'
            //                    'AuswahlBrauanlage'
            //  - Spalte unbenannt 'erg_S_Gesammt' -> 'erg_S_Gesamt'
            //                     'erg_W_Gesammt' -> 'erg_W_Gesamt'
            //                     'AuswahlBrauanlageName' -> 'Anlage'
            sqlExec("ALTER TABLE Sud RENAME TO TempTable");
            sqlExec("CREATE TABLE Sud ("
                "ID INTEGER PRIMARY KEY,"
                "Sudname TEXT,"
                "Menge REAL DEFAULT 20,"
                "SW REAL DEFAULT 12,"
                "CO2 REAL DEFAULT 5,"
                "IBU REAL DEFAULT 26,"
                "Kommentar TEXT,"
                "Braudatum DATETIME,"
                "BierWurdeGebraut INTEGER DEFAULT 0,"
                "WuerzemengeAnstellen REAL DEFAULT 20,"
                "SWAnstellen REAL DEFAULT 12,"
                "Abfuelldatum DATETIME,"
                "BierWurdeAbgefuellt INTEGER DEFAULT 0,"
                "SWSchnellgaerprobe REAL DEFAULT 2.5,"
                "SWJungbier REAL DEFAULT 3,"
                "TemperaturJungbier REAL DEFAULT 12,"
                "WuerzemengeKochende REAL DEFAULT 20,"
                "Speisemenge REAL DEFAULT 1,"
                "SWKochende REAL DEFAULT 12,"
                "AuswahlHefe TEXT,"
                "FaktorHauptguss REAL DEFAULT 3.5,"
                "KochdauerNachBitterhopfung INTEGER DEFAULT 90,"
                "EinmaischenTemp INTEGER DEFAULT 60,"
                "Erstellt DATETIME,"
                "Gespeichert DATETIME,"
                "erg_S_Gesamt REAL DEFAULT 0,"
                "erg_W_Gesamt REAL DEFAULT 0,"
                "erg_WHauptguss REAL DEFAULT 0,"
                "erg_WNachguss REAL DEFAULT 0,"
                "erg_Sudhausausbeute REAL DEFAULT 0,"
                "erg_Farbe REAL DEFAULT 0,"
                "erg_Preis REAL DEFAULT 0,"
                "erg_Alkohol REAL DEFAULT 0,"
                "KostenWasserStrom REAL DEFAULT 0,"
                "Reifezeit INTEGER DEFAULT 4,"
                "BierWurdeVerbraucht INTEGER DEFAULT 0,"
                "Nachisomerisierungszeit INTEGER DEFAULT 0,"
                "WuerzemengeVorHopfenseihen REAL DEFAULT 0,"
                "erg_EffektiveAusbeute REAL DEFAULT 0,"
                "RestalkalitaetSoll REAL DEFAULT 0,"
                "SchnellgaerprobeAktiv INTEGER DEFAULT 0,"
                "JungbiermengeAbfuellen REAL DEFAULT 0,"
                "erg_AbgefuellteBiermenge REAL DEFAULT 0,"
                "HefeAnzahlEinheiten INTEGER DEFAULT 1,"
                "berechnungsArtHopfen INTEGER DEFAULT 0,"
                "highGravityFaktor REAL DEFAULT 0,"
                "Anlage TEXT,"
                "AusbeuteIgnorieren INTEGER DEFAULT 0,"
                "MerklistenID INTEGER DEFAULT 0,"
                "Spunden INTEGER DEFAULT 0,"
                "Sudnummer INTEGER DEFAULT 0,"
                "Wasserprofil TEXT)");
            sqlExec("INSERT INTO Sud ("
                "ID,"
                "Sudname,"
                "Menge,"
                "SW,"
                "CO2,"
                "IBU,"
                "Kommentar,"
                "Braudatum,"
                "BierWurdeGebraut,"
                "WuerzemengeAnstellen,"
                "SWAnstellen,"
                "Abfuelldatum,"
                "BierWurdeAbgefuellt,"
                "SWSchnellgaerprobe,"
                "SWJungbier,"
                "TemperaturJungbier,"
                "WuerzemengeKochende,"
                "Speisemenge,"
                "SWKochende,"
                "AuswahlHefe,"
                "FaktorHauptguss,"
                "KochdauerNachBitterhopfung,"
                "EinmaischenTemp,"
                "Erstellt,"
                "Gespeichert,"
                "erg_S_Gesamt,"
                "erg_W_Gesamt,"
                "erg_WHauptguss,"
                "erg_WNachguss,"
                "erg_Sudhausausbeute,"
                "erg_Farbe,"
                "erg_Preis,"
                "erg_Alkohol,"
                "KostenWasserStrom,"
                "Reifezeit,"
                "BierWurdeVerbraucht,"
                "Nachisomerisierungszeit,"
                "WuerzemengeVorHopfenseihen,"
                "erg_EffektiveAusbeute,"
                "RestalkalitaetSoll,"
                "SchnellgaerprobeAktiv,"
                "JungbiermengeAbfuellen,"
                "erg_AbgefuellteBiermenge,"
                "HefeAnzahlEinheiten,"
                "berechnungsArtHopfen,"
                "highGravityFaktor,"
                "Anlage,"
                "AusbeuteIgnorieren,"
                "MerklistenID,"
                "Spunden,"
                "Sudnummer"
                ") SELECT "
                "ID,"
                "Sudname,"
                "Menge,"
                "SW,"
                "CO2,"
                "IBU,"
                "Kommentar,"
                "Braudatum,"
                "BierWurdeGebraut,"
                "WuerzemengeAnstellen,"
                "SWAnstellen,"
                "Abfuelldatum,"
                "BierWurdeAbgefuellt,"
                "SWSchnellgaerprobe,"
                "SWJungbier,"
                "TemperaturJungbier,"
                "WuerzemengeKochende,"
                "Speisemenge,"
                "SWKochende,"
                "AuswahlHefe,"
                "FaktorHauptguss,"
                "KochdauerNachBitterhopfung,"
                "EinmaischenTemp,"
                "Erstellt,"
                "Gespeichert,"
                "erg_S_Gesammt,"
                "erg_W_Gesammt,"
                "erg_WHauptguss,"
                "erg_WNachguss,"
                "erg_Sudhausausbeute,"
                "erg_Farbe,"
                "erg_Preis,"
                "erg_Alkohol,"
                "KostenWasserStrom,"
                "Reifezeit,"
                "BierWurdeVerbraucht,"
                "Nachisomerisierungszeit,"
                "WuerzemengeVorHopfenseihen,"
                "erg_EffektiveAusbeute,"
                "RestalkalitaetSoll,"
                "SchnellgaerprobeAktiv,"
                "JungbiermengeAbfuellen,"
                "erg_AbgefuellteBiermenge,"
                "HefeAnzahlEinheiten,"
                "berechnungsArtHopfen,"
                "highGravityFaktor,"
                "AuswahlBrauanlageName,"
                "AusbeuteIgnorieren,"
                "MerklistenID,"
                "Spunden,"
                "Sudnummer"
                " FROM TempTable");
            sqlExec("UPDATE Sud SET Wasserprofil='Profil 1'");
            sqlExec("DROP TABLE TempTable");

            // Geraete
            //  - Tabelle gelöscht
            sqlExec("DROP TABLE Geraete");

            // IgnorMsgID
            //  - Tabelle gelöscht
            sqlExec("DROP TABLE IgnorMsgID");

            // Rastauswahl
            //  - Tabelle gelöscht
            sqlExec("DROP TABLE Rastauswahl");

            // Global
            //  - Spalte gelöscht 'db_NeuBerechnen'
            sqlExec("DROP TABLE Global");
            sqlExec("CREATE TABLE Global (db_Version INTEGER)");
            sqlExec(QString("INSERT INTO Global (db_Version) VALUES (%1)").arg(version));

            mDb->commit();
        }

        /*
        if (version == 2000)
        {
            ++version;
            mDb->transaction();
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb->commit();
        }
        */
    }
    catch (const std::exception& ex)
    {
        mDb->rollback();
        throw ex;
    }
    catch (...)
    {
        mDb->rollback();
        throw std::runtime_error("unknown error");
    }
}
