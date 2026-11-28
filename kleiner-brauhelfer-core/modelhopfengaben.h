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
        ColID,        // INTEGER  | ID
        ColSudID,     // INTEGER  | Sud ID, Referenz auf ID in ModelSud
        ColName,      // TEXT     | Hopfenname
        ColProzent,   // REAL     | Prozent [%]
        ColZeit,      // INTEGER  | Zeit [min]
        Colerg_Menge, // REAL     | Gesamtmenge [g]
        ColAlpha,     // REAL     | Alpha-Säure [%]
        ColPellets,   // BOOLEAN  | Pellets
        ColZeitpunkt, // ENUM     | Zeitpunkt der Hopfengabe [Brauhelfer::HopfenZeitpunkt] (TODO: in Datenbank 'Vorderwuerze')
        // virtual columns
        ColDeleted,   // BOOLEAN  | marked for deletion
        ColIBUAnteil, // REAL     | IBU-Beitrag der Gabe bezogen auf Sollmenge und Alpha-Ausbeute [%]
        ColAusbeute,  // REAL     | Hopfenausbeute nach Kochzeit und Stammwuerze mit Pellet- und Kochgabe-Korrektur [%]
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelHopfengaben(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
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
