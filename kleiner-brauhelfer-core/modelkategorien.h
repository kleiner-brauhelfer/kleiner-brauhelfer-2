#ifndef MODELKATEGORIEN_H
#define MODELKATEGORIEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelKategorien : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColBemerkung,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelKategorien(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    bool setDataExt(const QModelIndex &idx, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

private:

    void addEmpty();

private:

    Brauhelfer* bh;
};

#endif // MODELKATEGORIEN_H
