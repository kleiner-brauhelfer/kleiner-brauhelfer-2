#ifndef MODELHAUPTGAERVERLAUF_H
#define MODELHAUPTGAERVERLAUF_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelHauptgaerverlauf : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColZeitstempel,
        ColRestextrakt,
        ColAlc,
        ColTemp,
        ColBemerkung,
        // virtual
        ColDeleted,
        ColsEVG,
        ColtEVG,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelHauptgaerverlauf(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    int getLastRow(const QVariant &sudId) const;
    double getHighestTemp(const QVariant &sudId) const;

private:

    Brauhelfer* bh;
};

#endif // MODELHAUPTGAERVERLAUF_H
