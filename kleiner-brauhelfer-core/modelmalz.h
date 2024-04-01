#ifndef MODELMALZ_H
#define MODELMALZ_H

#include "kleiner-brauhelfer-core_global.h"
#include "sqltablemodel.h"

class Brauhelfer;

class LIB_EXPORT ModelMalz : public SqlTableModel
{
    Q_OBJECT

public:

    enum Column
    {
        ColID,
        ColName,
        ColMenge,
        ColPotential,
        ColFarbe,
        ColpH,
        ColMaxProzent,
        ColBemerkung,
        ColEigenschaften,
        ColAlternativen,
        ColPreis,
        ColEingelagert,
        ColMindesthaltbar,
        ColLink,
        // virtual
        ColDeleted,
        ColInGebrauch,
        // number of columns
        NumCols
    };
    Q_ENUM(Column)

public:

    ModelMalz(Brauhelfer* bh, const QSqlDatabase &db = QSqlDatabase());
    QVariant dataExt(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QMap<int, QVariant> &values) const Q_DECL_OVERRIDE;
    QStringList inGebrauchListe(const QString &name) const;

private slots:
    void resetInGebrauch();
    void onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                          const QList<int> &roles = QList<int>());
    void onMalzschuettungDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                                     const QList<int> &roles = QList<int>());

private:

    void updateInGebrauch() const;

private:

    Brauhelfer* bh;
    mutable QVector<bool> inGebrauch;
};

#endif // MODELMALZ_H
