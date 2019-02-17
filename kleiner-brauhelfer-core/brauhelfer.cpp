#include "brauhelfer.h"
#include "database.h"
#include <QSqlQuery>

const int Brauhelfer::versionMajor = VER_MAJ;
const int Brauhelfer::verionMinor = VER_MIN;
const int Brauhelfer::versionPatch = VER_PAT;

Brauhelfer::Brauhelfer(const QString &databasePath, QObject *parent) :
    QObject(parent),
    mDatabasePath(databasePath),
    mReadonly(false)
{
    mDb = new Database();
    mDb->createTables(this);
    mSud = new SudObject(this);
    mCalc = new BierCalc();

    connect(mDb->modelSud, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelRastauswahl, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelMalz, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHopfen, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHefe, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWeitereZutaten, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelAusruestung, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelGeraete, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWasser, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelRasten, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelMalzschuettung, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHopfengaben, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWeitereZutatenGaben, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelSchnellgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHauptgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelNachgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelBewertungen, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelAnhang, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelFlaschenlabel, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelFlaschenlabelTags, SIGNAL(modified()), this, SIGNAL(modified()));
}

Brauhelfer::~Brauhelfer()
{
    disconnect();
    disconnectDatabase();
    delete mDb;
    delete mCalc;
    delete mSud;
}

bool Brauhelfer::connectDatabase()
{
    if (isConnectedDatabase())
    {
        mSud->unload();
        mDb->disconnect();
    }
    if (mDb->connect(databasePath(), readonly()))
    {
        mDb->select();
        mSud->unload();
    }
    emit connectionChanged(isConnectedDatabase());
    return isConnectedDatabase();
}

void Brauhelfer::disconnectDatabase()
{
    mSud->unload();
    mDb->disconnect();
    emit connectionChanged(isConnectedDatabase());
}

bool Brauhelfer::isConnectedDatabase() const
{
    return mDb->isConnected();
}

bool Brauhelfer::readonly() const
{
    return mReadonly;
}

void Brauhelfer::setReadonly(bool readonly)
{
    if (mReadonly != readonly)
    {
        mReadonly = readonly;
        emit readonlyChanged(mReadonly);
    }
}

bool Brauhelfer::isDirty() const
{
    return mDb->isDirty();
}

void Brauhelfer::save()
{
    if (!readonly() && isDirty())
    {
        blockSignals(true);
        mDb->save();
        blockSignals(false);
        emit modified();
        emit saved();
    }
}

void Brauhelfer::discard()
{
    blockSignals(true);
    mDb->discard();
    blockSignals(false);
    emit modified();
    emit discarded();
}

QString Brauhelfer::databasePath() const
{
    return mDatabasePath;
}

void Brauhelfer::setDatabasePath(const QString &filePath)
{
    if (mDatabasePath != filePath)
    {
        mDatabasePath = filePath;
        emit databasePathChanged(mDatabasePath);
    }
}

int Brauhelfer::databaseVersion() const
{
    return mDb->version();
}

bool Brauhelfer::updateDatabase()
{
    if (mDb->update())
        connectDatabase();
    return mDb->version() == verionMinor;
}

Database* Brauhelfer::db() const
{
    return mDb;
}

BierCalc* Brauhelfer::calc() const
{
    return mCalc;
}

SudObject* Brauhelfer::sud() const
{
    return mSud;
}

ModelSud *Brauhelfer::modelSud() const
{
    return mDb->modelSud;
}

SqlTableModel* Brauhelfer::modelRastauswahl() const
{
    return mDb->modelRastauswahl;
}

ModelMalz *Brauhelfer::modelMalz() const
{
    return mDb->modelMalz;
}

ModelHopfen *Brauhelfer::modelHopfen() const
{
    return mDb->modelHopfen;
}

ModelHefe *Brauhelfer::modelHefe() const
{
    return mDb->modelHefe;
}

