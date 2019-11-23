#ifndef MODELFLASCHENLABEL_H
#define MODELFLASCHENLABEL_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelFlaschenlabel : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColAuswahl,
        ColBreiteLabel,
        ColAnzahlLabels,
        ColAbstandLabels,
        ColSRandOben,
        ColSRandLinks,
        ColSRandRechts,
        ColSRandUnten,
        // virtual
        ColDeleted
    };

public:

    ModelFlaschenlabel(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
};

#endif // MODELFLASCHENLABEL_H
