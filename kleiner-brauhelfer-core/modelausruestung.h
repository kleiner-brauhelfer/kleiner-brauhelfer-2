#ifndef MODELAUSRUESTUNG_H
#define MODELAUSRUESTUNG_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelAusruestung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColTyp,
        ColSudhausausbeute,
        ColVerdampfungsrate,
        ColKorrekturWasser,
        ColKorrekturFarbe,
        ColKorrekturMenge,
        ColMaischebottich_Hoehe,
        ColMaischebottich_Durchmesser,
        ColMaischebottich_MaxFuellhoehe,
        ColSudpfanne_Hoehe,
        ColSudpfanne_Durchmesser,
        ColSudpfanne_MaxFuellhoehe,
        ColKosten,
        ColBemerkung,
        // virtual
        ColDeleted,
        ColMaischebottich_Volumen,
        ColMaischebottich_MaxFuellvolumen,
        ColSudpfanne_Volumen,
        ColSudpfanne_MaxFuellvolumen,
        ColVermoegen,
        ColAnzahlSude,
        ColAnzahlGebrauteSude,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelAusruestung(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private:
    Brauhelfer* bh;
};

#endif // MODELAUSRUESTUNG_H
