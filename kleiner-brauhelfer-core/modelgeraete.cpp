// clazy:excludeall=skipped-base-method
#include "modelgeraete.h"
#include "brauhelfer.h"

ModelGeraete::ModelGeraete(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db)
{
}
