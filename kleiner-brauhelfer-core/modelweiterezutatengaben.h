#ifndef MODELWEITEREZUTATENGABEN_H
#define MODELWEITEREZUTATENGABEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelWeitereZutatenGaben : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColName,
        ColMenge,
        ColEinheit,
        ColTyp,
        ColZeitpunkt,
        ColBemerkung,
        Colerg_Menge,
        ColAusbeute,
        ColFarbe,
        ColZugabeNach,
        ColZugabedauer,
        ColEntnahmeindex,
        ColZugabestatus,
        // virtual
        ColDeleted,
        ColExtrakt,
        ColZugabeDatum,
        ColEntnahmeDatum,
        ColAbfuellbereit,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelWeitereZutatenGaben(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELWEITEREZUTATENGABEN_H
