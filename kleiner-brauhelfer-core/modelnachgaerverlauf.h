#ifndef MODELNACHGAERVERLAUF_H
#define MODELNACHGAERVERLAUF_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"
#include <QDateTime>

class Brauhelfer;

class LIB_EXPORT ModelNachgaerverlauf : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColZeitstempel,
        ColDruck,
        ColTemp,
        ColCO2,
        ColBemerkung,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelNachgaerverlauf(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    QDateTime getLastDateTime(const QVariant &sudId) const;
    double getLastCO2(const QVariant &sudId) const;

private:

    int getLastRow(const QVariant &sudId) const;

private:

    Brauhelfer* bh;
};

#endif // MODELNACHGAERVERLAUF_H
