#include "brauhelfer.h"
#include "database.h"
#include <QtMath>

const int Brauhelfer::libVersionMajor = VER_MAJ;
const int Brauhelfer::libVersionMinor = VER_MIN;
const int Brauhelfer::libVersionPatch = VER_PAT;
const int Brauhelfer::supportedDatabaseVersion = libVersionMajor * 1000 + libVersionMinor;
const int Brauhelfer::supportedDatabaseVersionMinimal = 21;

QLoggingCategory Brauhelfer::loggingCategory("kleiner-brauhelfer-core", QtWarningMsg);

Brauhelfer::Brauhelfer(const QString &databasePath, QObject *parent) :
    QObject(parent),
    mDatabasePath(databasePath),
    mReadonly(false)
{
    mDb = new Database();
    mDb->createTables(this);
    mSud = new SudObject(this);

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
    connect(mDb->modelEtiketten, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modeTags, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelKategorien, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWasseraufbereitung, SIGNAL(modified()), this, SIGNAL(modified()));
}

Brauhelfer::~Brauhelfer()
{
    disconnect();
    disconnectDatabase();
    delete mDb;
    delete mSud;
}

bool Brauhelfer::connectDatabase()
{
    qInfo(loggingCategory) << "Brauhelfer::connectDatabase():" << databasePath();
    if (isConnectedDatabase())
    {
        mSud->unload();
        mDb->disconnect();
    }
    if (mDb->connect(databasePath(), readonly()))
    {
        qInfo(loggingCategory) << "Brauhelfer::connectDatabase() version:" << databaseVersion();
        if (mDb->version() == supportedDatabaseVersion)
        {
            mDb->setTables();
            mDb->select();
            mSud->init();
        }
    }
    emit connectionChanged(isConnectedDatabase());
    return isConnectedDatabase();
}

