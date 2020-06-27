#include "database.h"
#include <stdexcept>
#include <QFile>
#include <QSqlQuery>
#include "brauhelfer.h"

Database::Database() :
    mVersion(-1),
    mLastError(QString())
{
    QSqlDatabase::addDatabase("QSQLITE", "kbh");
}

void Database::createTables(Brauhelfer* bh)
{
    QSqlDatabase db = QSqlDatabase::database("kbh", false);
    if (!db.isValid())
        qCritical("Database connection is invalid.");
    modelSud = new ModelSud(bh, db);
    modelRasten = new ModelRasten(bh, db);
    modelMalzschuettung = new ModelMalzschuettung(bh, db);
    modelHopfengaben = new ModelHopfengaben(bh, db);
    modelHefegaben = new ModelHefegaben(bh, db);
    modelWeitereZutatenGaben = new ModelWeitereZutatenGaben(bh, db);
    modelSchnellgaerverlauf = new ModelSchnellgaerverlauf(bh, db);
    modelHauptgaerverlauf = new ModelHauptgaerverlauf(bh, db);
    modelNachgaerverlauf = new ModelNachgaerverlauf(bh, db);
    modelBewertungen = new ModelBewertungen(bh, db);
    modelMalz = new ModelMalz(bh, db);
    modelHopfen = new ModelHopfen(bh, db);
    modelHefe = new ModelHefe(bh, db);
    modelWeitereZutaten = new ModelWeitereZutaten(bh, db);
    modelAnhang = new ModelAnhang(bh, db);
    modelAusruestung = new ModelAusruestung(bh, db);
    modelGeraete = new ModelGeraete(bh, db);
    modelWasser = new ModelWasser(bh, db);
    modelEtiketten = new ModelEtiketten(bh, db);
    modeTags = new ModelTags(bh, db);
    modelKategorien = new ModelKategorien(bh, db);
    modelWasseraufbereitung = new ModelWasseraufbereitung(bh, db);
    modelSud->createConnections();
}

