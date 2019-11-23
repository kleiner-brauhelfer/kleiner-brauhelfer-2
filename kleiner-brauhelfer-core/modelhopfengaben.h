#ifndef MODELHOPFENGABEN_H
#define MODELHOPFENGABEN_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelHopfengaben : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColSudID,
        ColName,
        ColProzent,
        ColZeit,
        Colerg_Menge,
        ColAlpha,
        ColPellets,
        ColVorderwuerze,
        // virtual
        ColDeleted,
        ColIBUAnteil,
        ColAusbeute
    };

public:

    ModelHopfengaben(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELHOPFENGABEN_H
