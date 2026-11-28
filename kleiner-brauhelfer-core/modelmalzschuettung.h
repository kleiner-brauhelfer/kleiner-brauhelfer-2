#ifndef MODELMALZSCHUETTUNG_H
#define MODELMALZSCHUETTUNG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMalzschuettung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,             // INTEGER  | ID
        ColSudID,          // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColName,           // TEXT     | Malzname
        ColProzent,        // REAL     | Anteil an der Gesamtmalzmenge [%]
        Colerg_Menge,      // REAL     | Berechnete Malzmenge aus Anteil und Gesamtmalzmenge [kg]
        ColFarbe,          // REAL     | Farbe [EBC]
        ColPotential,      // REAL     | Extraktpotenzial [°P/kg] (bei Berechnungen nicht verwendet)
        ColpH,             // REAL     | pH
        // virtual columns
        ColDeleted,        // BOOLEAN  | marked for deletion
        ColExtrakt,        // REAL     | Malz-Stammwuerzeanteil multipliziert mit Malzanteil [°P]
        ColExtraktProzent, // REAL     | Anteil des Malzextrakts an der Stammwuerze aus allen Zutaten [%]
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMalzschuettung(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &idx) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void update(const QVariant &name, int col, const QVariant &value);
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELMALZSCHUETTUNG_H
