#ifndef MODELETIKETTEN_H
#define MODELETIKETTEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelEtiketten : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,               // INTEGER  | ID
        ColSudID,            // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColPfad,             // TEXT     | Dateipfad der Etikette
        ColAnzahl,           // INTEGER  | Anzahl Etiketten auf einem Blatt
        ColBreite,           // INTEGER  | Breite [mm]
        ColHoehe,            // INTEGER  | Hoehe [mm]
        ColAbstandHor,       // INTEGER  | Horizontaler Abstand zwischen Etiketten [mm]
        ColAbstandVert,      // INTEGER  | Vertikaler Abstand zwischen Etiketten [mm]
        ColRandOben,         // INTEGER  | Oberer Rand [mm]
        ColRandLinks,        // INTEGER  | Linker Rand [mm]
        ColRandRechts,       // INTEGER  | Rechter Rand [mm]
        ColRandUnten,        // INTEGER  | Unterer Rand [mm]
        ColPapiergroesse,    // INTEGER  | Papiergroesse [QPageSize::PageSizeId]
        ColAusrichtung,      // INTEGER  | Ausrichtung [QPageLayout::Orientation]
        ColHintergrundfarbe, // INTEGER  | Hintergrundfarbe [RGB]
        // virtual columns
        ColDeleted,          // BOOLEAN  | marked for deletion
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelEtiketten(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    bool setValuesFrom(int row, const QVariant& pfad);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    int getLastRow(const QVariant& pfad, int excludeRow) const;
};

#endif // MODELETIKETTEN_H
