#ifndef MODELWEITEREZUTATENGABEN_H
#define MODELWEITEREZUTATENGABEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelWeitereZutatenGaben : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,             // INTEGER  | ID
        ColSudID,          // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColName,           // TEXT     | Name der Zutat
        ColMenge,          // REAL     | Menge [Einheit/Liter]
        ColEinheit,        // ENUM     | Einheit [Brauhelfer::Einheit]
        ColTyp,            // ENUM     | Zutatentyp [Brauhelfer::ZusatzTyp]
        ColZeitpunkt,      // ENUM     | Zeitpunkt der Zugabe [Brauheler::ZusatzZeitpunkt]
        ColBemerkung,      // TEXT     | Bemerkung
        Colerg_Menge,      // REAL     | Ermittelte Menge [Einheit]
        ColAusbeute,       // REAL     | Ausbeute [%]
        ColFarbe,          // REAL     | Farbe [EBC]
        ColZugabeNach,     // INTEGER  | Zugabe nach [Tage]
        ColZugabedauer,    // INTEGER  | Zugabedauer [min]
        ColEntnahmeindex,  // ENUM     | Entnahmeverhalten [Brauhelfer::ZusatzEntnahmeindex]
        ColZugabestatus,   // ENUM     | Zugegeben-/Entnommenstatus [Brauhelfer::ZusatzStatus]
        ColUnvergaerbar,   // BOOLEAN  | Unvergaerbar
        // virtual columns
        ColDeleted,        // BOOLEAN  | marked for deletion
        Colerg_MengeIst,   // REAL     | Ist-Menge aus Wuerzemenge beim Anstellen und Menge pro Liter [Einheit]
        ColExtrakt,        // REAL     | Extrakt aus Menge, Einheit und Ausbeute berechnet [°P]
        ColExtraktProzent, // REAL     | Anteil des Zutatenextrakts an der Stammwuerze aus allen Zutaten [%]
        ColZugabeDatum,    // DATETIME | Zugabedatum aus Braudatum plus ZugabeNach berechnet
        ColEntnahmeDatum,  // DATETIME | Entnahmedatum aus Zugabedatum plus Zugabedauer berechnet
        ColAbfuellbereit,  // BOOLEAN  | Nicht bereit bei ausstehender Gaerungszugabe oder Entnahme, sonst bereit
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelWeitereZutatenGaben(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void update(const QVariant &name, int col, const QVariant &value);
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELWEITEREZUTATENGABEN_H
