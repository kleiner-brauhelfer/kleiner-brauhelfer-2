#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>

class Brauhelfer;
class ModelSud;
class ModelMaischplan;
class ModelMalzschuettung;
class ModelHopfengaben;
class ModelHefegaben;
class ModelWeitereZutatenGaben;
class ModelSchnellgaerverlauf;
class ModelHauptgaerverlauf;
class ModelNachgaerverlauf;
class ModelBewertungen;
class ModelMalz;
class ModelHopfen;
class ModelHefe;
class ModelWeitereZutaten;
class ModelAnhang;
class ModelAusruestung;
class ModelGeraete;
class ModelWasser;
class ModelEtiketten;
class ModelTags;
class ModelKategorien;
class ModelWasseraufbereitung;

class Database
{
    friend class Brauhelfer;

public:
    explicit Database();
    ~Database();
    void createTables(Brauhelfer* bh);

    bool connect(const QString &dbPath, bool readonly);
    void disconnect();
    bool isConnected() const;
    bool isDirty() const;
    void setTables();
    void select();
    int version() const;
    bool save();
    void discard();
    bool update();
    QSqlError lastError() const;

private:
    QSqlQuery sqlExec(const QSqlDatabase &db, const QString &query);

private:
    ModelSud* modelSud;
    ModelMaischplan* modelMaischplan;
    ModelMalzschuettung* modelMalzschuettung;
    ModelHopfengaben* modelHopfengaben;
    ModelHefegaben* modelHefegaben;
    ModelWeitereZutatenGaben* modelWeitereZutatenGaben;
    ModelSchnellgaerverlauf* modelSchnellgaerverlauf;
    ModelHauptgaerverlauf* modelHauptgaerverlauf;
    ModelNachgaerverlauf* modelNachgaerverlauf;
    ModelBewertungen* modelBewertungen;
    ModelMalz* modelMalz;
    ModelHopfen* modelHopfen;
    ModelHefe* modelHefe;
    ModelWeitereZutaten* modelWeitereZutaten;
    ModelAnhang* modelAnhang;
    ModelAusruestung* modelAusruestung;
    ModelGeraete* modelGeraete;
    ModelWasser* modelWasser;
    ModelEtiketten* modelEtiketten;
    ModelTags* modeTags;
    ModelKategorien* modelKategorien;
    ModelWasseraufbereitung* modelWasseraufbereitung;
    int mVersion;
    QSqlError mLastError;
};

#endif // DATABASE_H
