#include "modelrasten.h"
#include "brauhelfer.h"

ModelRasten::ModelRasten(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db)
{
}
