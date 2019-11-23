#include "modelanhang.h"
#include "brauhelfer.h"

ModelAnhang::ModelAnhang(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db)
{
}
