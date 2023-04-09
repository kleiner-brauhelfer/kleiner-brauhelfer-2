// clazy:excludeall=skipped-base-method
#include "modelkategorien.h"
#include "brauhelfer.h"

ModelKategorien::ModelKategorien(Brauhelfer *bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

bool ModelKategorien::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColName:
    {
        QString name = getUniqueName(idx, value);
        QVariant prevName = data(idx);
        if (QSqlTableModel::setData(idx, name))
        {
            if (!prevName.toString().isEmpty())
            {
                for (int r = 0; r < bh->modelSud()->rowCount(); ++r)
                {
                    if (bh->modelSud()->data(r, ModelSud::ColKategorie) == prevName)
                        bh->modelSud()->setData(r, ModelSud::ColKategorie, name);
                }
            }
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

void ModelKategorien::defaultValues(QMap<int, QVariant> &values) const
{
    values[ColName] = getUniqueName(index(0, ColName), values[ColName], true);
}
