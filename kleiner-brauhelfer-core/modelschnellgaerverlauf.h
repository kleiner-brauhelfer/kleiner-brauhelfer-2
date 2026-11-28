#ifndef MODELSCHNELLGAERVERLAUF_H
#define MODELSCHNELLGAERVERLAUF_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelSchnellgaerverlauf : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,          // INTEGER  | ID
        ColSudID,       // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColZeitstempel, // DATETIME | Zeitstempel der Messung
        ColRestextrakt, // REAL     | Restextrakt [°P]
        ColAlc,         // REAL     | Alkohol [%vol]
        ColTemp,        // REAL     | Temperatur [°C]
        ColBemerkung,   // TEXT     | Bemerkung
        // virtual columns
        ColDeleted,     // BOOLEAN  | marked for deletion
        ColsEVG,        // REAL     | Scheinbarer EVG aus Ist-Stammwuerze und Restextrakt [%]
        ColtEVG,        // REAL     | Tatsaechlicher EVG aus Ist-Stammwuerze und korrigiertem Restextrakt [%]
        ColTage,        // INTEGER  | Vergangene Tage seit der ersten Messung des Suds
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelSchnellgaerverlauf(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    int getLastRow(const QVariant &sudId) const;

private:

    Brauhelfer* bh;
};

#endif // MODELSCHNELLGAERVERLAUF_H
