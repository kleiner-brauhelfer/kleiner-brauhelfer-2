#ifndef MODELRASTEN_H
#define MODELRASTEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelRasten : public SqlTableModel
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
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelRasten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELRASTEN_H
