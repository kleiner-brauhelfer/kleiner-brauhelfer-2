#include "database.h"
#include <stdexcept>
#include <QFile>
#include <QSqlQuery>
#include "brauhelfer.h"

Database::Database() :
    mVersion(-1),
    mLastError(QString()),
    mModels()
{
    QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("kbh"));
}

void Database::createTables(Brauhelfer* bh)
{
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("kbh"), false);
    if (!db.isValid())
        qCritical(Brauhelfer::loggingCategory) << "Database connection is invalid.";
    modelSud = new ModelSud(bh, db);
    modelMaischplan = new ModelMaischplan(bh, db);
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

    mModels = {
        modelMalz,
        modelHopfen,
        modelHefe,
        modelWeitereZutaten,
        modelWasser,
        modelGeraete,
        modelAusruestung,
        modelMaischplan,
        modelMalzschuettung,
        modelHopfengaben,
        modelHefegaben,
        modelWeitereZutatenGaben,
        modelSchnellgaerverlauf,
        modelHauptgaerverlauf,
        modelNachgaerverlauf,
        modelBewertungen,
        modelAnhang,
        modelEtiketten,
        modeTags,
        modelKategorien,
        modelWasseraufbereitung,
        modelSud
    };
}

void Database::setTables()
{
    modelSud->setTable(QStringLiteral("Sud"));
    modelSud->setSort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    modelMaischplan->setTable(QStringLiteral("Maischplan"));
    modelMalzschuettung->setTable(QStringLiteral("Malzschuettung"));
    modelHopfengaben->setTable(QStringLiteral("Hopfengaben"));
    modelHefegaben->setTable(QStringLiteral("Hefegaben"));
    modelWeitereZutatenGaben->setTable(QStringLiteral("WeitereZutatenGaben"));
    modelSchnellgaerverlauf->setTable(QStringLiteral("Schnellgaerverlauf"));
    modelSchnellgaerverlauf->setSort(ModelSchnellgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelHauptgaerverlauf->setTable(QStringLiteral("Hauptgaerverlauf"));
    modelHauptgaerverlauf->setSort(ModelHauptgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelNachgaerverlauf->setTable(QStringLiteral("Nachgaerverlauf"));
    modelNachgaerverlauf->setSort(ModelNachgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelBewertungen->setTable(QStringLiteral("Bewertungen"));
    modelBewertungen->setSort(ModelBewertungen::ColDatum, Qt::AscendingOrder);
    modelMalz->setTable(QStringLiteral("Malz"));
    modelHopfen->setTable(QStringLiteral("Hopfen"));
    modelHefe->setTable(QStringLiteral("Hefe"));
    modelWeitereZutaten->setTable(QStringLiteral("WeitereZutaten"));
    modelAnhang->setTable(QStringLiteral("Anhang"));
    modelAusruestung->setTable(QStringLiteral("Ausruestung"));
    modelGeraete->setTable(QStringLiteral("Geraete"));
    modelWasser->setTable(QStringLiteral("Wasser"));
    modelEtiketten->setTable(QStringLiteral("Etiketten"));
    modeTags->setTable(QStringLiteral("Tags"));
    modelKategorien->setTable(QStringLiteral("Kategorien"));
    modelWasseraufbereitung->setTable(QStringLiteral("Wasseraufbereitung"));

    // sanity check
    Q_ASSERT(modelSud->columnCount() == ModelSud::NumCols);
    Q_ASSERT(modelMaischplan->columnCount() == ModelMaischplan::NumCols);
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
    delete modelMaischplan;
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
    QSqlDatabase::removeDatabase(QStringLiteral("kbh"));
}

bool Database::connect(const QString &dbPath, bool readonly)
{
    if (isConnected())
        return false;

    if (!QFile::exists(dbPath))
    {
        qCritical(Brauhelfer::loggingCategory) << "Database file not found.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("kbh"), false);
    if (!db.isValid())
    {
        qCritical(Brauhelfer::loggingCategory) << "Database connection is invalid.";
        return false;
    }

    db.close();
    db.setDatabaseName(dbPath);
    if (readonly)
        db.setConnectOptions(QStringLiteral("QSQLITE_OPEN_READONLY"));
    else
        db.setConnectOptions();

    if (!db.open())
    {
        qCritical(Brauhelfer::loggingCategory) << "Failed to open database connection.";
        return false;
    }

    try
    {
        QSqlQuery query = sqlExec(db, QStringLiteral("SELECT db_Version FROM Global"));
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

    qCritical(Brauhelfer::loggingCategory) << "Failed to query database version.";
    disconnect();
    return false;
}

void Database::disconnect()
{
    if (isConnected())
    {
        for (auto* model : std::as_const(mModels))
            model->clear();
        mVersion = -1;

        QSqlDatabase db = QSqlDatabase::database(QStringLiteral("kbh"), false);
        db.close();
    }
}

bool Database::isConnected() const
{
    return mVersion != -1;
}

bool Database::isDirty() const
{
    if (!isConnected())
        return false;
    for (auto* model : std::as_const(mModels))
        if (model->isDirty())
            return true;
    return false;
}

void Database::select()
{
    for (auto* model : std::as_const(mModels))
        model->select();
}

int Database::version() const
{
    return mVersion;
}

bool Database::save()
{
    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("kbh"), false);
    if (!db.transaction())
    {
        qCritical(Brauhelfer::loggingCategory) << "Failed to create a database transaction";
        return false;
    }

    for (auto* model : std::as_const(mModels))
    {
        if (!model->submitAll())
        {
            mLastError = model->lastError();
            db.rollback();
            qCritical(Brauhelfer::loggingCategory) << "Failed to submit to database (" << model->tableName() << "):" << mLastError.text();
            return false;
        }
    }

    if (!db.commit())
    {
        mLastError = db.lastError();
        qCritical(Brauhelfer::loggingCategory) << "Failed to commit to database:" << mLastError.text();
        db.rollback();
        return false;
    }

    return true;
}

void Database::discard()
{
    for (auto* model : std::as_const(mModels))
        model->revertAll();
}

QSqlQuery Database::sqlExec(const QSqlDatabase &db, const QString &query)
{
    QSqlQuery sqlQuery(db);
    if (!sqlQuery.exec(query))
    {
        mLastError = sqlQuery.lastError();
        qCritical(Brauhelfer::loggingCategory) << query;
        qCritical(Brauhelfer::loggingCategory) << mLastError.text();
        throw std::runtime_error(mLastError.text().toStdString());
    }
    return sqlQuery;
}

QSqlError Database::lastError() const
{
    return mLastError;
}
