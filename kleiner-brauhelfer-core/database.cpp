#include "database.h"
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include "brauhelfer.h"

Database::Database() :
    mDb(QSqlDatabase::addDatabase("QSQLITE", "kbh")),
    mVersion(-1)
{
    if (mDb.open())
        mDb.close();
}

void Database::createTables(Brauhelfer* bh)
{
    modelSud = new ModelSud(bh, mDb);
    modelRasten = new ModelRasten(bh, mDb);
    modelMalzschuettung = new ModelMalzschuettung(bh, mDb);
    modelHopfengaben = new ModelHopfengaben(bh, mDb);
    modelHefegaben = new ModelHefegaben(bh, mDb);
    modelWeitereZutatenGaben = new ModelWeitereZutatenGaben(bh, mDb);
    modelSchnellgaerverlauf = new ModelSchnellgaerverlauf(bh, mDb);
    modelHauptgaerverlauf = new ModelHauptgaerverlauf(bh, mDb);
    modelNachgaerverlauf = new ModelNachgaerverlauf(bh, mDb);
    modelBewertungen = new ModelBewertungen(bh, mDb);
    modelMalz = new ModelMalz(bh, mDb);
    modelHopfen = new ModelHopfen(bh, mDb);
    modelHefe = new ModelHefe(bh, mDb);
    modelWeitereZutaten = new ModelWeitereZutaten(bh, mDb);
    modelAnhang = new SqlTableModel(bh, mDb);
    modelAusruestung = new ModelAusruestung(bh, mDb);
    modelGeraete = new SqlTableModel(bh, mDb);
    modelWasser = new ModelWasser(bh, mDb);
    modelFlaschenlabel = new SqlTableModel(bh, mDb);
    modelFlaschenlabelTags = new ModelFlaschenlabelTags(bh, mDb);
    modelSud->createConnections();
    if (mDb.open())
        mDb.close();
}

void Database::setTables()
{
    modelSud->setTable("Sud");
    modelSud->setSortByFieldName("Braudatum", Qt::DescendingOrder);
    modelRasten->setTable("Rasten");
    modelMalzschuettung->setTable("Malzschuettung");
    modelHopfengaben->setTable("Hopfengaben");
    modelHefegaben->setTable("Hefegaben");
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
    modelGeraete->setTable("Geraete");
    modelWasser->setTable("Wasser");
    modelFlaschenlabel->setTable("Flaschenlabel");
    modelFlaschenlabelTags->setTable("FlaschenlabelTags");
}

Database::~Database()
{
    QString connectionName = mDb.connectionName();
    disconnect();
    QSqlDatabase::removeDatabase(connectionName);
    delete modelSud;
    delete modelRasten;
    delete modelMalzschuettung;
    delete modelHopfengaben;
    delete modelHefegaben;
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
    delete modelGeraete;
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
            mDb.setDatabaseName(dbPath);
            if (readonly)
                mDb.setConnectOptions("QSQLITE_OPEN_READONLY");
            if (mDb.open())
            {
                QSqlQuery query = sqlExec("SELECT db_Version FROM Global");
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
        modelHefegaben->clear();
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
        modelGeraete->clear();
        modelWasser->clear();
        modelFlaschenlabel->clear();
        modelFlaschenlabelTags->clear();
        mDb.close();
        mVersion = -1;
    }
}

bool Database::isConnected() const
{
    return mDb.isOpen();
}

