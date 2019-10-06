#include "brauhelfer.h"
#include "database.h"

const int Brauhelfer::libVersionMajor = VER_MAJ;
const int Brauhelfer::libVerionMinor = VER_MIN;
const int Brauhelfer::libVersionPatch = VER_PAT;
const int Brauhelfer::supportedDatabaseVersion = libVersionMajor * 1000 + libVerionMinor;
const int Brauhelfer::supportedDatabaseVersionMinimal = 21;

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
    connect(mDb->modelHefegaben, SIGNAL(modified()), this, SIGNAL(modified()));
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
        bool wasBlocked = blockSignals(true);
        mDb->save();
        blockSignals(wasBlocked);
        emit saved();
        emit modified();
    }
}

void Brauhelfer::discard()
{
    bool wasBlocked = blockSignals(true);
    mDb->discard();
    blockSignals(wasBlocked);
    emit discarded();
    emit modified();
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
    mDb->update();
    connectDatabase();
    return mDb->version() == supportedDatabaseVersion;
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

ModelHefegaben* Brauhelfer::modelHefegaben() const
{
    return mDb->modelHefegaben;
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

int Brauhelfer::sudKopieren(int sudId, const QString& name, bool teilen)
{
    int row = modelSud()->getRowWithValue("ID", sudId);
    if (row < 0)
        return -1;

    QVariantMap values = modelSud()->copyValues(row);
    values.insert("Sudname", name);
    if (!teilen)
    {
        values.insert("Status", Sud_Status_Rezept);
        values.insert("MerklistenID", 0);
        values.insert("Erstellt", QDateTime::currentDateTime().toString(Qt::ISODate));
        values.remove("Braudatum");
        values.remove("Abfuelldatum");
    }
    row = modelSud()->append(values);

    int neueSudId = values.value("ID").toInt();
    const QVariantMap valueId = {{"SudID", neueSudId}};
    sudKopierenModel(modelRasten(), sudId, valueId);
    sudKopierenModel(modelHopfengaben(), sudId, valueId);
    if (teilen)
        sudKopierenModel(modelHefegaben(), sudId, valueId);
    else
        sudKopierenModel(modelHefegaben(), sudId, {{"SudID", neueSudId}, {"Zugegeben", 0}});
    if (teilen)
        sudKopierenModel(modelWeitereZutatenGaben(), sudId, valueId);
    else
        sudKopierenModel(modelWeitereZutatenGaben(), sudId, {{"SudID", neueSudId}, {"Zugabestatus", EWZ_Zugabestatus_nichtZugegeben}});
    sudKopierenModel(modelMalzschuettung(), sudId, valueId);
    sudKopierenModel(modelAnhang(), sudId, valueId);
    sudKopierenModel(modelFlaschenlabel(), sudId, valueId);
    sudKopierenModel(modelFlaschenlabelTags(), sudId, valueId);
    if (teilen)
    {
        sudKopierenModel(modelSchnellgaerverlauf(), sudId, valueId);
        sudKopierenModel(modelHauptgaerverlauf(), sudId, valueId);
        sudKopierenModel(modelNachgaerverlauf(), sudId, valueId);
        sudKopierenModel(modelBewertungen(), sudId, valueId);
    }
    return row;
}

void Brauhelfer::sudKopierenModel(SqlTableModel* model, int sudId, const QVariantMap &overrideValues)
{
    int colSudId = model->fieldIndex("SudID");
    for (int i = 0; i < model->rowCount(); ++i)
    {
        if (model->index(i, colSudId).data().toInt() == sudId)
        {
            QVariantMap values = model->copyValues(i);
            QVariantMap::const_iterator it = overrideValues.constBegin();
            while (it != overrideValues.constEnd())
            {
                values.insert(it.key(), it.value());
                ++it;
            }
            model->append(values);
        }
    }
}

int  Brauhelfer::sudTeilen(int sudId, const QString& name1, const QString &name2, double prozent)
{
    int row1 = modelSud()->getRowWithValue("ID", sudId);
    if (row1 < 0)
        return -1;
    int row2 = sudKopieren(sudId, name2, true);
    if (row2 < 0)
        return -1;

    int colSudId = modelHefegaben()->fieldIndex("SudID");
    int colMenge = modelHefegaben()->fieldIndex("Menge");

    double Menge = modelSud()->data(row1, "Menge").toDouble();
    double WuerzemengeVorHopfenseihen = modelSud()->data(row1, "WuerzemengeVorHopfenseihen").toDouble();
    double WuerzemengeKochende = modelSud()->data(row1, "WuerzemengeKochende").toDouble();
    double Speisemenge = modelSud()->data(row1, "Speisemenge").toDouble();
    double WuerzemengeAnstellen = modelSud()->data(row1, "WuerzemengeAnstellen").toDouble();
    double erg_AbgefuellteBiermenge = modelSud()->data(row1, "erg_AbgefuellteBiermenge").toDouble();

    double factor = 1.0 - prozent;
    modelSud()->setData(row2, "Menge", Menge * factor);
    modelSud()->setData(row2, "WuerzemengeVorHopfenseihen", WuerzemengeVorHopfenseihen * factor);
    modelSud()->setData(row2, "WuerzemengeKochende", WuerzemengeKochende * factor);
    modelSud()->setData(row2, "Speisemenge", Speisemenge * factor);
    modelSud()->setData(row2, "WuerzemengeAnstellen", WuerzemengeAnstellen * factor);
    modelSud()->setData(row2, "erg_AbgefuellteBiermenge", erg_AbgefuellteBiermenge * factor);
    int sudId2 = modelSud()->data(row2, "ID").toInt();
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->index(row, colSudId).data().toInt() == sudId2)
        {
            QModelIndex index = modelHefegaben()->index(row, colMenge);
            modelHefegaben()->setData(index, qRound(index.data().toInt() * factor));
        }
    }

    factor = prozent;
    modelSud()->setData(row1, "Sudname", name1);
    modelSud()->setData(row1, "Menge", Menge * factor);
    modelSud()->setData(row1, "WuerzemengeVorHopfenseihen", WuerzemengeVorHopfenseihen * factor);
    modelSud()->setData(row1, "WuerzemengeKochende", WuerzemengeKochende * factor);
    modelSud()->setData(row1, "Speisemenge", Speisemenge * factor);
    modelSud()->setData(row1, "WuerzemengeAnstellen", WuerzemengeAnstellen * factor);
    modelSud()->setData(row1, "erg_AbgefuellteBiermenge", erg_AbgefuellteBiermenge * factor);
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->index(row, colSudId).data().toInt() == sudId)
        {
            QModelIndex index = modelHefegaben()->index(row, colMenge);
            modelHefegaben()->setData(index, qRound(index.data().toInt() * factor));
        }
    }

    return row1;
}