void Database::setTables()
{
    modelSud->setTable("Sud");
    modelSud->setSort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    modelRasten->setTable("Rasten");
    modelMalzschuettung->setTable("Malzschuettung");
    modelHopfengaben->setTable("Hopfengaben");
    modelHefegaben->setTable("Hefegaben");
    modelWeitereZutatenGaben->setTable("WeitereZutatenGaben");
    modelSchnellgaerverlauf->setTable("Schnellgaerverlauf");
    modelSchnellgaerverlauf->setSort(ModelSchnellgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelHauptgaerverlauf->setTable("Hauptgaerverlauf");
    modelHauptgaerverlauf->setSort(ModelHauptgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelNachgaerverlauf->setTable("Nachgaerverlauf");
    modelNachgaerverlauf->setSort(ModelNachgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelBewertungen->setTable("Bewertungen");
    modelBewertungen->setSort(ModelBewertungen::ColDatum, Qt::AscendingOrder);
    modelMalz->setTable("Malz");
    modelHopfen->setTable("Hopfen");
    modelHefe->setTable("Hefe");
    modelWeitereZutaten->setTable("WeitereZutaten");
    modelAnhang->setTable("Anhang");
    modelAusruestung->setTable("Ausruestung");
    modelGeraete->setTable("Geraete");
    modelWasser->setTable("Wasser");
    modelEtiketten->setTable("Etiketten");
    modeTags->setTable("Tags");
    modelKategorien->setTable("Kategorien");
    modelWasseraufbereitung->setTable("Wasseraufbereitung");

    // sanity check
    Q_ASSERT(modelSud->columnCount() == ModelSud::NumCols);
    Q_ASSERT(modelRasten->columnCount() == ModelRasten::NumCols);
    Q_ASSERT(modelMalzschuettung->columnCount() == ModelMalzschuettung::NumCols);
    Q_ASSERT(modelHopfengaben->columnCount() == ModelHopfengaben::NumCols);
    Q_ASSERT(modelHefegaben->columnCount() == ModelHefegaben::NumCols);
    Q_ASSERT(modelWeitereZutatenGaben->columnCount() == ModelWeitereZutatenGaben::NumCols);
    Q_ASSERT(modelSchnellgaerverlauf->columnCount() == ModelSchnellgaerverlauf::NumCols);
    Q_ASSERT(modelHauptgaerverlauf->columnCount() == ModelHauptgaerverlauf::NumCols);
    Q_ASSERT(modelNachgaerverlauf->columnCount() == ModelNachgaerverlauf::NumCols);
    Q_ASSERT(modelBewertungen->columnCount() == ModelBewertungen::NumCols);
    Q_ASSERT(modelMalz->columnCount() == ModelMalz::NumCols);
    Q_ASSERT(modelHopfen->columnCount() == ModelHopfen::NumCols);
    Q_ASSERT(modelHefe->columnCount() == ModelHefe::NumCols);
    Q_ASSERT(modelWeitereZutaten->columnCount() == ModelWeitereZutaten::NumCols);
    Q_ASSERT(modelAnhang->columnCount() == ModelAnhang::NumCols);
    Q_ASSERT(modelAusruestung->columnCount() == ModelAusruestung::NumCols);
    Q_ASSERT(modelGeraete->columnCount() == ModelGeraete::NumCols);
    Q_ASSERT(modelWasser->columnCount() == ModelWasser::NumCols);
    Q_ASSERT(modelEtiketten->columnCount() == ModelEtiketten::NumCols);
    Q_ASSERT(modeTags->columnCount() == ModelTags::NumCols);
    Q_ASSERT(modelKategorien->columnCount() == ModelKategorien::NumCols);
    Q_ASSERT(modelWasseraufbereitung->columnCount() == ModelWasseraufbereitung::NumCols);
}

Database::~Database()
{
    disconnect();
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
    delete modelEtiketten;
    delete modeTags;
    delete modelKategorien;
    delete modelWasseraufbereitung;
    QSqlDatabase::removeDatabase("kbh");
}

bool Database::connect(const QString &dbPath, bool readonly)
{
    if (!isConnected())
    {
        if (QFile::exists(dbPath))
        {
            QSqlDatabase db = QSqlDatabase::database("kbh", false);
            if (!db.isValid())
            {
                qCritical("Database connection is invalid.");
                return false;
            }
            db.close();
            db.setDatabaseName(dbPath);
            db.open();
            if (readonly)
                db.setConnectOptions("QSQLITE_OPEN_READONLY");
            if (db.open())
            {
                try
                {
                    QSqlQuery query = sqlExec(db, "SELECT db_Version FROM Global");
                    if (query.first())
                    {
                        int version = query.value(0).toInt();
                        if (version > 0)
                        {
                            mVersion = version;
                            return true;
                        }
                    }
                }
                catch (...)
                {
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
        modelEtiketten->clear();
        modeTags->clear();
        modelKategorien->clear();
        modelWasseraufbereitung->clear();
        mVersion = -1;
    }
}

bool Database::isConnected() const
{
    return mVersion != -1;
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
           modelEtiketten->isDirty() |
           modeTags->isDirty() |
           modelKategorien->isDirty() |
           modelWasseraufbereitung->isDirty();
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
    modelEtiketten->select();
    modeTags->select();
    modelKategorien->select();
    modelWasseraufbereitung->select();
    modelSud->select();
}

int Database::version() const
{
    return mVersion;
}

bool Database::save()
{
    bool ret = true;
    if (!modelMalz->submitAll())
    {
        mLastError = modelMalz->lastError();
        ret = false;
    }
    if (!modelHopfen->submitAll())
    {
        mLastError = modelHopfen->lastError();
        ret = false;
    }
    if (!modelHefe->submitAll())
    {
        mLastError = modelHefe->lastError();
        ret = false;
    }
    if (!modelWeitereZutaten->submitAll())
    {
        mLastError = modelWeitereZutaten->lastError();
        ret = false;
    }
    if (!modelWasser->submitAll())
    {
        mLastError = modelWasser->lastError();
        ret = false;
    }
    if (!modelGeraete->submitAll())
    {
        mLastError = modelGeraete->lastError();
        ret = false;
    }
    if (!modelAusruestung->submitAll())
    {
        mLastError = modelAusruestung->lastError();
        ret = false;
    }
    if (!modelRasten->submitAll())
    {
        mLastError = modelRasten->lastError();
        ret = false;
    }
    if (!modelMalzschuettung->submitAll())
    {
        mLastError = modelMalzschuettung->lastError();
        ret = false;
    }
    if (!modelHopfengaben->submitAll())
    {
        mLastError = modelHopfengaben->lastError();
        ret = false;
    }
    if (!modelHefegaben->submitAll())
    {
        mLastError = modelHefegaben->lastError();
        ret = false;
    }
    if (!modelWeitereZutatenGaben->submitAll())
    {
        mLastError = modelWeitereZutatenGaben->lastError();
        ret = false;
    }
    if (!modelSchnellgaerverlauf->submitAll())
    {
        mLastError = modelSchnellgaerverlauf->lastError();
        ret = false;
    }
    if (!modelHauptgaerverlauf->submitAll())
    {
        mLastError = modelHauptgaerverlauf->lastError();
        ret = false;
    }
    if (!modelNachgaerverlauf->submitAll())
    {
        mLastError = modelNachgaerverlauf->lastError();
        ret = false;
    }
    if (!modelBewertungen->submitAll())
    {
        mLastError = modelBewertungen->lastError();
        ret = false;
    }
    if (!modelAnhang->submitAll())
    {
        mLastError = modelAnhang->lastError();
        ret = false;
    }
    if (!modelEtiketten->submitAll())
    {
        mLastError = modelEtiketten->lastError();
        ret = false;
    }
    if (!modeTags->submitAll())
    {
        mLastError = modeTags->lastError();
        ret = false;
    }
    if (!modelKategorien->submitAll())
    {
        mLastError = modelKategorien->lastError();
        ret = false;
    }
    if (!modelWasseraufbereitung->submitAll())
    {
        mLastError = modelWasseraufbereitung->lastError();
        ret = false;
    }
    if (!modelSud->submitAll())
    {
        mLastError = modelSud->lastError();
        ret = false;
    }
    return ret;
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
    modelEtiketten->revertAll();
    modeTags->revertAll();
    modelKategorien->revertAll();
    modelWasseraufbereitung->revertAll();
    modelSud->revertAll();
}

QSqlQuery Database::sqlExec(QSqlDatabase& db, const QString &query)
{
    QSqlQuery sqlQuery = db.exec(query);
    QSqlError error = db.lastError();
    if (error.isValid())
    {
        mLastError = error;
        qCritical() << query;
        qCritical() << error;
        throw std::runtime_error(error.text().toStdString());
    }
    return sqlQuery;
}

QSqlError Database::lastError() const
{
    return mLastError;
}
