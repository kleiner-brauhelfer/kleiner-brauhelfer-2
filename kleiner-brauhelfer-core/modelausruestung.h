#ifndef MODELAUSRUESTUNG_H
#define MODELAUSRUESTUNG_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelAusruestung : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColTyp,
        ColSudhausausbeute,
        ColVerdampfungsziffer,
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
        // virtual
        ColDeleted,
        ColMaischebottich_Volumen,
        ColMaischebottich_MaxFuellvolumen,
        ColSudpfanne_Volumen,
        ColSudpfanne_MaxFuellvolumen,
        ColVermoegen,
        ColAnzahlSude
    };

public:

    ModelAusruestung(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;

private:
    Brauhelfer* bh;
};

#endif // MODELAUSRUESTUNG_H
