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
        ColAnteilWasser,
        ColTempWasser,
        ColAnteilMalz,
        ColTempMalz,
        ColAnteilMaische,
        ColTempMaische,
        ColDauerMaische,
        ColTotalMengeWasser,
        ColTotalMengeMalz,
        ColDauerExtra1,
        ColTempExtra1,
        ColDauerExtra2,
        ColTempExtra2,
        // virtual
        ColDeleted,
        ColMengeWasser,
        ColMengeMalz,
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
    double getPreviousTempMaische(const QVariant &sudId, int fromRow) const;
    double getPreviousMengeWasser(const QVariant &sudId, int fromRow) const;

    void getMaischeValues(const QVariant &sudId, int fromRow, double& m, double &c);

private:

    Brauhelfer* bh;
    bool mUpdating;
};

#endif // MODELMAISCHPLAN_H
