// clazy:excludeall=skipped-base-method
#include "modelgeraete.h"
#include "brauhelfer.h"

ModelGeraete::ModelGeraete(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db)
{
}
