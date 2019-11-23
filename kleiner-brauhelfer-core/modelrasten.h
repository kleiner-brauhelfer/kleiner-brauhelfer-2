#ifndef MODELRASTEN_H
#define MODELRASTEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelRasten : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColName,
        ColTemp,
        ColDauer,
        // virtual
        ColDeleted
    };

public:

    ModelRasten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELRASTEN_H
