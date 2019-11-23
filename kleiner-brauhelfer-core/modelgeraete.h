#ifndef MODELGERAETE_H
#define MODELGERAETE_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelGeraete : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColBezeichnung,
        ColAusruestungAnlagenID,
        // virtual
        ColDeleted
    };

public:

    ModelGeraete(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELGERAETE_H
