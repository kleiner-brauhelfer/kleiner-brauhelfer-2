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
        ColID,             // INTEGER  | ID
        ColSudID,          // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColName,           // TEXT     | Name
        ColMenge,          // REAL     | Aufbereitungsmenge; wird mit Faktor in Restalkalitaet umgerechnet [Einheit]
        ColEinheit,        // ENUM     | Einheit [Brauhelfer::Einheit]
        ColFaktor,         // REAL     | Umrechnungsfaktor zwischen Aufbereitungsmenge und Restalkalitaet
        ColRestalkalitaet, // REAL     | Restalkalitaetsbeitrag aus Menge geteilt durch Faktor [mmol/L]
        // virtual columns
        ColDeleted,        // BOOLEAN  | marked for deletion
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelWasseraufbereitung(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    double restalkalitaet(const QVariant &sudId) const;

private:

    Brauhelfer* bh;
};

#endif // MODELWASSERAUFBEREITUNG_H
