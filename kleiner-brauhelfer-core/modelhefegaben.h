#ifndef MODELHEFEGABEN_H
#define MODELHEFEGABEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelHefegaben : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,            // INTEGER  | ID
        ColSudID,         // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColName,          // TEXT     | Hefename
        ColMenge,         // INTEGER  | Menge [Einheit]
        ColZugegeben,     // BOOLEAN  | Zugegeben
        ColZugabeNach,    // INTEGER  | Zugabe nach [Tagen seit Braudatum]
        // virtual columns
        ColDeleted,       // BOOLEAN  | Virtuelles Löschmerkmal
        ColZugabeDatum,   // DATETIME | Zugabedatum, Braudatum + ZugabeNach
        ColAbfuellbereit, // BOOLEAN  | Bereit zum Abfuellen, sobald die Hefe zugegeben wurde
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelHefegaben(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void update(const QVariant &name, int col, const QVariant &value);
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELHEFEGABEN_H
