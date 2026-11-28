#ifndef MODELSUD_H
#define MODELSUD_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelSud : public SqlTableModel
{
    Q_OBJECT

    friend class ProxyModelSud;

public:

    enum Column
    {
        ColID,                          // INTEGER  | ID
        ColSudname,                     // TEXT     | Name des Suds
        ColSudnummer,                   // INTEGER  | Sudnummer
        ColKategorie,                   // TEXT     | Kategorie
        ColAnlage,                      // TEXT     | Brauanlage
        ColMenge,                       // REAL     | Soll-Biermenge beim Anstellen [Liter]
        ColSW,                          // REAL     | Stammwuerze [°P]
        ColhighGravityFaktor,           // REAL     | High Gravity Faktor
        ColFaktorHauptguss,             // REAL     | Faktor Hauptguss
        ColWasserprofil,                // TEXT     | Wasserprofil
        ColRestalkalitaetSoll,          // REAL     | Restalkalitaet Soll [mmol/L]
        ColCO2,                         // REAL     | CO2-Gehalt [g/L]
        ColIBU,                         // REAL     | IBU (Bittereinheiten)
        ColberechnungsArtHopfen,        // ENUM     | Berechnungsart Hopfen [Brauhelfer::BerechnungsartHopfen]
        ColKochdauer,                   // INTEGER  | Kochdauer [min]
        ColNachisomerisierungszeit,     // INTEGER  | Nachisomerisierungszeit [min]
        ColReifezeit,                   // INTEGER  | Reifezeit [Tage]
        ColKostenWasserStrom,           // REAL     | Kosten Wasser und Strom [€]
        ColKommentar,                   // TEXT     | Kommentar
        ColStatus,                      // ENUM     | Status des Sudes [Brauhelfer::SudStatus]
        ColBraudatum,                   // DATETIME | Braudatum
        ColAbfuelldatum,                // DATETIME | Abfülldatum
        ColErstellt,                    // DATETIME | Erstellt
        ColGespeichert,                 // DATETIME | Gespeichert
        Colerg_S_Gesamt,                // REAL     | Ermittelte Stammwuerze gesamt [°P]
        Colerg_W_Gesamt,                // REAL     | Ermittelte Wassermenge gesamt [Liter]
        Colerg_WHauptguss,              // REAL     | Ermittelte Wassermenge Hauptguss [Liter]
        Colerg_WNachguss,               // REAL     | Ermittelte Wassermenge Nachguss [Liter]
        Colerg_Farbe,                   // REAL     | Ermittelte Farbe [EBC]
        ColSWKochende,                  // REAL     | Stammwuerze Kochende [°P]
        ColSWAnstellen,                 // REAL     | Stammwuerze Anstellen [°P]
        ColSchnellgaerprobeAktiv,       // INTEGER  | Schnellgaerprobe aktiv
        ColSWSchnellgaerprobe,          // REAL     | Stammwuerze Schnellgaerprobe [°P]
        ColSWJungbier,                  // REAL     | Stammwuerze Jungbier [°P]
        ColTemperaturJungbier,          // REAL     | Temperatur Jungbier [°C]
        ColWuerzemengeVorHopfenseihen,  // REAL     | Wuerzenmenge vor Hopfenseihen [Liter]
        ColWuerzemengeKochende,         // REAL     | Wuerzenmenge Kochende [Liter]
        ColWuerzemengeAnstellen,        // REAL     | Wuerzenmenge Anstellen [Liter]
        ColSpunden,                     // INTEGER  | Spunden
        ColSpeisemenge,                 // REAL     | Speisemenge [Liter]
        ColJungbiermengeAbfuellen,      // REAL     | Jungbiermenge Abfuellen [Liter]
        Colerg_AbgefuellteBiermenge,    // REAL     | Ermittelte abgefuellte Biermenge [Liter]
        Colerg_Sudhausausbeute,         // REAL     | Ermittelte Sudhausausbeute [%]
        Colerg_EffektiveAusbeute,       // REAL     | Ermittelte effektive Ausbeute [%]
        Colerg_Preis,                   // REAL     | Ermittelter Preis [Waehrung]
        Colerg_Alkohol,                 // REAL     | Ermittelter Alkoholgehalt [%]
        ColAusbeuteIgnorieren,          // INTEGER  | Ausbeute ignorieren
        ColMerklistenID,                // INTEGER  | Merklistenkennung
        ColSudhausausbeute,             // REAL     | Sudhausausbeute [%]
        ColVerdampfungsrate,            // REAL     | Verdampfungsrate [Liter/h]
        ColVergaerungsgrad,             // REAL     | Vergaerungsgrad [%]
        ColWuerzemengeKochbeginn,       // REAL     | Wuerzenmenge Kochbeginn [Liter]
        ColSWKochbeginn,                // REAL     | Stammwuerze Kochbeginn [°P]
        ColVerschneidungAbfuellen,      // REAL     | Verschneidung Abfuellen
        ColTemperaturKarbonisierung,    // REAL     | Temperatur Karbonisierung [°C]
        ColBemerkungBrauen,             // TEXT     | Bemerkung Brauen
        ColBemerkungAbfuellen,          // TEXT     | Bemerkung Abfuellen
        ColBemerkungGaerung,            // TEXT     | Bemerkung Gaerung
        ColReifungStart,                // DATETIME | Reifung Start
        ColVerduennungAnstellen,        // REAL     | Verduennung Anstellen
        ColBemerkungZutatenMaischen,    // TEXT     | Bemerkung Zutaten Maischen
        ColBemerkungZutatenKochen,      // TEXT     | Bemerkung Zutaten Kochen
        ColBemerkungZutatenGaerung,     // TEXT     | Bemerkung Zutaten Gaerung
        ColBemerkungMaischplan,         // TEXT     | Bemerkung Maischplan
        ColBemerkungWasseraufbereitung, // TEXT     | Bemerkung Wasseraufbereitung
        ColMengeHefestarter,            // REAL     | Menge Hefestarter [Liter]
        ColSWHefestarter,               // REAL     | Stammwuerze Hefestarter [°P]
        // virtual columns
        ColDeleted,                     // BOOLEAN  | marked for deletion
        ColMengeSollAnstellen,          // REAL     | Sollmenge beim Anstellen aus Zielmenge und Anlagenkorrektur [Liter]
        ColMengeSollKochende,           // REAL     | Sollmenge Kochende nach Abzug von HGF-Menge und Hefestarter [Liter]
        ColMengeSollKochbeginn,         // REAL     | Sollmenge Kochbeginn aus Kochendmenge plus Verdampfungsverlust [Liter]
        ColMengeSollVerdampfung,        // REAL     | Soll-Verdampfungsverlust aus Rate und Kochdauer [Liter]
        ColMengeSollHgf,                // REAL     | Fuer den High-Gravity-Ausgleich benoetigte Menge [Liter]
        ColSWSollAnstellen,             // REAL     | Soll-Stammwuerze beim Anstellen ohne Gaerungszutaten [°P]
        ColSWSollKochende,              // REAL     | Soll-Stammwuerze Kochende unter Beruecksichtigung von HGF und Hefestarter [°P]
        ColSWSollKochbeginn,            // REAL     | Soll-Stammwuerze Kochbeginn ohne Kochzutaten [°P]
        ColSWSollKochbeginnMitWz,       // REAL     | Soll-Stammwuerze Kochbeginn einschliesslich Kochzutaten [°P]
        ColSWAnteilMalz,                // REAL     | Stammwuerzeanteil des Malzes nach Abzug aller Zutaten und des Hefestarters [°P]
        ColSWAnteilZusatzMaischen,      // REAL     | Stammwuerzeanteil der Maisch-Zutaten aus Rezeptgaben [°P]
        ColSWAnteilZusatzKochen,        // REAL     | Stammwuerzeanteil der Koch-Zutaten aus Rezeptgaben [°P]
        ColSWAnteilZusatzGaerung,       // REAL     | Stammwuerzeanteil der Gaerungs-Zutaten aus Rezeptgaben [°P]
        ColSWAnteilHefestarter,         // REAL     | Stammwuerzeanteil des Hefestarters bezogen auf Anstellmenge [°P]
        ColSWAnteilZutaten,             // REAL     | Stammwuerzeanteil aller Zutaten ohne Hefestarter [°P]
        ColSRESoll,                     // REAL     | Soll-Restextrakt aus Stammwuerze, Vergaerungsgrad und unvergaerbaren Zutaten [°P]
        ColAlkoholSoll,                 // REAL     | Soll-Alkoholgehalt aus Stammwuerze und Soll-Restextrakt [%]
        ColSWIst,                       // REAL     | Ist-Stammwuerze beim Anstellen plus Gaerungs-Zutaten [°P]
        ColSREErwartet,                 // REAL     | Erwarteter Restextrakt aus Ist-Stammwuerze und Vergaerungsgrad [°P]
        ColSREIst,                      // REAL     | Ist-Restextrakt aus Schnellgaerprobe oder Jungbier [°P]
        ColMengeIst,                    // REAL     | Ist-Menge passend zum Sudstatus: Rezept, Anstellen oder abgefuelltes Bier [Liter]
        ColIbuIst,                      // REAL     | Ist-IBU, bei gebrautem Sud auf Ist-Menge skaliert
        ColFarbeIst,                    // INTEGER  | Ist-Farbe, bei gebrautem Sud auf Ist-Menge skaliert [EBC]
        ColCO2Ist,                      // REAL     | CO2-Gehalt des letzten Nachgaerverlauf-Eintrags [g/L]
        ColSpundungsdruck,              // REAL     | Erforderlicher Spundungsdruck aus CO2-Ziel und Jungbiertemperatur [bar]
        ColGruenschlauchzeitpunkt,      // INTEGER  | Stammwuerze-Zeitpunkt fuer das Gruenschlauchen aus CO2-Ziel und Schnellgaerprobe
        ColSpeiseNoetig,                // REAL     | Benoetigte Speisemenge fuer das CO2-Ziel [ml]
        ColSpeiseAnteil,                // REAL     | Verwendbare Speisemenge, begrenzt auf Vorrat und null bei Spunden [ml]
        ColZuckerAnteil,                // REAL     | Benoetigte Zuckermenge nach Anrechnung vorhandener Speise [g]
        ColWoche,                       // INTEGER  | Aktuelle Reifungswoche ab Reifungsstart oder Abfuelldatum (beginnend bei 1)
        ColReifezeitDelta,              // INTEGER  | Verbleibende Reifetage; negativ bei ueberschrittener Soll-Reifezeit [Tage]
        ColAbfuellenBereitZutaten,      // INTEGER  | Wahr, wenn alle Hefe- und weiteren Zutaten abfuellbereit sind
        ColWuerzemengeAnstellenTotal,   // REAL     | Anstellwuerze plus Speisemenge [Liter]
        ColVerdampfungsrateIst,         // REAL     | Ist-Verdampfungsrate aus Wuerzemenge bei Kochbeginn und vor Hopfenseihen sowie Kochdauer [%]
        ColsEVG,                        // REAL     | Scheinbarer EVG aus Ist-Stammwuerze und Ist-Restextrakt [%]
        ColtEVG,                        // REAL     | Tatsaechlicher EVG aus Ist-Stammwuerze und korrigiertem Ist-Restextrakt [%]
        ColRestalkalitaetWasser,        // REAL     | Restalkalitaet des ausgewaehlten Wasserprofils [mmol/L]
        ColRestalkalitaetIst,           // REAL     | Wasser-Restalkalitaet plus Wasseraufbereitungs-Zusatz [mmol/L]
        ColPhMalz,                      // REAL     | Mengen-gewichteter pH-Wert der Malzschuettung
        ColPhMaische,                   // REAL     | Maische-pH aus Malz-pH und Ist-Restalkalitaet
        ColPhMaischeSoll,               // REAL     | Soll-Maische-pH aus Malz-pH und Soll-Restalkalitaet
        ColBewertungMittel,             // REAL     | Gerundeter Mittelwert der Bewertungen
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelSud(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    void createConnections();
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    QMap<int, QVariant> copyValues(int row) const Q_DECL_OVERRIDE;
    QVariant dataSud(const QVariant &sudId, int col);
    QVariant dataAnlage(int row, int col) const;
    void setDataAnlage(int row, int col, const QVariant& value);
    QVariant dataWasser(int row, int col) const;
    void setDataWasser(int row, int col, const QVariant& value);
    void update(int row, int colChanged = -1);

private slots:

    void onModelReset();
    void onRowChanged(const QModelIndex &index);
    void onOtherModelRowChanged(const QModelIndex &index);
    void onAnlageRowChanged(const QModelIndex &index);

private:

    bool setDataExt_impl(const QModelIndex &index, const QVariant &value);
    void updateSwWeitereZutaten(int row);
    void updateWasser(int row);
    void updateFarbe(int row);
    void updatePreis(int row);
    void removeRowsFrom(SqlTableModel* model, int colId, const QList<int>& sudIds);

private:

    Brauhelfer* bh;
    bool mUpdating;
    bool mSkipUpdateOnOtherModelChanged;
    QVector<double> swWzMaischenRecipe;
    QVector<double> swWzKochenRecipe;
    QVector<double> swWzGaerungRecipe;
    QVector<double> swWzGaerungCurrent;
    QVector<double> swWzUnvergaerbarRecipe;
};

#endif // MODELSUD_H
