#include "database.h"
#include "database_defs.h"
#include <QSqlQuery>

bool Database::update()
{
    QSqlQuery query;
    QSqlDatabase db = QSqlDatabase::database("kbh", false);
    if (!db.isValid())
    {
        qCritical("Database connection is invalid.");
        return false;
    }
    int version = mVersion;
    try
    {
        if (version == 21)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();
            sqlExec(db, "ALTER TABLE 'WeitereZutatenGaben' ADD COLUMN 'Zugabedauer' NUMERIC DEFAULT 0");
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        if (version == 22)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();
            sqlExec(db, "CREATE TABLE IF NOT EXISTS 'Flaschenlabel' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Auswahl' Text, 'BreiteLabel' INTEGER, 'AnzahlLabels' INTEGER, 'AbstandLabels' INTEGER, 'SRandOben' INTEGER, 'SRandLinks' INTEGER, 'SRandRechts' INTEGER, 'SRandUnten' INTEGER)");
            sqlExec(db, "CREATE TABLE IF NOT EXISTS 'FlaschenlabelTags' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Tagname' Text, 'Value' Text)");
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        if (version == 23)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();
            sqlExec(db, "ALTER TABLE 'Sud' ADD COLUMN 'Sudnummer' NUMERIC DEFAULT 0");
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        if (version == 24)
        {
            version = 2000;
            qInfo() << "Updating to version:" << version;
            db.transaction();

            // Ausruestung & Geraete
            //  - neue Spalte 'Typ'
            //                'KorrekturMenge'
            //  - Spalte gelöscht 'AnlagenID'
            sqlExec(db, "UPDATE Geraete SET AusruestungAnlagenID=(SELECT rowid FROM Ausruestung WHERE AnlagenID=AusruestungAnlagenID)");
            sqlExec(db, "ALTER TABLE Ausruestung RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Ausruestung ("
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
            sqlExec(db, "INSERT INTO Ausruestung ("
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
            sqlExec(db, "DROP TABLE TempTable");

            // Wasser
            //  - neue Spalte 'Name'
            sqlExec(db, "ALTER TABLE Wasser RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Wasser ("
                "ID INTEGER PRIMARY KEY,"
                "Name TEXT,"
                "Calcium REAL DEFAULT 0,"
                "Magnesium REAL DEFAULT 0,"
                "Saeurekapazitaet REAL DEFAULT 0)");
            sqlExec(db, "INSERT INTO Wasser ("
                "Calcium,"
                "Magnesium,"
                "Saeurekapazitaet"
                ") SELECT "
                "Calcium,"
                "Magnesium,"
                "Saeurekapazitaet"
                " FROM TempTable");
            sqlExec(db, "UPDATE Wasser SET Name='Profil 1'");
            sqlExec(db, "DROP TABLE TempTable");

            // Hefe
            //  - Spalte gelöscht 'Enheiten'
            sqlExec(db, "ALTER TABLE Hefe RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Hefe ("
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
            sqlExec(db, "INSERT INTO Hefe ("
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
            sqlExec(db, "DROP TABLE TempTable");

            // Hopfengaben
            //  - Spalte gelöscht 'Aktiv'
            //                    'erg_Hopfentext'
            sqlExec(db, "ALTER TABLE Hopfengaben RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Hopfengaben ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Prozent REAL DEFAULT 0,"
                "Zeit INTEGER DEFAULT 0,"
                "erg_Menge REAL DEFAULT 0,"
                "Alpha REAL DEFAULT 0,"
                "Pellets INTEGER DEFAULT 0,"
                "Vorderwuerze INTEGER DEFAULT 0)");
            sqlExec(db, "INSERT INTO Hopfengaben ("
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
            sqlExec(db, "DROP TABLE TempTable");

            // Hefegaben
            //  - neue Tabelle
            sqlExec(db, "CREATE TABLE Hefegaben ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Menge INTEGER DEFAULT 0,"
                "Zugegeben INTEGER DEFAULT 0,"
                "ZugabeNach INTEGER DEFAULT 0)");
            sqlExec(db, "INSERT INTO Hefegaben ("
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
            sqlExec(db, "ALTER TABLE WeitereZutatenGaben ADD ZugabeNach INTEGER DEFAULT 0");
            query = sqlExec(db, "SELECT ID, SudID, Zeitpunkt_von, Typ FROM WeitereZutatenGaben");
            while (query.next())
            {
                int id = query.value(0).toInt();
                int sudId = query.value(1).toInt();
                QDateTime zeitpunkt = query.value(2).toDateTime();
                int typ = query.value(3).toInt();
                QSqlQuery query2 = sqlExec(db, QString("SELECT Braudatum FROM Sud WHERE ID=%1").arg(sudId));
                if (query2.first())
                {
                    QDateTime braudatum = query2.value(0).toDateTime();
                    qint64 tage = braudatum.daysTo(zeitpunkt);
                    sqlExec(db, QString("UPDATE WeitereZutatenGaben SET ZugabeNach=%1 WHERE ID=%2").arg(tage).arg(id));
                }
                if (typ < 0)
                    sqlExec(db, QString("UPDATE WeitereZutatenGaben SET Typ=%1 WHERE ID=%2").arg(EWZ_Typ_Hopfen).arg(id));
            }
            sqlExec(db, "ALTER TABLE WeitereZutatenGaben RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE WeitereZutatenGaben ("
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
            sqlExec(db, "INSERT INTO WeitereZutatenGaben ("
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
            sqlExec(db, "DROP TABLE TempTable");

            // Rasten
            //  - Spalte gelöscht 'RastAktiv'
            //  - Spalte unbenannt 'RastTemp' -> 'Temp'
            //                     'RastDauer' -> 'Dauer'
            sqlExec(db, "ALTER TABLE Rasten RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Rasten ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER,"
                "Name TEXT,"
                "Temp REAL DEFAULT 0,"
                "Dauer REAL DEFAULT 0)");
            sqlExec(db, "INSERT INTO Rasten ("
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
            sqlExec(db, "DROP TABLE TempTable");

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
            sqlExec(db, "ALTER TABLE Sud ADD Status INTEGER DEFAULT 0");
            query = sqlExec(db, "SELECT ID, BierWurdeGebraut, BierWurdeAbgefuellt, BierWurdeVerbraucht FROM Sud");
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
                sqlExec(db, QString("UPDATE Sud SET Status=%1 WHERE ID=%2").arg(status).arg(id));
            }
            sqlExec(db, "ALTER TABLE Sud RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Sud ("
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
            sqlExec(db, "INSERT INTO Sud ("
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
            sqlExec(db, "UPDATE Sud SET Wasserprofil='Profil 1'");
            sqlExec(db, "DROP TABLE TempTable");

            // IgnorMsgID
            //  - Tabelle gelöscht
            sqlExec(db, "DROP TABLE IgnorMsgID");

            // Rastauswahl
            //  - Tabelle gelöscht
            sqlExec(db, "DROP TABLE Rastauswahl");

            // Global
            //  - Spalte gelöscht 'db_NeuBerechnen'
            sqlExec(db, "DROP TABLE Global");
            sqlExec(db, "CREATE TABLE Global (db_Version INTEGER)");
            sqlExec(db, QString("INSERT INTO Global (db_Version) VALUES (%1)").arg(version));

            db.commit();
        }

        if (version == 2000)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();

            // Malz
            //  - NUMERIC -> REAL
            sqlExec(db, "ALTER TABLE Malz RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Malz ("
                "ID INTEGER PRIMARY KEY,"
                "Beschreibung TEXT NOT NULL UNIQUE,"
                "Farbe REAL DEFAULT 0,"
                "MaxProzent REAL DEFAULT 100,"
                "Menge REAL DEFAULT 0,"
                "Preis REAL DEFAULT 0,"
                "Bemerkung TEXT,"
                "Anwendung TEXT,"
                "Eingelagert DATETIME,"
                "Mindesthaltbar DATETIME,"
                "Link TEXT)");
            sqlExec(db, "INSERT INTO Malz ("
                "Beschreibung,"
                "Farbe,"
                "MaxProzent,"
                "Menge,"
                "Preis,"
                "Bemerkung,"
                "Anwendung,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                ") SELECT "
                "Beschreibung,"
                "Farbe,"
                "MaxProzent,"
                "Menge,"
                "Preis,"
                "Bemerkung,"
                "Anwendung,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Hopfen
            //  - NUMERIC -> REAL
            sqlExec(db, "ALTER TABLE Hopfen RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Hopfen ("
                "ID INTEGER PRIMARY KEY,"
                "Beschreibung TEXT NOT NULL UNIQUE,"
                "Alpha REAL DEFAULT 0,"
                "Menge REAL DEFAULT 0,"
                "Preis REAL DEFAULT 0,"
                "Pellets INTEGER DEFAULT 1,"
                "Bemerkung TEXT,"
                "Eigenschaften TEXT,"
                "Typ INTEGER DEFAULT 0,"
                "Eingelagert DATETIME,"
                "Mindesthaltbar DATETIME,"
                "Link TEXT)");
            sqlExec(db, "INSERT INTO Hopfen ("
                "Beschreibung,"
                "Alpha,"
                "Menge,"
                "Preis,"
                "Pellets,"
                "Bemerkung,"
                "Eigenschaften,"
                "Typ,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                ") SELECT "
                "Beschreibung,"
                "Alpha,"
                "Menge,"
                "Preis,"
                "Pellets,"
                "Bemerkung,"
                "Eigenschaften,"
                "Typ,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Hefe
            //  - NUMERIC -> REAL
            sqlExec(db, "ALTER TABLE Hefe RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Hefe ("
                "ID INTEGER PRIMARY KEY,"
                "Beschreibung TEXT NOT NULL UNIQUE,"
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
            sqlExec(db, "INSERT INTO Hefe ("
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
            sqlExec(db, "DROP TABLE TempTable");

            // WeitereZutaten
            //  - NUMERIC -> REAL
            sqlExec(db, "ALTER TABLE WeitereZutaten RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE WeitereZutaten ("
                "ID INTEGER PRIMARY KEY,"
                "Beschreibung TEXT NOT NULL UNIQUE,"
                "Menge REAL DEFAULT 0,"
                "Einheiten INTEGER DEFAULT 0,"
                "Typ INTEGER DEFAULT 0,"
                "Ausbeute REAL DEFAULT 0,"
                "EBC REAL DEFAULT 0,"
                "Preis REAL DEFAULT 0,"
                "Bemerkung TEXT,"
                "Eingelagert DATETIME,"
                "Mindesthaltbar DATETIME,"
                "Link TEXT)");
            sqlExec(db, "INSERT INTO WeitereZutaten ("
                "Beschreibung ,"
                "Menge,"
                "Einheiten,"
                "Typ,"
                "Ausbeute,"
                "EBC,"
                "Preis,"
                "Bemerkung,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                ") SELECT "
                "Beschreibung ,"
                "Menge,"
                "Einheiten,"
                "Typ,"
                "Ausbeute,"
                "EBC,"
                "Preis,"
                "Bemerkung,"
                "Eingelagert,"
                "Mindesthaltbar,"
                "Link"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Schnellgaerverlauf
            //  - NUMERIC -> REAL
            //  - Spalte unbenannt SW -> Restextrakt
            //  - Neue Spalte Bemerkung
            sqlExec(db, "ALTER TABLE Schnellgaerverlauf RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Schnellgaerverlauf ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER NOT NULL,"
                "Zeitstempel DATETIME,"
                "Restextrakt REAL DEFAULT 0,"
                "Alc REAL DEFAULT 0,"
                "Temp REAL DEFAULT 18,"
                "Bemerkung TEXT)");
            sqlExec(db, "INSERT INTO Schnellgaerverlauf ("
                "SudID,"
                "Zeitstempel,"
                "Restextrakt,"
                "Alc,"
                "Temp"
                ") SELECT "
                "SudID,"
                "Zeitstempel,"
                "SW,"
                "Alc,"
                "Temp"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Hauptgaerverlauf
            //  - NUMERIC -> REAL
            //  - Spalte unbenannt SW -> Restextrakt
            //  - Neue Spalte Bemerkung
            sqlExec(db, "ALTER TABLE Hauptgaerverlauf RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Hauptgaerverlauf ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER NOT NULL,"
                "Zeitstempel DATETIME,"
                "Restextrakt REAL DEFAULT 0,"
                "Alc REAL DEFAULT 0,"
                "Temp REAL DEFAULT 18,"
                "Bemerkung TEXT)");
            sqlExec(db, "INSERT INTO Hauptgaerverlauf ("
                "SudID,"
                "Zeitstempel,"
                "Restextrakt,"
                "Alc,"
                "Temp"
                ") SELECT "
                "SudID,"
                "Zeitstempel,"
                "SW,"
                "Alc,"
                "Temp"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Nachgaerverlauf
            //  - NUMERIC -> REAL
            //  - Neue Spalte Bemerkung
            sqlExec(db, "ALTER TABLE Nachgaerverlauf RENAME TO TempTable");
            sqlExec(db, "CREATE TABLE Nachgaerverlauf ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER NOT NULL,"
                "Zeitstempel DATETIME,"
                "Druck REAL DEFAULT 0,"
                "Temp REAL DEFAULT 18,"
                "CO2 REAL DEFAULT 0,"
                "Bemerkung TEXT)");
            sqlExec(db, "INSERT INTO Nachgaerverlauf ("
                "SudID,"
                "Zeitstempel,"
                "Druck,"
                "Temp,"
                "CO2"
                ") SELECT "
                "SudID,"
                "Zeitstempel,"
                "Druck,"
                "Temp,"
                "CO2"
                " FROM TempTable");
            sqlExec(db, "DROP TABLE TempTable");

            // Flaschenlabel -> Etiketten
            //  - Tabelle unbenannt Flaschenlabel -> Etiketten
            //  - Spalten komplett überarbeitet
            sqlExec(db, "CREATE TABLE Etiketten ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER NOT NULL,"
                "Pfad TEXT NOT NULL,"
                "Anzahl INTEGER DEFAULT 20,"
                "Breite INTEGER DEFAULT 100,"
                "Hoehe INTEGER DEFAULT 60,"
                "AbstandHor INTEGER DEFAULT 2,"
                "AbstandVert INTEGER DEFAULT 2,"
                "RandOben INTEGER DEFAULT 10,"
                "RandLinks INTEGER DEFAULT 5,"
                "RandRechts INTEGER DEFAULT 5,"
                "RandUnten INTEGER DEFAULT 15)");
            sqlExec(db, "INSERT INTO Etiketten ("
                "SudID,"
                "Pfad,"
                "Anzahl,"
                "Breite,"
                "AbstandHor,"
                "AbstandVert,"
                "RandOben,"
                "RandLinks,"
                "RandRechts,"
                "RandUnten"
                ") SELECT "
                "SudID,"
                "Auswahl,"
                "AnzahlLabels,"
                "BreiteLabel,"
                "AbstandLabels,"
                "AbstandLabels,"
                "SRandOben,"
                "SRandLinks,"
                "SRandRechts,"
                "SRandUnten"
                " FROM Flaschenlabel");
            sqlExec(db, "DROP TABLE Flaschenlabel");

            // FlaschenlabelTags -> Tags
            //  - Tabelle unbenannt FlaschenlabelTags -> Tags
            //  - Spalte unbenannt Tagname -> Key
            sqlExec(db, "CREATE TABLE Tags ("
                "ID INTEGER PRIMARY KEY,"
                "SudID INTEGER NOT NULL,"
                "Key TEXT NOT NULL,"
                "Value TEXT)");
            sqlExec(db, "INSERT INTO Tags ("
                "SudID,"
                "Key,"
                "Value"
                ") SELECT "
                "SudID,"
                "Tagname,"
                "Value"
                " FROM FlaschenlabelTags");
            sqlExec(db, "DROP TABLE FlaschenlabelTags");

            // Global
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        if (version == 2001)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();

            // Sud
            //  - neue Spalte 'Sudhausausbeute'
            //  - neue Spalte 'Verdampfungsrate'
            //  - neue Spalte 'Vergaerungsgrad'
            sqlExec(db, "ALTER TABLE Sud ADD COLUMN Sudhausausbeute REAL DEFAULT 60");
            sqlExec(db, "ALTER TABLE Sud ADD COLUMN Verdampfungsrate REAL DEFAULT 10");
            sqlExec(db, "ALTER TABLE Sud ADD COLUMN Vergaerungsgrad REAL DEFAULT 70");
            sqlExec(db, "UPDATE Sud SET Sudhausausbeute = (SELECT Sudhausausbeute FROM Ausruestung WHERE Name = Anlage)");
            sqlExec(db, "UPDATE Sud SET Verdampfungsrate = (SELECT Verdampfungsziffer FROM Ausruestung WHERE Name = Anlage)");

            // Global
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        if (version == 2002)
        {
            ++version;
            qInfo() << "Updating to version:" << version;
            db.transaction();

            // Sud
            //  - neue Spalte 'WuerzemengeKochbeginn'
            //  - neue Spalte 'SWKochbeginn'
            sqlExec(db, "ALTER TABLE Sud ADD COLUMN WuerzemengeKochbeginn REAL DEFAULT 0");
            sqlExec(db, "ALTER TABLE Sud ADD COLUMN SWKochbeginn REAL DEFAULT 0");

            // Global
            sqlExec(db, QString("UPDATE Global SET db_Version=%1").arg(version));
            db.commit();
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        db.rollback();
        qCritical() << ex.what();
        return false;
    }
    catch (...)
    {
        db.rollback();
        qCritical("unknown error");
        return false;
    }
}
