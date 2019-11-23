#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include "sqltablemodel.h"
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
#include "modelflaschenlabeltags.h"
#include "modelanhang.h"
#include "modelflaschenlabel.h"
#include "modelgeraete.h"

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
    void select();
    int version() const;
    void save();
    void discard();
    bool update();

private:
    void setTables();
    QSqlQuery sqlExec(QSqlDatabase &db, const QString &query);

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
    ModelFlaschenlabel* modelFlaschenlabel;
    ModelFlaschenlabelTags* modelFlaschenlabelTags;
    int mVersion;
};

#endif // DATABASE_H
