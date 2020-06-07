#ifndef MODELWASSERAUFBEREITUNG_H
#define MODELWASSERAUFBEREITUNG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelWasseraufbereitung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColName,
        ColMenge,
        ColEinheit,
        ColFaktor,
        ColRestalkalitaet,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelWasseraufbereitung(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    double restalkalitaet(const QVariant &sudId) const;

private:

    Brauhelfer* bh;
};

#endif // MODELWASSERAUFBEREITUNG_H