void Brauhelfer::disconnectDatabase()
{
    if (isConnectedDatabase())
    {
        qInfo(loggingCategory) << "Brauhelfer::disconnectDatabase()";
        mSud->unload();
        mDb->disconnect();
        emit connectionChanged(isConnectedDatabase());
    }
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

bool Brauhelfer::save()
{
    if (!readonly() && isDirty())
    {
        qInfo(loggingCategory) << "Brauhelfer::save()";
        bool wasBlocked = blockSignals(true);
        if (mDb->save())
        {
            blockSignals(wasBlocked);
            emit saved();
            emit modified();
            return true;
        }
        blockSignals(wasBlocked);
    }
    return false;
}

void Brauhelfer::discard()
{
    if (isDirty())
    {
        qInfo(loggingCategory) << "Brauhelfer::discard()";
        bool wasBlocked = blockSignals(true);
        mDb->discard();
        blockSignals(wasBlocked);
        emit discarded();
        emit modified();
    }
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

int Brauhelfer::databaseVersionSupported() const
{
    return supportedDatabaseVersion;
}

int Brauhelfer::databaseVersion() const
{
    return mDb->version();
}

QString Brauhelfer::lastError() const
{
    return mDb->lastError().text();
}

bool Brauhelfer::updateDatabase()
{
    qInfo(loggingCategory) << "Brauhelfer::updateDatabase()";
    mDb->update();
    connectDatabase();
    return mDb->version() == supportedDatabaseVersion;
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

ModelGeraete *Brauhelfer::modelGeraete() const
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

ModelAnhang *Brauhelfer::modelAnhang() const
{
    return mDb->modelAnhang;
}

ModelEtiketten *Brauhelfer::modelEtiketten() const
{
    return mDb->modelEtiketten;
}

ModelTags *Brauhelfer::modelTags() const
{
    return mDb->modeTags;
}

ModelKategorien *Brauhelfer::modelKategorien() const
{
    return mDb->modelKategorien;
}

ModelWasseraufbereitung *Brauhelfer::modelWasseraufbereitung() const
{
    return mDb->modelWasseraufbereitung;
}

int Brauhelfer::sudKopieren(int sudId, const QString& name, bool teilen)
{
    qInfo(loggingCategory) << "Brauhelfer::sudKopieren():" << sudId;
    int row = modelSud()->getRowWithValue(ModelSud::ColID, sudId);
    if (row < 0)
        return -1;

    QMap<int, QVariant> values = modelSud()->copyValues(row);
    values.insert(ModelSud::ColSudname, name);
    if (!teilen)
    {
        values.insert(ModelSud::ColStatus, static_cast<int>(SudStatus::Rezept));
        values.insert(ModelSud::ColMerklistenID, 0);
        values.insert(ModelSud::ColErstellt, QDateTime::currentDateTime().toString(Qt::ISODate));
        values.remove(ModelSud::ColBraudatum);
        values.remove(ModelSud::ColAbfuelldatum);
    }
    row = modelSud()->appendDirect(values);

    QVariant neueSudId = values.value(ModelSud::ColID);
    sudKopierenModel(modelRasten(), ModelRasten::ColSudID, sudId, {{ModelRasten::ColSudID, neueSudId}});
    sudKopierenModel(modelHopfengaben(), ModelHopfengaben::ColSudID, sudId, {{ModelHopfengaben::ColSudID, neueSudId}});
    if (teilen)
        sudKopierenModel(modelHefegaben(), ModelHefegaben::ColSudID, sudId, {{ModelHefegaben::ColSudID, neueSudId}});
    else
        sudKopierenModel(modelHefegaben(), ModelHefegaben::ColSudID, sudId, {{ModelHefegaben::ColSudID, neueSudId}, {ModelHefegaben::ColZugegeben, false}});
    if (teilen)
        sudKopierenModel(modelWeitereZutatenGaben(), ModelWeitereZutatenGaben::ColSudID, sudId, {{ModelWeitereZutatenGaben::ColSudID, neueSudId}});
    else
        sudKopierenModel(modelWeitereZutatenGaben(), ModelWeitereZutatenGaben::ColSudID, sudId, {{ModelWeitereZutatenGaben::ColSudID, neueSudId}, {ModelWeitereZutatenGaben::ColZugabestatus, static_cast<int>(Brauhelfer::ZusatzStatus::NichtZugegeben)}});
    sudKopierenModel(modelMalzschuettung(), ModelMalzschuettung::ColSudID, sudId, {{ModelMalzschuettung::ColSudID, neueSudId}});
    sudKopierenModel(modelAnhang(), ModelAnhang::ColSudID, sudId, {{ModelAnhang::ColSudID, neueSudId}});
    sudKopierenModel(modelEtiketten(), ModelEtiketten::ColSudID, sudId, {{ModelEtiketten::ColSudID, neueSudId}});
    sudKopierenModel(modelTags(), ModelTags::ColSudID, sudId, {{ModelTags::ColSudID, neueSudId}});
    sudKopierenModel(modelWasseraufbereitung(), ModelWasseraufbereitung::ColSudID, sudId, {{ModelWasseraufbereitung::ColSudID, neueSudId}});
    if (teilen)
    {
        sudKopierenModel(modelSchnellgaerverlauf(), ModelSchnellgaerverlauf::ColSudID, sudId, {{ModelSchnellgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelHauptgaerverlauf(), ModelHauptgaerverlauf::ColSudID, sudId, {{ModelHauptgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelNachgaerverlauf(), ModelNachgaerverlauf::ColSudID, sudId, {{ModelNachgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelBewertungen(), ModelBewertungen::ColSudID, sudId, {{ModelBewertungen::ColSudID, neueSudId}});
    }
    modelSud()->update(row);
    return row;
}

void Brauhelfer::sudKopierenModel(SqlTableModel* model, int colSudId, const QVariant &sudId, const QMap<int, QVariant> &overrideValues)
{
    int N = model->rowCount();
    for (int r = 0; r < N; ++r)
    {
        if (model->data(r, colSudId) == sudId)
        {
            QMap<int, QVariant> values = model->copyValues(r);
            QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
            while (it != overrideValues.constEnd())
            {
                values.insert(it.key(), it.value());
                ++it;
            }
            model->appendDirect(values);
        }
    }
}

int Brauhelfer::sudTeilen(int sudId, const QString& name1, const QString &name2, double prozent)
{
    qInfo(loggingCategory) << "Brauhelfer::sudTeilen():" << sudId;
    int row1 = modelSud()->getRowWithValue(ModelSud::ColID, sudId);
    if (row1 < 0)
        return -1;
    int row2 = sudKopieren(sudId, name2, true);
    if (row2 < 0)
        return -1;

    BerechnungsartHopfen berechnungsArtHopfen = static_cast<BerechnungsartHopfen>(modelSud()->data(row1, ModelSud::ColberechnungsArtHopfen).toInt());

    std::vector<int> listIndexes = {
        ModelSud::ColMenge,
        ModelSud::ColWuerzemengeKochbeginn,
        ModelSud::ColWuerzemengeVorHopfenseihen,
        ModelSud::ColWuerzemengeKochende,
        ModelSud::ColSpeisemenge,
        ModelSud::ColVerschneidungAbfuellen,
        ModelSud::ColWuerzemengeAnstellen,
        ModelSud::ColJungbiermengeAbfuellen,
        ModelSud::Colerg_AbgefuellteBiermenge,
        ModelSud::Colerg_S_Gesamt,
        ModelSud::Colerg_WHauptguss,
        ModelSud::Colerg_WNachguss,
        ModelSud::Colerg_W_Gesamt,
    };
    std::vector<double> listValues(listIndexes.size());
    for (size_t i = 0; i < listIndexes.size(); i++)
        listValues[i] = modelSud()->data(row1, listIndexes[i]).toDouble();

    double factor = 1.0 - prozent;
    for (size_t i = 0; i < listIndexes.size(); i++)
        modelSud()->setData(row2, listIndexes[i], listValues[i] * factor);
    int sudId2 = modelSud()->data(row2, ModelSud::ColID).toInt();
    for (int row = 0; row < modelHopfengaben()->rowCount(); ++row)
    {
        if (modelHopfengaben()->data(row, ModelHopfengaben::ColSudID).toInt() == sudId2)
        {
            double ausbeute = modelHopfengaben()->data(row, ModelHopfengaben::ColAusbeute).toDouble();
            if (berechnungsArtHopfen == BerechnungsartHopfen::Keine || (berechnungsArtHopfen == BerechnungsartHopfen::IBU && ausbeute == 0.0))
            {
                QModelIndex idx = modelHopfengaben()->index(row, ModelHopfengaben::Colerg_Menge);
                modelHopfengaben()->setData(idx, idx.data().toDouble() * factor);
            }
        }
    }
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->data(row, ModelHefegaben::ColSudID).toInt() == sudId2)
        {
            QModelIndex idx = modelHefegaben()->index(row, ModelHefegaben::ColMenge);
            modelHefegaben()->setData(idx, qRound(idx.data().toInt() * factor));
        }
    }

    factor = prozent;
    modelSud()->setData(row1, ModelSud::ColSudname, name1);
    for (size_t i = 0; i < listIndexes.size(); i++)
        modelSud()->setData(row1, listIndexes[i], listValues[i] * factor);
    for (int row = 0; row < modelHopfengaben()->rowCount(); ++row)
    {
        if (modelHopfengaben()->data(row, ModelHopfengaben::ColSudID).toInt() == sudId)
        {
            double ausbeute = modelHopfengaben()->data(row, ModelHopfengaben::ColAusbeute).toDouble();
            if (berechnungsArtHopfen == BerechnungsartHopfen::Keine || (berechnungsArtHopfen == BerechnungsartHopfen::IBU && ausbeute == 0.0))
            {
                QModelIndex idx = modelHopfengaben()->index(row, ModelHopfengaben::Colerg_Menge);
                modelHopfengaben()->setData(idx, idx.data().toDouble() * factor);
            }
        }
    }
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->data(row, ModelHefegaben::ColSudID).toInt() == sudId)
        {
            QModelIndex idx = modelHefegaben()->index(row, ModelHefegaben::ColMenge);
            modelHefegaben()->setData(idx, qRound(idx.data().toInt() * factor));
        }
    }

    return row1;
}

bool Brauhelfer::rohstoffAbziehen(RohstoffTyp typ, const QString& name, double menge)
{
    int row;
    double mengeLager;
    SqlTableModel *modelLager;
    switch (typ)
    {
    case RohstoffTyp::Malz:
        modelLager = modelMalz();
        row = modelLager->getRowWithValue(ModelMalz::ColName, name);
        if (row != -1)
        {
            mengeLager = modelLager->data(row, ModelMalz::ColMenge).toDouble() - menge;
            if (mengeLager < 0.0)
                mengeLager = 0.0;
            return modelLager->setData(row, ModelMalz::ColMenge, mengeLager);
        }
        break;
    case RohstoffTyp::Hopfen:
        modelLager = modelHopfen();
        row = modelLager->getRowWithValue(ModelHopfen::ColName, name);
        if (row != -1)
        {
            mengeLager = modelLager->data(row, ModelHopfen::ColMenge).toDouble() - menge;
            if (mengeLager < 0.0)
                mengeLager = 0.0;
            return modelLager->setData(row, ModelHopfen::ColMenge, mengeLager);
        }
        break;
    case RohstoffTyp::Hefe:
        modelLager = modelHefe();
        row = modelLager->getRowWithValue(ModelHefe::ColName, name);
        if (row != -1)
        {
            mengeLager = modelLager->data(row, ModelHefe::ColMenge).toInt() - menge;
            if (mengeLager < 0.0)
                mengeLager = 0.0;
            return modelLager->setData(row, ModelHefe::ColMenge, mengeLager);
        }
        break;
    case RohstoffTyp::Zusatz:
        modelLager = modelWeitereZutaten();
        row = modelLager->getRowWithValue(ModelWeitereZutaten::ColName, name);
        if (row != -1)
        {
            mengeLager = modelLager->data(row, ModelWeitereZutaten::ColMenge).toDouble();
            Einheit einheit = static_cast<Einheit>(modelLager->data(row, ModelWeitereZutaten::ColEinheit).toInt());
            switch (einheit)
            {
            case Einheit::Kg:
                mengeLager -= menge / 1000;
                break;
            case Einheit::g:
                mengeLager -= menge;
                break;
            case Einheit::mg:
                mengeLager -= menge * 1000;
                break;
            case Einheit::Stk:
                mengeLager -= qCeil(menge);
                break;
            case Einheit::l:
                mengeLager -= menge / 1000;
                break;
            case Einheit::ml:
                mengeLager -= menge;
                break;
            }
            if (mengeLager < 0.0)
                mengeLager = 0.0;
            return modelLager->setData(row, ModelWeitereZutaten::ColMenge, mengeLager);
        }
        break;
    }
    return false;
}
