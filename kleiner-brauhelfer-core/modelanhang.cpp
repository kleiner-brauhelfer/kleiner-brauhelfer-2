// clazy:excludeall=skipped-base-method
#include "modelanhang.h"
#include "brauhelfer.h"

ModelAnhang::ModelAnhang(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db)
{
}
