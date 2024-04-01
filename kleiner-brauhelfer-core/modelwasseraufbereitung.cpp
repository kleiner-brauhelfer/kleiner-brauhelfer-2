// clazy:excludeall=skipped-base-method
#include "modelwasseraufbereitung.h"
#include "proxymodel.h"
#include "brauhelfer.h"

ModelWasseraufbereitung::ModelWasseraufbereitung(Brauhelfer* bh, const QSqlDatabase &db) :
    SqlTableModel(bh, db),
    bh(bh)
{
}

bool ModelWasseraufbereitung::setDataExt(const QModelIndex &idx, const QVariant &value)
{
    switch(idx.column())
    {
    case ColMenge:
    {
        double m = value.toDouble();
        double f = data(idx.row(), ColFaktor).toDouble();
        double ra = 0;
        if (f != 0)
            ra =  m / f;
        if (m >= 0)
        {
            QSqlTableModel::setData(idx, value);
            QSqlTableModel::setData(index(idx.row(), ColRestalkalitaet), ra);
        }
        break;
    }
    case ColRestalkalitaet:
    {
        double ra = value.toDouble();
        double f = data(idx.row(), ColFaktor).toDouble();
        double m = ra * f;
        if (m >= 0)
        {
            QSqlTableModel::setData(idx, value);
            QSqlTableModel::setData(index(idx.row(), ColMenge), m);
        }
        break;
    }
    case ColFaktor:
    {
        double f = value.toDouble();
        double m = data(idx.row(), ColMenge).toDouble();
        double ra = 0;
        if (f != 0)
            ra = m / f;
        if (m >= 0)
        {
            QSqlTableModel::setData(idx, value);
            QSqlTableModel::setData(index(idx.row(), ColRestalkalitaet), ra);
        }
        break;
    }
    default:
        return false;
    }
    return true;
}

double ModelWasseraufbereitung::restalkalitaet(const QVariant& sudId) const
{
    double ra = 0.0;
    ProxyModel model;
    model.setSourceModel(const_cast<ModelWasseraufbereitung*>(this));
    model.setFilterKeyColumn(ColSudID);
    model.setFilterRegularExpression(QStringLiteral("^%1$").arg(sudId.toInt()));
    for (int r = 0; r < model.rowCount(); r++)
        ra += model.data(r, ColRestalkalitaet).toDouble();
    return ra;
}
