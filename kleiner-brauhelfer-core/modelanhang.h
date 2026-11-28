#ifndef MODELANHANG_H
#define MODELANHANG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelAnhang : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,      // INTEGER  | ID
        ColSudID,   // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColPfad,    // TEXT     | Dateipfad des Anhangs
        // virtual columns
        ColDeleted, // BOOLEAN  | marked for deletion
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelAnhang(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
};

#endif // MODELANHANG_H
