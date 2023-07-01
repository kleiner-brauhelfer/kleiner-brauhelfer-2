#ifndef BRAUHELFER_H
#define BRAUHELFER_H

#include <QObject>
#include <QLoggingCategory>
#include "kleiner-brauhelfer-core_global.h"
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
#include "modelmaischplan.h"
#include "modeltags.h"
#include "modelanhang.h"
#include "modeletiketten.h"
#include "modelgeraete.h"
#include "modelkategorien.h"
#include "modelwasseraufbereitung.h"

class Database;

class LIB_EXPORT Brauhelfer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString databasePath READ databasePath WRITE setDatabasePath NOTIFY databasePathChanged)
    Q_PROPERTY(bool readonly READ readonly WRITE setReadonly NOTIFY readonlyChanged)
    Q_PROPERTY(bool connected READ isConnectedDatabase NOTIFY connectionChanged)
    Q_PROPERTY(int databaseVersionSupported READ databaseVersionSupported CONSTANT)
    Q_PROPERTY(int databaseVersion READ databaseVersion CONSTANT)
    Q_PROPERTY(bool modified READ isDirty NOTIFY modified)

    Q_PROPERTY(SudObject* sud READ sud CONSTANT)
    Q_PROPERTY(ModelSud* modelSud READ modelSud CONSTANT)
    Q_PROPERTY(ModelMalz* modelMalz READ modelMalz CONSTANT)
    Q_PROPERTY(ModelHopfen* modelHopfen READ modelHopfen CONSTANT)
    Q_PROPERTY(ModelHefe* modelHefe READ modelHefe CONSTANT)
    Q_PROPERTY(ModelWeitereZutaten* modelWeitereZutaten READ modelWeitereZutaten CONSTANT)
    Q_PROPERTY(ModelAusruestung* modelAusruestung READ modelAusruestung CONSTANT)
    Q_PROPERTY(ModelGeraete* modelGeraete READ modelGeraete CONSTANT)
    Q_PROPERTY(ModelWasser* modelWasser READ modelWasser CONSTANT)
    Q_PROPERTY(ModelMaischplan* modelMaischplan READ modelMaischplan CONSTANT)
    Q_PROPERTY(ModelMalzschuettung* modelMalzschuettung READ modelMalzschuettung CONSTANT)
    Q_PROPERTY(ModelHopfengaben* modelHopfengaben READ modelHopfengaben CONSTANT)
    Q_PROPERTY(ModelHefegaben* modelHefegaben READ modelHefegaben CONSTANT)
    Q_PROPERTY(ModelWeitereZutatenGaben* modelWeitereZutatenGaben READ modelWeitereZutatenGaben CONSTANT)
    Q_PROPERTY(ModelSchnellgaerverlauf* modelSchnellgaerverlauf READ modelSchnellgaerverlauf CONSTANT)
    Q_PROPERTY(ModelHauptgaerverlauf* modelHauptgaerverlauf READ modelHauptgaerverlauf CONSTANT)
    Q_PROPERTY(ModelNachgaerverlauf* modelNachgaerverlauf READ modelNachgaerverlauf CONSTANT)
    Q_PROPERTY(ModelBewertungen* modelBewertungen READ modelBewertungen CONSTANT)
    Q_PROPERTY(ModelAnhang* modelAnhang READ modelAnhang CONSTANT)
    Q_PROPERTY(ModelEtiketten* modelEtiketten READ modelEtiketten CONSTANT)
    Q_PROPERTY(ModelTags* modelTags READ modelTags CONSTANT)
    Q_PROPERTY(ModelKategorien* modelKategorien READ modelKategorien CONSTANT)
    Q_PROPERTY(ModelWasseraufbereitung* modelWasseraufbereitung READ modelWasseraufbereitung CONSTANT)

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

    enum class HopfenZeitpunkt
    {
        KochenAlt = 0,
        Vorderwuerze = 1,
        Kochbeginn = 2,
        Kochen = 3,
        Kochende = 4,
        Ausschlagen = 5
    };
    Q_ENUM(HopfenZeitpunkt)

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
        l = 4,
        ml = 5
    };
    Q_ENUM(Einheit)

    enum class RastTyp
    {
        Einmaischen = 0,
        Aufheizen = 1,
        Zubruehen = 2,
        Zuschuetten = 3,
        Dekoktion = 4
    };
    Q_ENUM(RastTyp)

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
        BrauheldPro30 = 0x3030,
    };
    Q_ENUM(AnlageTyp)

public:

    static const int libVersionMajor;
    static const int libVersionMinor;
    static const int libVersionPatch;
    static const int supportedDatabaseVersion;
    static const int supportedDatabaseVersionMinimal;
    static QLoggingCategory loggingCategory;

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
    ModelMaischplan* modelMaischplan() const;
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
    ModelKategorien* modelKategorien() const;
    ModelWasseraufbereitung* modelWasseraufbereitung() const;

    Q_INVOKABLE int sudKopieren(int sudId, const QString& name, bool teilen = false);
    Q_INVOKABLE void sudKopierenModel(SqlTableModel* model, int colSudId, const QVariant &sudId, const QMap<int, QVariant> &overrideValues);
    Q_INVOKABLE int sudTeilen(int sudId, const QString &name1, const QString &name2, double prozent);
    Q_INVOKABLE bool rohstoffAbziehen(Brauhelfer::RohstoffTyp typ, const QString& name, double menge);

signals:
    void databasePathChanged(const QString &databasePath);
    void readonlyChanged(bool readonly);
    void connectionChanged(bool connected);
    void modified();
    void saved();
    void discarded();

private:
    QString mDatabasePath;
    bool mReadonly;
    Database* mDb;
    SudObject* mSud;
};

#endif // BRAUHELFER_H
