#ifndef MODELANHANG_H
#define MODELANHANG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelAnhang : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColPfad,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelAnhang(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELANHANG_H
