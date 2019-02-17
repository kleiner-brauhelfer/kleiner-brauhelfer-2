#ifndef MODELRASTEN_H
#define MODELRASTEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelRasten : public SqlTableModel
{
public:
    ModelRasten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private:
    Brauhelfer* bh;
};

#endif // MODELRASTEN_H
