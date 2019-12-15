#ifndef MODELBEWERTUNGEN_H
#define MODELBEWERTUNGEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelBewertungen : public SqlTableModel
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
        ColDeleted,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelBewertungen(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    int max(const QVariant &sudId);
    int mean(const QVariant &sudId);

private:
    Brauhelfer* bh;
};

#endif // MODELBEWERTUNGEN_H
