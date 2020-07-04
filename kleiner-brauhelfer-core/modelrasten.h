#ifndef MODELRASTEN_H
#define MODELRASTEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelRasten : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColTyp,
        ColName,
        ColTemp,
        ColDauer,
        ColMengenfaktor,
        ColParam1,
        ColParam2,
        ColParam3,
        ColParam4,
        // virtual
        ColDeleted,
        ColMenge,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelRasten(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void update(const QVariant &sudId);
    double menge(const QVariant &sudId) const;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:

    void getMaischeValues(const QVariant &sudId, int fromRow, double& m, double &c);
    double getPreviousTemp(const QVariant &sudId, int fromRow) const;
    double getPreviousMenge(const QVariant &sudId, int fromRow) const;

private:

    Brauhelfer* bh;
    bool mUpdating;
};

#endif // MODELRASTEN_H
