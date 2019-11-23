#ifndef MODELANHANG_H
#define MODELANHANG_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelAnhang : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColPfad,
        // virtual
        ColDeleted
    };

public:

    ModelAnhang(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELANHANG_H