bool Database::isDirty() const
{
    return modelSud->isDirty() |
           modelRasten->isDirty() |
           modelMalzschuettung->isDirty() |
           modelHopfengaben->isDirty() |
           modelHefegaben->isDirty() |
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
           modelGeraete->isDirty() |
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
    modelGeraete->select();
    modelAusruestung->select();
    modelRasten->select();
    modelMalzschuettung->select();
    modelHopfengaben->select();
    modelHefegaben->select();
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
    modelGeraete->submitAll();
    modelAusruestung->submitAll();
    modelRasten->submitAll();
    modelMalzschuettung->submitAll();
    modelHopfengaben->submitAll();
    modelHefegaben->submitAll();
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
    modelGeraete->revertAll();
    modelRasten->revertAll();
    modelMalzschuettung->revertAll();
    modelHopfengaben->revertAll();
    modelHefegaben->revertAll();
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
    QSqlQuery sqlQuery = mDb.exec(query);
    QSqlError lastError = mDb.lastError();
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
    QSqlQuery query;
    int version = mVersion;
    try
    {
        if (version == 21)
        {
            ++version;
            mDb.transaction();
            sqlExec("ALTER TABLE 'WeitereZutatenGaben' ADD COLUMN 'Zugabedauer' NUMERIC DEFAULT 0");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb.commit();
        }

        if (version == 22)
        {
            ++version;
            mDb.transaction();
            sqlExec("CREATE TABLE IF NOT EXISTS 'Flaschenlabel' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Auswahl' Text, 'BreiteLabel' INTEGER, 'AnzahlLabels' INTEGER, 'AbstandLabels' INTEGER, 'SRandOben' INTEGER, 'SRandLinks' INTEGER, 'SRandRechts' INTEGER, 'SRandUnten' INTEGER)");
            sqlExec("CREATE TABLE IF NOT EXISTS 'FlaschenlabelTags' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Tagname' Text, 'Value' Text)");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb.commit();
        }

        if (version == 23)
        {
            ++version;
            mDb.transaction();
            sqlExec("ALTER TABLE 'Sud' ADD COLUMN 'Sudnummer' NUMERIC DEFAULT 0");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb.commit();
        }

        if (version == 24)
        {
            version = 2000;
            mDb.transaction();

            // Ausruestung & Geraete
            //  - neue Spalte 'Typ'
            //                'KorrekturMenge'
            //  - Spalte gelöscht 'AnlagenID'
            sqlExec("UPDATE Geraete SET AusruestungAnlagenID=(SELECT rowid FROM Ausruestung WHERE AnlagenID=AusruestungAnlagenID)");
            sqlExec("ALTER TABLE Ausruestung RENAME TO TempTable");
            sqlExec("CREATE TABLE Ausruestung ("
                "ID INTEGER PRIMARY KEY,"
                "Name TEXT,"
                "Typ INTEGER DEFAULT 0,"
                "Sudhausausbeute REAL DEFAULT 60,"
                "Verdampfungsziffer REAL DEFAULT 10,"
                "KorrekturWasser REAL DEFAULT 0,"
                "KorrekturFarbe REAL DEFAULT 0,"
                "KorrekturMenge REAL DEFAULT 0,"
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

            // Hefe
            //  - Spalte gelöscht 'Enheiten'
            sqlExec("ALTER TABLE Hefe RENAME TO TempTable");
            sqlExec("CREATE TABLE Hefe ("
                "ID INTEGER PRIMARY KEY,"
                "Beschreibung TEXT,"
                "Menge INTEGER DEFAULT 0,"
                "Preis REAL DEFAULT 0,"
                "Bemerkung TEXT,"
                "TypOGUG INTEGER DEFAULT 0,"
                "TypTrFl INTEGER DEFAULT 0,"
                "Verpackungsmenge TEXT,"
                "Wuerzemenge REAL DEFAULT 0,"
                "Eigenschaften TEXT,"
                "SED INTEGER DEFAULT 0,"
                "EVG TEXT,"
                "Temperatur TEXT,"
                "Eingelagert DATETIME,"
                "Mindesthaltbar DATETIME,"
                "Link TEXT)");
            sqlExec("INSERT INTO Hefe ("
                "Beschreibung,"
                "Menge,"
                "Preis,"
                "Bemerkung,"
                "TypOGUG,"
                "TypTrFl,"
                "Verpackungsmenge,"
                "Wuerzemenge,"
                "Eigenschaften,"
                "SED,"
                "EVG,"
                "Temperatur,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                ") SELECT "
                "Beschreibung,"
                "Menge,"
                "Preis,"
                "Bemerkung,"
                "TypOGUG,"
                "TypTrFl,"
                "Verpackungsmenge,"
                "Wuerzemenge,"
                "Eigenschaften,"
                "SED,"
                "EVG,"
                "Temperatur,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                " FROM TempTable");
            sqlExec("DROP TABLE TempTable");

            // Hopfengaben
            //  - Spalte gelöscht 'Aktiv'
            //                    'erg_Hopfentext'
            sqlExec("ALTER TABLE Hopfengaben RENAME TO TempTable");
            sqlExec("CREATE TABLE Hopfengaben ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Prozent REAL DEFAULT 0,"
                "Zeit INTEGER DEFAULT 0,"
                "erg_Menge REAL DEFAULT 0,"
                "Alpha REAL DEFAULT 0,"
                "Pellets INTEGER DEFAULT 0,"
                "Vorderwuerze INTEGER DEFAULT 0)");
            sqlExec("INSERT INTO Hopfengaben ("
                "SudID,"
                "Name,"
                "Prozent,"
                "Zeit,"
                "erg_Menge,"
                "Alpha,"
                "Pellets,"
                "Vorderwuerze"
                ") SELECT "
                "SudID,"
                "Name,"
                "Prozent,"
                "Zeit,"
                "erg_Menge,"
                "Alpha,"
                "Pellets,"
                "Vorderwuerze"
                " FROM TempTable");
            sqlExec("DROP TABLE TempTable");

            // Hefegaben
            //  - neue Tabelle
            sqlExec("CREATE TABLE Hefegaben ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Menge INTEGER DEFAULT 0,"
                "Zugegeben INTEGER DEFAULT 0,"
                "ZugabeNach INTEGER DEFAULT 0)");
            sqlExec("INSERT INTO Hefegaben ("
                "SudID,"
                "Name,"
                "Menge,"
                "Zugegeben"
                ") SELECT "
                "ID,"
                "AuswahlHefe,"
                "HefeAnzahlEinheiten,"
                "BierWurdeGebraut"
                " FROM Sud WHERE AuswahlHefe IS NOT NULL AND AuswahlHefe <> ''");

            // WeitereZutatenGaben
            //  - Spalte gelöscht 'Zeitpunkt_von'
            //                    'Zeitpunkt_bis'
            //  - neue Spalte 'ZugabeNach'
            sqlExec("ALTER TABLE WeitereZutatenGaben ADD ZugabeNach INTEGER DEFAULT 0");
            query = sqlExec("SELECT ID, SudID, Zeitpunkt_von, Typ FROM WeitereZutatenGaben");
            while (query.next())
            {
                int id = query.value(0).toInt();
                int sudId = query.value(1).toInt();
                QDateTime zeitpunkt = query.value(2).toDateTime();
                int typ = query.value(3).toInt();
                QSqlQuery query2 = sqlExec(QString("SELECT Braudatum FROM Sud WHERE ID=%1").arg(sudId));
                if (query2.first())
                {
                    QDateTime braudatum = query2.value(0).toDateTime();
                    qint64 tage = braudatum.daysTo(zeitpunkt);
                    sqlExec(QString("UPDATE WeitereZutatenGaben SET ZugabeNach=%1 WHERE ID=%2").arg(tage).arg(id));
                }
                if (typ < 0)
                    sqlExec(QString("UPDATE WeitereZutatenGaben SET Typ=%1 WHERE ID=%2").arg(EWZ_Typ_Hopfen).arg(id));
            }
            sqlExec("ALTER TABLE WeitereZutatenGaben RENAME TO TempTable");
            sqlExec("CREATE TABLE WeitereZutatenGaben ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Menge REAL DEFAULT 0,"
                "Einheit INTEGER DEFAULT 0,"
                "Typ INTEGER DEFAULT 0,"
                "Zeitpunkt INTEGER DEFAULT 0,"
                "Bemerkung TEXT,"
                "erg_Menge REAL DEFAULT 0,"
                "Ausbeute REAL DEFAULT 0,"
                "Farbe REAL DEFAULT 0,"
                "ZugabeNach INTEGER DEFAULT 0,"
                "Zugabedauer INTEGER DEFAULT 0,"
                "Entnahmeindex INTEGER DEFAULT 0,"
                "Zugabestatus INTEGER DEFAULT 0)"
                );
            sqlExec("INSERT INTO WeitereZutatenGaben ("
                "SudID,"
                "Name,"
                "Menge,"
                "Einheit,"
                "Typ,"
                "Zeitpunkt,"
                "Bemerkung,"
                "erg_Menge,"
                "Ausbeute,"
                "Farbe,"
                "ZugabeNach,"
                "Zugabedauer,"
                "Entnahmeindex,"
                "Zugabestatus"
                ") SELECT "
                "SudID,"
                "Name,"
                "Menge,"
                "Einheit,"
                "Typ,"
                "Zeitpunkt,"
                "Bemerkung,"
                "erg_Menge,"
                "Ausbeute,"
                "Farbe,"
                "ZugabeNach,"
                "Zugabedauer,"
                "Entnahmeindex,"
                "Zugabestatus"
                " FROM TempTable");
            sqlExec("DROP TABLE TempTable");

            // Rasten
            //  - Spalte gelöscht 'RastAktiv'
            //  - Spalte unbenannt 'RastTemp' -> 'Temp'
            //                     'RastDauer' -> 'Dauer'
            sqlExec("ALTER TABLE Rasten RENAME TO TempTable");
            sqlExec("CREATE TABLE Rasten ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Temp REAL DEFAULT 0,"
                "Dauer REAL DEFAULT 0)");
            sqlExec("INSERT INTO Rasten ("
                "SudID,"
                "Name,"
                "Temp,"
                "Dauer"
                ") SELECT "
                "SudID,"
                "RastName,"
                "RastTemp,"
                "RastDauer"
                " FROM TempTable");
            sqlExec("DROP TABLE TempTable");

            // Sud
            //  - neue Spalte 'Wasserprofil'
            //  - Spalte gelöscht 'BierWurdeGebraut'
            //                    'BierWurdeAbgefuellt'
            //                    'BierWurdeVerbraucht'
            //                    'Anstelldatum'
            //                    'AktivTab'
            //                    'Bewertung'
            //                    'BewertungText'
            //                    'AktivTab_Gaerverlauf'
            //                    'SWVorHopfenseihen'
            //                    'BewertungMaxSterne'
            //                    'NeuBerechnen'
            //                    'AuswahlBrauanlage'
            //                    'AuswahlHefe'
            //                    'HefeAnzahlEinheiten'
            //  - neue Spalte 'Status'
            //  - Spalte unbenannt 'erg_S_Gesammt' -> 'erg_S_Gesamt'
            //                     'erg_W_Gesammt' -> 'erg_W_Gesamt'
            //                     'AuswahlBrauanlageName' -> 'Anlage'
            sqlExec("ALTER TABLE Sud ADD Status INTEGER DEFAULT 0");
            query = sqlExec("SELECT ID, BierWurdeGebraut, BierWurdeAbgefuellt, BierWurdeVerbraucht FROM Sud");
            while (query.next())
            {
                int id = query.value(0).toInt();
                bool gebraut = query.value(1).toBool();
                bool abgefuelllt = query.value(2).toBool();
                bool verbraucht = query.value(3).toBool();
                int status = Sud_Status_Rezept;
                if (gebraut)
                {
                    status = Sud_Status_Gebraut;
                    if (abgefuelllt)
                    {
                        status = Sud_Status_Abgefuellt;
                        if (verbraucht)
                        {
                            status = Sud_Status_Verbraucht;
                        }
                    }
                }
                sqlExec(QString("UPDATE Sud SET Status=%1 WHERE ID=%2").arg(status).arg(id));
            }
            sqlExec("ALTER TABLE Sud RENAME TO TempTable");
            sqlExec("CREATE TABLE Sud ("
                "ID INTEGER PRIMARY KEY,"
                "Sudname TEXT,"
                "Sudnummer INTEGER DEFAULT 0,"
                "Anlage TEXT,"
                "Menge REAL DEFAULT 20,"
                "SW REAL DEFAULT 12,"
                "highGravityFaktor REAL DEFAULT 0,"
                "FaktorHauptguss REAL DEFAULT 3.5,"
                "Wasserprofil TEXT,"
                "RestalkalitaetSoll REAL DEFAULT 0,"
                "EinmaischenTemp INTEGER DEFAULT 60,"
                "CO2 REAL DEFAULT 5,"
                "IBU REAL DEFAULT 26,"
                "berechnungsArtHopfen INTEGER DEFAULT 0,"
                "KochdauerNachBitterhopfung INTEGER DEFAULT 90,"
                "Nachisomerisierungszeit INTEGER DEFAULT 0,"
                "Reifezeit INTEGER DEFAULT 4,"
                "KostenWasserStrom REAL DEFAULT 0,"
                "Kommentar TEXT,"
                "Status INTEGER DEFAULT 0,"
                "Braudatum DATETIME,"
                "Abfuelldatum DATETIME,"
                "Erstellt DATETIME,"
                "Gespeichert DATETIME,"
                "erg_S_Gesamt REAL DEFAULT 0,"
                "erg_W_Gesamt REAL DEFAULT 0,"
                "erg_WHauptguss REAL DEFAULT 0,"
                "erg_WNachguss REAL DEFAULT 0,"
                "erg_Farbe REAL DEFAULT 0,"
                "SWKochende REAL DEFAULT 12,"
                "SWAnstellen REAL DEFAULT 12,"
                "SchnellgaerprobeAktiv INTEGER DEFAULT 0,"
                "SWSchnellgaerprobe REAL DEFAULT 2.5,"
                "SWJungbier REAL DEFAULT 3,"
                "TemperaturJungbier REAL DEFAULT 12,"
                "WuerzemengeVorHopfenseihen REAL DEFAULT 0,"
                "WuerzemengeKochende REAL DEFAULT 20,"
                "WuerzemengeAnstellen REAL DEFAULT 20,"
                "Spunden INTEGER DEFAULT 0,"
                "Speisemenge REAL DEFAULT 1,"
                "JungbiermengeAbfuellen REAL DEFAULT 0,"
                "erg_AbgefuellteBiermenge REAL DEFAULT 0,"
                "erg_Sudhausausbeute REAL DEFAULT 0,"
                "erg_EffektiveAusbeute REAL DEFAULT 0,"
                "erg_Preis REAL DEFAULT 0,"
                "erg_Alkohol REAL DEFAULT 0,"
                "AusbeuteIgnorieren INTEGER DEFAULT 0,"
                "MerklistenID INTEGER DEFAULT 0)");
            sqlExec("INSERT INTO Sud ("
                "ID,"
                "Sudname,"
                "Sudnummer,"
                "Anlage,"
                "Menge,"
                "SW,"
                "highGravityFaktor,"
                "FaktorHauptguss,"
                "RestalkalitaetSoll,"
                "EinmaischenTemp,"
                "CO2,"
                "IBU,"
                "berechnungsArtHopfen,"
                "KochdauerNachBitterhopfung,"
                "Nachisomerisierungszeit,"
                "Reifezeit,"
                "KostenWasserStrom,"
                "Kommentar,"
                "Status,"
                "Braudatum,"
                "Abfuelldatum,"
                "Erstellt,"
                "Gespeichert,"
                "erg_S_Gesamt,"
                "erg_W_Gesamt,"
                "erg_WHauptguss,"
                "erg_WNachguss,"
                "erg_Farbe,"
                "SWKochende,"
                "SWAnstellen,"
                "SchnellgaerprobeAktiv,"
                "SWSchnellgaerprobe,"
                "SWJungbier,"
                "TemperaturJungbier,"
                "WuerzemengeVorHopfenseihen,"
                "WuerzemengeKochende,"
                "WuerzemengeAnstellen,"
                "Spunden,"
                "Speisemenge,"
                "JungbiermengeAbfuellen,"
                "erg_AbgefuellteBiermenge,"
                "erg_Sudhausausbeute,"
                "erg_EffektiveAusbeute,"
                "erg_Preis,"
                "erg_Alkohol,"
                "AusbeuteIgnorieren,"
                "MerklistenID"
                ") SELECT "
                "ID,"
                "Sudname,"
                "Sudnummer,"
                "AuswahlBrauanlageName,"
                "Menge,"
                "SW,"
                "highGravityFaktor,"
                "FaktorHauptguss,"
                "RestalkalitaetSoll,"
                "EinmaischenTemp,"
                "CO2,"
                "IBU,"
                "berechnungsArtHopfen,"
                "KochdauerNachBitterhopfung,"
                "Nachisomerisierungszeit,"
                "Reifezeit,"
                "KostenWasserStrom,"
                "Kommentar,"
                "Status,"
                "Braudatum,"
                "Abfuelldatum,"
                "Erstellt,"
                "Gespeichert,"
                "erg_S_Gesammt,"
                "erg_W_Gesammt,"
                "erg_WHauptguss,"
                "erg_WNachguss,"
                "erg_Farbe,"
                "SWKochende,"
                "SWAnstellen,"
                "SchnellgaerprobeAktiv,"
                "SWSchnellgaerprobe,"
                "SWJungbier,"
                "TemperaturJungbier,"
                "WuerzemengeVorHopfenseihen,"
                "WuerzemengeKochende,"
                "WuerzemengeAnstellen,"
                "Spunden,"
                "Speisemenge,"
                "JungbiermengeAbfuellen,"
                "erg_AbgefuellteBiermenge,"
                "erg_Sudhausausbeute,"
                "erg_EffektiveAusbeute,"
                "erg_Preis,"
                "erg_Alkohol,"
                "AusbeuteIgnorieren,"
                "MerklistenID"
                " FROM TempTable");
            sqlExec("UPDATE Sud SET Wasserprofil='Profil 1'");
            sqlExec("DROP TABLE TempTable");

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

            mDb.commit();
        }
    }
    catch (const std::exception& ex)
    {
        mDb.rollback();
        throw ex;
    }
    catch (...)
    {
        mDb.rollback();
        throw std::runtime_error("unknown error");
    }
}
