#ifndef MODELMAISCHPLAN_H
#define MODELMAISCHPLAN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMaischplan : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColTyp,
        ColName,
        ColTempWasser,
        ColAnteilWasser,
        ColTempMalz,
        ColAnteilMalz,
        ColTempRast,
        ColDauerRast,
        ColAnteilMaische,
        ColTempExtra1,
        ColDauerExtra1,
        ColTempExtra2,
        ColDauerExtra2,
        // virtual
        ColDeleted,
        ColMengeWasser,
        ColTotalMengeWasser,
        ColMengeMalz,
        ColTotalMengeMalz,
        ColMengeMaische,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMaischplan(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;

private:

    void updateSud(const QVariant &sudId);
    double getTotalAnteil(const QVariant &sudId, int rowFrom, int colAnteil) const;
    double getPreviousValueTemperature(int row) const;
    void getPreviousMaischeValues(int row, double& m, double &c, double &T) const;

private slots:

    void onSudDataChanged(const QModelIndex &index);
    void onRowsSwapped(int row1, int row2);

private:

    Brauhelfer* bh;
    bool mUpdating;
};

#endif // MODELMAISCHPLAN_H
