#include "proxymodelrohstoff.h"
#include "sqltablemodel.h"

ProxyModelRohstoff::ProxyModelRohstoff(QObject *parent) :
    ProxyModel(parent),
    mAmountColumn(-1),
    mInUsedColumn(-1),
    mFilter(Filter::Alle)
{
    connect(this, &ProxyModelRohstoff::sourceModelChanged, this, &ProxyModelRohstoff::onSourceModelChanged);
}

void ProxyModelRohstoff::onSourceModelChanged()
{
    if(SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel()))
    {
        setFilterKeyColumns({model->fieldIndex("Name"),
                             model->fieldIndex("Bemerkung"),
                             model->fieldIndex("Eigenschaften")});
        mAmountColumn = model->fieldIndex(QStringLiteral("Menge"));
        mInUsedColumn = model->fieldIndex(QStringLiteral("InGebrauch"));
    }
    else if(ProxyModel* model = qobject_cast<ProxyModel*>(sourceModel()))
    {
        setFilterKeyColumns({model->fieldIndex("Name"),
                             model->fieldIndex("Bemerkung"),
                             model->fieldIndex("Eigenschaften")});
        mAmountColumn = model->fieldIndex(QStringLiteral("Menge"));
        mInUsedColumn = model->fieldIndex(QStringLiteral("InGebrauch"));
    }
    else
    {
        mAmountColumn = -1;
        mInUsedColumn = -1;
    }
}

ProxyModelRohstoff::Filter ProxyModelRohstoff::filter() const
{
    return mFilter;
}

void ProxyModelRohstoff::setFilter(Filter value)
{
    if (mFilter != value)
    {
        mFilter = value;
        invalidate();
    }
}

bool ProxyModelRohstoff::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
    if (accept && mFilter == Filter::Vorhanden)
    {
        QModelIndex index = sourceModel()->index(source_row, mAmountColumn, source_parent);
        if (index.isValid())
            accept = sourceModel()->data(index).toDouble() > 0.0;
    }
    if (accept && mFilter == Filter::InGebrauch)
    {
        QModelIndex index = sourceModel()->index(source_row, mInUsedColumn, source_parent);
        if (index.isValid())
            accept = sourceModel()->data(index).toBool();
    }
    return accept;
}
