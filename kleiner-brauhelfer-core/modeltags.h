#ifndef MODELTAGS_H
#define MODELTAGS_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelTags : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,      // INTEGER  | ID
        ColSudID,   // INTEGER  | Sud ID, Referenz auf ID in ModelSud oder -1 fur global
        ColKey,     // TEXT     | Schluessel des benutzerdefinierten Tags
        ColValue,   // TEXT     | Wert des benutzerdefinierten Tags
        // virtual columns
        ColDeleted, // BOOLEAN  | marked for deletion
        ColGlobal,  // BOOLEAN  | Kennzeichnung eines Tags fuer alle Sude
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelTags(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    bool isUnique(const QModelIndex &index, const QVariant &value, const QVariant &sudId, bool ignoreIndexRow = false) const;
    QString getUniqueName(const QModelIndex &index, const QVariant &value, const QVariant &sudId, bool ignoreIndexRow = false) const;

private:

    Brauhelfer* bh;
};

#endif // MODELTAGS_H
