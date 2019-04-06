#ifndef BRAUHELFER_H
#define BRAUHELFER_H

#include <QObject>
#include "kleiner-brauhelfer-core_global.h"
#include "biercalc.h"
#include "sudobject.h"
#include "sqltablemodel.h"
#include "modelsud.h"
#include "modelmalz.h"
#include "modelhopfen.h"
#include "modelhefe.h"
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
#include "database_defs.h"

class Database;

class LIB_EXPORT Brauhelfer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString databasePath READ databasePath WRITE setDatabasePath NOTIFY databasePathChanged)
    Q_PROPERTY(bool readonly READ readonly WRITE setReadonly NOTIFY readonlyChanged)
    Q_PROPERTY(bool connected READ isConnectedDatabase NOTIFY connectionChanged)
    Q_PROPERTY(int databaseVersion READ databaseVersion NOTIFY connectionChanged)
    Q_PROPERTY(bool modified READ isDirty NOTIFY modified)

    Q_PROPERTY(BierCalc* calc READ calc CONSTANT)
    Q_PROPERTY(SudObject* sud READ sud CONSTANT)
    Q_PROPERTY(SqlTableModel* modelSud READ modelSud CONSTANT)
    Q_PROPERTY(SqlTableModel* modelRastauswahl READ modelRastauswahl CONSTANT)
    Q_PROPERTY(SqlTableModel* modelMalz READ modelMalz CONSTANT)
    Q_PROPERTY(SqlTableModel* modelHopfen READ modelHopfen CONSTANT)
    Q_PROPERTY(SqlTableModel* modelHefe READ modelHefe CONSTANT)
    Q_PROPERTY(SqlTableModel* modelWeitereZutaten READ modelWeitereZutaten CONSTANT)
    Q_PROPERTY(SqlTableModel* modelAusruestung READ modelAusruestung CONSTANT)
    Q_PROPERTY(SqlTableModel* modelGeraete READ modelGeraete CONSTANT)
    Q_PROPERTY(SqlTableModel* modelWasser READ modelWasser CONSTANT)
    Q_PROPERTY(SqlTableModel* modelRasten READ modelRasten CONSTANT)
    Q_PROPERTY(SqlTableModel* modelMalzschuettung READ modelMalzschuettung CONSTANT)
    Q_PROPERTY(SqlTableModel* modelHopfengaben READ modelHopfengaben CONSTANT)
    Q_PROPERTY(SqlTableModel* modelWeitereZutatenGaben READ modelWeitereZutatenGaben CONSTANT)
    Q_PROPERTY(SqlTableModel* modelSchnellgaerverlauf READ modelSchnellgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelHauptgaerverlauf READ modelHauptgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelNachgaerverlauf READ modelNachgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelBewertungen READ modelBewertungen CONSTANT)
    Q_PROPERTY(SqlTableModel* modelAnhang READ modelAnhang CONSTANT)
    Q_PROPERTY(SqlTableModel* modelFlaschenlabel READ modelFlaschenlabel CONSTANT)
    Q_PROPERTY(SqlTableModel* modelFlaschenlabelTags READ modelFlaschenlabelTags CONSTANT)

public:

    static const int libVersionMajor;
    static const int libVerionMinor;
    static const int libVersionPatch;
    static const int supportedDatabaseVersion;

public:

    explicit Brauhelfer(const QString &databasePath = QString(), QObject *parent = nullptr);
    ~Brauhelfer();

    Q_INVOKABLE bool connectDatabase();
    Q_INVOKABLE void disconnectDatabase();
    bool isConnectedDatabase() const;

    bool readonly() const;
    void setReadonly(bool readonly);

    bool isDirty() const;
    Q_INVOKABLE void save();
    Q_INVOKABLE void discard();

    QString databasePath() const;
    void setDatabasePath(const QString &filePath);

    int databaseVersion() const;

    bool updateDatabase();

    Database* db() const;
    BierCalc* calc() const;
    SudObject* sud() const;

    ModelSud* modelSud() const;
    SqlTableModel* modelRastauswahl() const;
    ModelMalz* modelMalz() const;
    ModelHopfen* modelHopfen() const;
    ModelHefe* modelHefe() const;
    ModelWeitereZutaten* modelWeitereZutaten() const;
    ModelAusruestung* modelAusruestung() const;
    SqlTableModel* modelGeraete() const;
    ModelWasser* modelWasser() const;
    ModelRasten* modelRasten() const;
    ModelMalzschuettung* modelMalzschuettung() const;
    ModelHopfengaben* modelHopfengaben() const;
    ModelWeitereZutatenGaben* modelWeitereZutatenGaben() const;
    ModelSchnellgaerverlauf* modelSchnellgaerverlauf() const;
    ModelHauptgaerverlauf* modelHauptgaerverlauf() const;
    ModelNachgaerverlauf* modelNachgaerverlauf() const;
    ModelBewertungen* modelBewertungen() const;
    SqlTableModel* modelAnhang() const;
    SqlTableModel* modelFlaschenlabel() const;
    ModelFlaschenlabelTags* modelFlaschenlabelTags() const;

    int sudKopieren(int sudId, const QString& name, bool teilen = false);
    int sudTeilen(int sudId, const QString &name1, const QString &name2, double prozent);

signals:
    void databasePathChanged(const QString &databasePath);
    void readonlyChanged(bool readonly);
    void connectionChanged(bool connected);
    void modified();
    void saved();
    void discarded();

private:
    void sudKopierenModel(SqlTableModel* model, int sudId, const QVariantMap &overrideValues);

private:
    QString mDatabasePath;
    bool mReadonly;
    Database* mDb;
    BierCalc* mCalc;
    SudObject* mSud;
};

#endif // BRAUHELFER_H
