#ifndef BRAUHELFER_H
#define BRAUHELFER_H

#include <QObject>
#include <QDebug>
#include "kleiner-brauhelfer-core_global.h"
#include "biercalc.h"
#include "sudobject.h"
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
#include "modeltags.h"
#include "modelanhang.h"
#include "modeletiketten.h"
#include "modelgeraete.h"

class Database;

class LIB_EXPORT Brauhelfer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString databasePath READ databasePath WRITE setDatabasePath NOTIFY databasePathChanged)
    Q_PROPERTY(bool readonly READ readonly WRITE setReadonly NOTIFY readonlyChanged)
    Q_PROPERTY(bool connected READ isConnectedDatabase NOTIFY connectionChanged)
    Q_PROPERTY(int databaseVersionSupported READ databaseVersionSupported CONSTANT)
    Q_PROPERTY(int databaseVersion READ databaseVersion NOTIFY connectionChanged)
    Q_PROPERTY(bool modified READ isDirty NOTIFY modified)

    Q_PROPERTY(SudObject* sud READ sud CONSTANT)
    Q_PROPERTY(SqlTableModel* modelSud READ modelSud CONSTANT)
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
    Q_PROPERTY(SqlTableModel* modelHefegaben READ modelHefegaben CONSTANT)
    Q_PROPERTY(SqlTableModel* modelWeitereZutatenGaben READ modelWeitereZutatenGaben CONSTANT)
    Q_PROPERTY(SqlTableModel* modelSchnellgaerverlauf READ modelSchnellgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelHauptgaerverlauf READ modelHauptgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelNachgaerverlauf READ modelNachgaerverlauf CONSTANT)
    Q_PROPERTY(SqlTableModel* modelBewertungen READ modelBewertungen CONSTANT)
    Q_PROPERTY(SqlTableModel* modelAnhang READ modelAnhang CONSTANT)
    Q_PROPERTY(SqlTableModel* modelEtiketten READ modelEtiketten CONSTANT)
    Q_PROPERTY(SqlTableModel* modelTags READ modelTags CONSTANT)

public:

    enum class SudStatus
    {
        Rezept = 0,
        Gebraut = 1,
        Abgefuellt = 2,
        Verbraucht = 3
    };
    Q_ENUM(SudStatus)

    enum class RohstoffTyp
    {
        Malz = 0,
        Hopfen = 1,
        Hefe = 2,
        Zusatz = 3
    };
    Q_ENUM(RohstoffTyp)

    enum class BerechnungsartHopfen
    {
        Keine = -1,
        Gewicht = 0,
        IBU = 1
    };
    Q_ENUM(BerechnungsartHopfen)

    enum class HopfenTyp
    {
        Unbekannt = 0,
        Aroma = 1,
        Bitter = 2,
        Universal = 3
    };
    Q_ENUM(HopfenTyp)

    enum class HefeTyp
    {
        Unbekannt = 0,
        Trocken = 1,
        Fluessig = 2
    };
    Q_ENUM(HefeTyp)

    enum class ZusatzTyp
    {
        Honig = 0,
        Zucker = 1,
        Gewuerz = 2,
        Frucht = 3,
        Sonstiges = 4,
        Kraut = 5,
        Wasseraufbereiung = 6,
        Klaermittel = 7,
        Hopfen = 100
    };
    Q_ENUM(ZusatzTyp)

    enum class ZusatzZeitpunkt
    {
        Gaerung = 0,
        Kochen = 1,
        Maischen = 2
    };
    Q_ENUM(ZusatzZeitpunkt)

    enum class ZusatzStatus
    {
        NichtZugegeben = 0,
        Zugegeben = 1,
        Entnommen = 2
    };
    Q_ENUM(ZusatzStatus)

    enum class ZusatzEntnahmeindex
    {
        MitEntnahme = 0,
        OhneEntnahme = 1
    };
    Q_ENUM(ZusatzEntnahmeindex)

    enum class Einheit
    {
        Kg = 0,
        g = 1,
        mg = 2,
        Stk = 3,
        l,
        ml
    };
    Q_ENUM(Einheit)

    enum class AnlageTyp
    {
        Standard = 0x0000,
        GrainfatherG30 = 0x1030,
        GrainfatherG70 = 0x1070,
        Braumeister10 = 0x2010,
        Braumeister20 = 0x2020,
        Braumeister50 = 0x2050,
        Braumeister200 = 0x2140,
        Braumeister500 = 0x2320,
        Braumeister1000 = 0x2640,
    };
    Q_ENUM(AnlageTyp)

public:

    static const int libVersionMajor;
    static const int libVerionMinor;
    static const int libVersionPatch;
    static const int supportedDatabaseVersion;
    static const int supportedDatabaseVersionMinimal;

public:

    explicit Brauhelfer(const QString &databasePath = QString(), QObject *parent = nullptr);
    ~Brauhelfer();

    Q_INVOKABLE bool connectDatabase();
    Q_INVOKABLE void disconnectDatabase();
    bool isConnectedDatabase() const;

    bool readonly() const;
    void setReadonly(bool readonly);

    bool isDirty() const;
    Q_INVOKABLE bool save();
    Q_INVOKABLE void discard();

    QString databasePath() const;
    void setDatabasePath(const QString &filePath);

    int databaseVersionSupported() const;
    int databaseVersion() const;
    QString lastError() const;
    bool updateDatabase();

    SudObject* sud() const;

    ModelSud* modelSud() const;
    ModelMalz* modelMalz() const;
    ModelHopfen* modelHopfen() const;
    ModelHefe* modelHefe() const;
    ModelWeitereZutaten* modelWeitereZutaten() const;
    ModelAusruestung* modelAusruestung() const;
    ModelGeraete* modelGeraete() const;
    ModelWasser* modelWasser() const;
    ModelRasten* modelRasten() const;
    ModelMalzschuettung* modelMalzschuettung() const;
    ModelHopfengaben* modelHopfengaben() const;
    ModelHefegaben* modelHefegaben() const;
    ModelWeitereZutatenGaben* modelWeitereZutatenGaben() const;
    ModelSchnellgaerverlauf* modelSchnellgaerverlauf() const;
    ModelHauptgaerverlauf* modelHauptgaerverlauf() const;
    ModelNachgaerverlauf* modelNachgaerverlauf() const;
    ModelBewertungen* modelBewertungen() const;
    ModelAnhang* modelAnhang() const;
    ModelEtiketten* modelEtiketten() const;
    ModelTags* modelTags() const;

    Q_INVOKABLE int sudKopieren(int sudId, const QString& name, bool teilen = false);
    Q_INVOKABLE int sudTeilen(int sudId, const QString &name1, const QString &name2, double prozent);
    Q_INVOKABLE bool rohstoffAbziehen(RohstoffTyp typ, const QString& name, double menge);

signals:
    void databasePathChanged(const QString &databasePath);
    void readonlyChanged(bool readonly);
    void connectionChanged(bool connected);
    void modified();
    void saved();
    void discarded();

private:
    void sudKopierenModel(SqlTableModel* model, int colSudId, const QVariant &sudId, const QMap<int, QVariant> &overrideValues);

private:
    QString mDatabasePath;
    bool mReadonly;
    Database* mDb;
    SudObject* mSud;
};

#endif // BRAUHELFER_H