ModelWeitereZutaten *Brauhelfer::modelWeitereZutaten() const
{
    return mDb->modelWeitereZutaten;
}

ModelAusruestung *Brauhelfer::modelAusruestung() const
{
    return mDb->modelAusruestung;
}

SqlTableModel* Brauhelfer::modelGeraete() const
{
    return mDb->modelGeraete;
}

ModelWasser *Brauhelfer::modelWasser() const
{
    return mDb->modelWasser;
}

ModelRasten* Brauhelfer::modelRasten() const
{
    return mDb->modelRasten;
}

ModelMalzschuettung *Brauhelfer::modelMalzschuettung() const
{
    return mDb->modelMalzschuettung;
}

ModelHopfengaben* Brauhelfer::modelHopfengaben() const
{
    return mDb->modelHopfengaben;
}

ModelWeitereZutatenGaben *Brauhelfer::modelWeitereZutatenGaben() const
{
    return mDb->modelWeitereZutatenGaben;
}

ModelSchnellgaerverlauf *Brauhelfer::modelSchnellgaerverlauf() const
{
    return mDb->modelSchnellgaerverlauf;
}

ModelHauptgaerverlauf *Brauhelfer::modelHauptgaerverlauf() const
{
    return mDb->modelHauptgaerverlauf;
}

ModelNachgaerverlauf *Brauhelfer::modelNachgaerverlauf() const
{
    return mDb->modelNachgaerverlauf;
}

ModelBewertungen *Brauhelfer::modelBewertungen() const
{
    return mDb->modelBewertungen;
}

SqlTableModel* Brauhelfer::modelAnhang() const
{
    return mDb->modelAnhang;
}

SqlTableModel* Brauhelfer::modelFlaschenlabel() const
{
    return mDb->modelFlaschenlabel;
}

ModelFlaschenlabelTags *Brauhelfer::modelFlaschenlabelTags() const
{
    return mDb->modelFlaschenlabelTags;
}

int Brauhelfer::sudKopieren(int sudId, const QString& name, bool full)
{
    int row = modelSud()->getRowWithValue("ID", sudId);
    if (row < 0)
        return -1;

    QVariantMap values = modelSud()->copyValues(row);
    values.insert("Sudname", name);
    if (!full)
    {
        values.insert("BierWurdeGebraut", 0);
        values.insert("BierWurdeAbgefuellt", 0);
        values.insert("BierWurdeVerbraucht", 0);
        values.insert("Erstellt", QDateTime::currentDateTime().toString(Qt::ISODate));
        values.remove("Bewertung");
        values.remove("BewertungText");
        values.remove("MerklistenID");
    }
    row = modelSud()->append(values);

    int neueSudId = values.value("ID").toInt();
    sudKopierenModel(modelRasten(), sudId, neueSudId);
    sudKopierenModel(modelHopfengaben(), sudId, neueSudId);
    sudKopierenModel(modelWeitereZutatenGaben(), sudId, neueSudId);
    sudKopierenModel(modelMalzschuettung(), sudId, neueSudId);
    sudKopierenModel(modelAnhang(), sudId, neueSudId);
    sudKopierenModel(modelFlaschenlabel(), sudId, neueSudId);
    sudKopierenModel(modelFlaschenlabelTags(), sudId, neueSudId);
    if (full)
    {
        sudKopierenModel(modelSchnellgaerverlauf(), sudId, neueSudId);
        sudKopierenModel(modelHauptgaerverlauf(), sudId, neueSudId);
        sudKopierenModel(modelNachgaerverlauf(), sudId, neueSudId);
        sudKopierenModel(modelBewertungen(), sudId, neueSudId);
    }
    return row;
}

void Brauhelfer::sudKopierenModel(SqlTableModel* model, int sudId, int neueSudId)
{
    int colSudId = model->fieldIndex("SudID");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->index(i, colSudId).data().toInt() == sudId)
        {
            QVariantMap values = model->copyValues(i);
            values.insert("SudID", neueSudId);
            model->append(values);
        }
    }
}
