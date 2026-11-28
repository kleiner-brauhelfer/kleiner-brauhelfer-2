#ifndef MODELWASSER_H
#define MODELWASSER_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelWasser : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,                   // INTEGER  | ID
        ColName,                 // TEXT     | Name
        ColHydrogencarbonat,     // REAL     | Hydrogencarbonat [mg/L]
        ColCalcium,              // REAL     | Calcium [mg/L]
        ColMagnesium,            // REAL     | Magnesium [mg/L]
        ColSulfat,               // REAL     | Sulfat [mg/L]
        ColChlorid,              // REAL     | Chlorid [mg/L]
        ColNatrium,              // REAL     | Natrium [mg/L]
        ColRestalkalitaetAdd,    // REAL     | Restalkalitaet Zusatz [mmol/L]
        ColBemerkung,            // TEXT     | Bemerkung
        // virtual columns
        ColDeleted,              // BOOLEAN  | marked for deletion
        ColHydrogencarbonatMmol, // REAL     | Hydrogencarbonat aus mg/L in mmol/L umgerechnet
        ColCalciumMmol,          // REAL     | Calcium aus mg/L in mmol/L umgerechnet
        ColMagnesiumMmol,        // REAL     | Magnesium aus mg/L in mmol/L umgerechnet
        ColSulfatMmol,           // REAL     | Sulfat aus mg/L in mmol/L umgerechnet
        ColChloridMmol,          // REAL     | Chlorid aus mg/L in mmol/L umgerechnet
        ColNatriumMmol,          // REAL     | Natrium aus mg/L in mmol/L umgerechnet
        ColCarbonatHaerte,       // REAL     | Carbonathaerte aus Hydrogencarbonat berechnet [°dH]
        ColCalciumHaerte,        // REAL     | Calciumhaerte aus Calcium berechnet [°dH]
        ColMagnesiumHaerte,      // REAL     | Magnesiumhaerte aus Magnesium berechnet [°dH]
        ColRestalkalitaet,       // REAL     | Restalkalitaet aus Hydrogencarbonat, Calcium, Magnesium und Zusatz [mmol/L]
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    static double Restalkalitaet(double hydrogencarbonat, double calcium, double magnesium);

public:

    ModelWasser(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    Brauhelfer* bh;
};

#endif // MODELWASSER_H
