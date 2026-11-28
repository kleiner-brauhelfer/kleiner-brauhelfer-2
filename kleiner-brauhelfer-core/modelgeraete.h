#ifndef MODELGERAETE_H
#define MODELGERAETE_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelGeraete : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,                   // INTEGER  | ID
        ColBezeichnung,          // TEXT     | Name des Geraets
        ColAusruestungAnlagenID, // NUMERIC  | Ausruestung ID, Referenz auf ID in ModelAusruestung (TODO: INTEGER)
        // virtual columns
        ColDeleted,              // BOOLEAN  | marked for deletion
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelGeraete(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
};

#endif // MODELGERAETE_H
