#ifndef MODELBEWERTUNGEN_H
#define MODELBEWERTUNGEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelBewertungen : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,                      // INTEGER  | ID
        ColSudID,                   // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColWoche,                   // INTEGER  | Bewertungswoche ab Reifungsstart (TODO: durch virtuelle Spalte ersetzen)
        ColDatum,                   // DATETIME | Datum der Bewertung
        ColSterne,                  // NUMERIC  | Anzahl Sterne [-]
        ColBemerkung,               // TEXT     | Bemerkung
        ColFarbe,                   // INTEGER  | Auswahl Kategorie Farbe
        ColFarbeBemerkung,          // TEXT     | Bemerkung Kategorie Farbe
        ColSchaum,                  // INTEGER  | Auswahl Kategorie Schaum
        ColSchaumBemerkung,         // TEXT     | Bemerkung Kategorie Schaum
        ColGeruch,                  // INTEGER  | Auswahl Kategorie Geruch
        ColGeruchBemerkung,         // TEXT     | Bemerkung Kategorie Geruch
        ColGeschmack,               // INTEGER  | Auswahl Kategorie Geschmack
        ColGeschmackBemerkung,      // TEXT     | Bemerkung Kategorie Geschmack
        ColAntrunk,                 // INTEGER  | Auswahl Kategorie Antrunk
        ColAntrunkBemerkung,        // TEXT     | Bemerkung Kategorie Antrunk
        ColHaupttrunk,              // INTEGER  | Auswahl Kategorie Haupttrunk
        ColHaupttrunkBemerkung,     // TEXT     | Bemerkung Kategorie Haupttrunk
        ColNachtrunk,               // INTEGER  | Auswahl Kategorie Nachtrunk
        ColNachtrunkBemerkung,      // TEXT     | Bemerkung Kategorie Nachtrunk
        ColGesamteindruck,          // INTEGER  | Auswahl Kategorie Gesamteindruck
        ColGesamteindruckBemerkung, // TEXT     | Bemerkung Kategorie Gesamteindruck
        // virtual columns
        ColDeleted,                 // BOOLEAN  | marked for deletion
        ColSudName,                 // TEXT     | Name des zugehoerigen Suds aus ModelSud
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelBewertungen(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    int max(const QVariant &sudId);
    int mean(const QVariant &sudId);

private:
    Brauhelfer* bh;
};

#endif // MODELBEWERTUNGEN_H
