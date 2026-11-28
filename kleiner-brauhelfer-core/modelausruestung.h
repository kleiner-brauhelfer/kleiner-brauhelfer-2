#ifndef MODELAUSRUESTUNG_H
#define MODELAUSRUESTUNG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelAusruestung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,                             // INTEGER  | ID
        ColName,                           // TEXT     | Name der Anlage
        ColTyp,                            // ENUM     | Typ der Anlage [AnlageTyp]
        ColSudhausausbeute,                // REAL     | Sudhausausbeute [%]
        ColVerdampfungsrate,               // REAL     | Verdampfungsrate [Liter/h]
        ColKorrekturWasser,                // REAL     | Korrektur der Nachgussmenge [Liter]
        ColKorrekturFarbe,                 // REAL     | Korrektur Farbe [EBC]
        ColKorrekturMenge,                 // REAL     | Korrektur der Sollmenge [Liter]
        ColMaischebottich_Hoehe,           // REAL     | Maischebottich Hoehe [cm]
        ColMaischebottich_Durchmesser,     // REAL     | Maischebottich Durchmesser [cm]
        ColMaischebottich_MaxFuellhoehe,   // REAL     | Maischebottich max. Fuellhoehe [cm]
        ColSudpfanne_Hoehe,                // REAL     | Sudpfanne Hoehe [cm]
        ColSudpfanne_Durchmesser,          // REAL     | Sudpfanne Durchmesser [cm]
        ColSudpfanne_MaxFuellhoehe,        // REAL     | Sudpfanne max. Fuellhoehe [cm]
        ColKosten,                         // REAL     | Betriebskosten (pauschal) [Waehrung]
        ColBemerkung,                      // TEXT     | Bemerkung
        // virtual columns
        ColDeleted,                        // BOOLEAN  | marked for deletion
        ColMaischebottich_Volumen,         // REAL     | Maischebottich-Zylindervolumen aus Hoehe und Durchmesser [Liter]
        ColMaischebottich_MaxFuellvolumen, // REAL     | Maischebottich-Maximalvolumen aus Fuellhoehe und Durchmesser [Liter]
        ColSudpfanne_Volumen,              // REAL     | Sudpfannen-Zylindervolumen aus Hoehe und Durchmesser [Liter]
        ColSudpfanne_MaxFuellvolumen,      // REAL     | Sudpfannen-Maximalvolumen aus Fuellhoehe und Durchmesser [Liter]
        ColVermoegen,                      // REAL     | Kleineres der maximalen Fuellvolumen von Maischebottich und Sudpfanne [Liter]
        ColAnzahlSude,                     // INTEGER  | Anzahl aller Sude mit dieser Anlage [-]
        ColAnzahlGebrauteSude,             // INTEGER  | Anzahl gebrauter Sude [-]
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelAusruestung(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:
    Brauhelfer* bh;
};

#endif // MODELAUSRUESTUNG_H
