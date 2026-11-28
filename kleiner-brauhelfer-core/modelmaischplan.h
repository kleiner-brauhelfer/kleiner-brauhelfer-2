#ifndef MODELMAISCHPLAN_H
#define MODELMAISCHPLAN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMaischplan : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,               // INTEGER  | ID
        ColSudID,            // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColTyp,              // ENUM     | Typ des Maischschritts [Brauhelfer::RastTyp]
        ColName,             // TEXT     | Name
        ColTempWasser,       // REAL     | Temperatur Wasser [°C]
        ColAnteilWasser,     // REAL     | Anteil Wasser [%]
        ColTempMalz,         // REAL     | Temperatur Malz [°C]
        ColAnteilMalz,       // REAL     | Anteil Malz [%]
        ColTempRast,         // REAL     | Temperatur Rast [°C]
        ColDauerRast,        // INTEGER  | Dauer Rast [min]
        ColAnteilMaische,    // REAL     | Anteil Maische [%]
        ColTempExtra1,       // REAL     | Temperatur der Dekoktions- oder Extramischung 1 [°C]
        ColDauerExtra1,      // INTEGER  | Dauer der Dekoktions- oder Extramischung 1 [min]
        ColTempExtra2,       // REAL     | Temperatur der Dekoktions- oder Extramischung 2 [°C]
        ColDauerExtra2,      // INTEGER  | Dauer der Dekoktions- oder Extramischung 2 [min]
        // virtual columns
        ColDeleted,          // BOOLEAN  | marked for deletion
        ColMengeWasser,      // REAL     | Wassermenge dieses Schritts aus Anteil der Hauptgussmenge [Liter]
        ColTotalMengeWasser, // REAL     | Kumulierte Wassermenge bis zu diesem Schritt [Liter]
        ColMengeMalz,        // REAL     | Malzmenge dieses Schritts aus Anteil und Gesamtmalzmenge [kg]
        ColTotalMengeMalz,   // REAL     | Kumulierte Malzmenge bis zu diesem Schritt [kg]
        ColMengeMaische,     // REAL     | Maischeanteil an kumulierter Wassermenge und Malzverdraengung [Liter]
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMaischplan(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;

private:

    void updateSud(const QVariant &sudId);
    double getTotalAnteil(const QVariant &sudId, int rowFrom, int colAnteil) const;
    double getPreviousValueTemperature(int row) const;
    void getPreviousMaischeValues(int row, double& m, double &c, double &T) const;

private slots:

    void onSudDataChanged(const QModelIndex &index);
    void onRowsSwapped(int row1, int row2);

private:

    Brauhelfer* bh;
    bool mUpdating;
};

#endif // MODELMAISCHPLAN_H
