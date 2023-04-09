#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include "modelsud.h"
#include "modelmalz.h"
#include "modelhopfen.h"
#include "modelhefe.h"
#include "modelhefegaben.h"
#include "modelweiterezutaten.h"
#include "modelschnellgaerverlauf.h"
#include "modelhauptgaerverlauf.h"
#include "modelnachgaerverlauf.h"
#include "modelbewertungen.h"
#include "modelwasser.h"
#include "modelmalzschuettung.h"
#include "modelhopfengaben.h"
#include "modelweiterezutatengaben.h"
#include "modelausruestung.h"
#include "modelrasten.h"
#include "modeltags.h"
#include "modelanhang.h"
#include "modeletiketten.h"
#include "modelgeraete.h"
#include "modelkategorien.h"
#include "modelwasseraufbereitung.h"

class QSqlDatabase;
class Brauhelfer;

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
    ModelRasten* modelRasten;
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
