#ifndef MODELMALZSCHUETTUNG_H
#define MODELMALZSCHUETTUNG_H

#include "sqltablemodel.h"

class Brauhelfer;

class ModelMalzschuettung : public SqlTableModel
{
    Q_OBJECT
public:
    ModelMalzschuettung(Brauhelfer* bh, QSqlDatabase db = QSqlDatabase());
    bool setDataExt(const QModelIndex &index, const QVariant &value) Q_DECL_OVERRIDE;
    void defaultValues(QVariantMap &values) const Q_DECL_OVERRIDE;
private slots:
    void onSudDataChanged(const QModelIndex &index);
private:
    Brauhelfer* bh;
};

#endif // MODELMALZSCHUETTUNG_H
