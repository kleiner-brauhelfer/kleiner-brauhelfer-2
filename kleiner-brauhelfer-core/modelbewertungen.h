#ifndef MODELBEWERTUNGEN_H
#define MODELBEWERTUNGEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelBewertungen : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColWoche,
        ColDatum,
        ColSterne,
        ColBemerkung,
        ColFarbe,
        ColFarbeBemerkung,
        ColSchaum,
        ColSchaumBemerkung,
        ColGeruch,
        ColGeruchBemerkung,
        ColGeschmack,
        ColGeschmackBemerkung,
        ColAntrunk,
        ColAntrunkBemerkung,
        ColHaupttrunk,
        ColHaupttrunkBemerkung,
        ColNachtrunk,
        ColNachtrunkBemerkung,
        ColGesamteindruck,
        ColGesamteindruckBemerkung,
        // virtual
        ColDeleted
    };

public:

    ModelBewertungen(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
    int max(int sudId);
    int mean(int sudId);

private:
    Brauhelfer* bh;
};

#endif // MODELBEWERTUNGEN_H
