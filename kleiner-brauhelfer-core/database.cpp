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
    modelRastauswahl = new SqlTableModel(bh, *mDb);
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
    modelGeraete = new SqlTableModel(bh, *mDb);
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
    modelRastauswahl->setTable("Rastauswahl");
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
    modelGeraete->setTable("Geraete");
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
    delete modelRastauswahl;
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
        modelRastauswahl->clear();
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
        modelGeraete->clear();
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
           modelRastauswahl->isDirty() |
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
    modelRastauswahl->select();
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
    modelGeraete->submitAll();
    modelAusruestung->submitAll();
    modelRastauswahl->submitAll();
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
    modelGeraete->revertAll();
    modelRastauswahl->revertAll();
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
        if (version == 21)
        {
            ++version;
            mDb->transaction();
            sqlExec("ALTER TABLE 'WeitereZutatenGaben' ADD COLUMN 'Zugabedauer' NUMERIC DEFAULT 0");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb->commit();
        }

        if (version == 22)
        {
            ++version;
            mDb->transaction();
            sqlExec("CREATE TABLE IF NOT EXISTS 'Flaschenlabel' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Auswahl' Text, 'BreiteLabel' INTEGER, 'AnzahlLabels' INTEGER, 'AbstandLabels' INTEGER, 'SRandOben' INTEGER, 'SRandLinks' INTEGER, 'SRandRechts' INTEGER, 'SRandUnten' INTEGER)");
            sqlExec("CREATE TABLE IF NOT EXISTS 'FlaschenlabelTags' ('ID' INTEGER PRIMARY KEY  NOT NULL ,'SudID' INTEGER, 'Tagname' Text, 'Value' Text)");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb->commit();
        }

        if (version == 23)
        {
            ++version;
            mDb->transaction();
            sqlExec("ALTER TABLE 'Sud' ADD COLUMN 'Sudnummer' NUMERIC DEFAULT 0");
            sqlExec(QString("UPDATE Global SET db_Version=%1").arg(version));
            mDb->commit();
        }
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
