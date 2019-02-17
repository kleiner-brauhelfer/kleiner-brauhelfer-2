#include "modelrasten.h"
#include "brauhelfer.h"

ModelRasten::ModelRasten(Brauhelfer* bh, QSqlDatabase db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

void ModelRasten::defaultValues(QVariantMap &values) const
{
    if (!values.contains("RastAktiv"))
        values.insert("RastAktiv", true);
}
