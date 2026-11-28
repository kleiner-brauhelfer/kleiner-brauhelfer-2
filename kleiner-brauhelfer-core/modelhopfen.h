#ifndef MODELHOPFEN_H
#define MODELHOPFEN_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelHopfen : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,             // INTEGER  | ID
        ColName,           // TEXT     | Name
        ColMenge,          // REAL     | Menge [g]
        ColAlpha,          // REAL     | Alpha-Säure [%]
        ColPellets,        // BOOLEAN  | Pellets
        ColTyp,            // ENUM     | Hopfentyp [Brauhelfer::HopfenTyp]
        ColBemerkung,      // TEXT     | Bemerkung
        ColEigenschaften,  // TEXT     | Eigenschaften
        ColAlternativen,   // TEXT     | Alternativen
        ColPreis,          // REAL     | Preis [Waehrung/g]
        ColEingelagert,    // DATETIME | Einlagerungsdatum
        ColMindesthaltbar, // DATETIME | Verfallsdatum
        ColLink,           // TEXT     | Link
        // virtual columns
        ColDeleted,        // BOOLEAN  | marked for deletion
        ColInGebrauch,     // BOOLEAN  | Hopfen wird in mindestens einem Sud im Rezeptstatus verwendet
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelHopfen(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    QStringList inGebrauchListe(const QString &name) const;

private slots:
    void resetInGebrauch();
    void onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                          const QList<int> &roles = QList<int>());
    void onHopfengabenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                  const QList<int> &roles = QList<int>());

private:

    void updateInGebrauch() const;

private:

    Brauhelfer* bh;
    mutable QVector<bool> inGebrauch;
};

#endif // MODELHOPFEN_H
