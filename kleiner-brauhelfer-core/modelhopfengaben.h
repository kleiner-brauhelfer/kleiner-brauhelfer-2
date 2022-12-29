#ifndef MODELHOPFENGABEN_H
#define MODELHOPFENGABEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelHopfengaben : public SqlTableModel
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
        ColZeitpunkt, // in DB: Vorderwuerze (bei Gelegenheit umbenennen)
        // virtual
        ColDeleted,
        ColIBUAnteil,
        ColAusbeute,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelHopfengaben(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void update(const QVariant &name, int col, const QVariant &value);
    int import(int row);
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;

private slots:

    void onSudDataChanged(const QModelIndex &index);

private:

    Brauhelfer* bh;
};

#endif // MODELHOPFENGABEN_H
