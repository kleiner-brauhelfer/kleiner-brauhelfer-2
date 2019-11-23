#include "modelflaschenlabel.h"
#include "brauhelfer.h"

ModelFlaschenlabel::ModelFlaschenlabel(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db)
{
}
