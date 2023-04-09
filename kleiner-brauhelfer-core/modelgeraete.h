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
        ColID,
        ColBezeichnung,
        ColAusruestungAnlagenID,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelGeraete(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
};

#endif // MODELGERAETE_H
