#ifndef MODELMALZSCHUETTUNG_H
#define MODELMALZSCHUETTUNG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMalzschuettung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColName,
        ColProzent,
        Colerg_Menge,
        ColFarbe,
        ColPotential,
        ColpH,
        // virtual
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMalzschuettung(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELMALZSCHUETTUNG_H
