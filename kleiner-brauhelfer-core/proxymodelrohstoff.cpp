#include "proxymodelrohstoff.h"
#include "sqltablemodel.h"

ProxyModelRohstoff::ProxyModelRohstoff(QObject *parent) :
    ProxyModel(parent),
    mAmountColumn(-1),
    mInUsedColumn(-1),
    mFilter(Filter::Alle)
{
    connect(this, SIGNAL(sourceModelChanged()), this, SLOT(onSourceModelChanged()));
}

void ProxyModelRohstoff::onSourceModelChanged()
{
    if(SqlTableModel* model = dynamic_cast<SqlTableModel*>(sourceModel()))
    {
        setFilterKeyColumn(model->fieldIndex("Beschreibung"));
        mAmountColumn = model->fieldIndex("Menge");
        mInUsedColumn = model->fieldIndex("InGebrauch");
    }
    else if(ProxyModel* model = dynamic_cast<ProxyModel*>(sourceModel()))
    {
        setFilterKeyColumn(model->fieldIndex("Beschreibung"));
        mAmountColumn = model->fieldIndex("Menge");
        mInUsedColumn = model->fieldIndex("InGebrauch");
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
    mFilter = value;
    invalidateFilter();
    sort();
    emit filterChanged();
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
