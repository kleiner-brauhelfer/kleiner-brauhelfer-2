#include "proxymodel.h"
#include "sqltablemodel.h"

ProxyModel::ProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mDeletedColumn(-1)
{
    setDynamicSortFilter(false);
    setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
}

void ProxyModel::setSourceModel(QAbstractItemModel *model)
{
    QAbstractItemModel *prevModel = sourceModel();
    if (prevModel)
    {
        disconnect(prevModel, &QAbstractItemModel::modelReset, this, &ProxyModel::invalidate);
        if(SqlTableModel* m = qobject_cast<SqlTableModel*>(prevModel))
            disconnect(m, &SqlTableModel::modified, this, &ProxyModel::modified);
        else if(ProxyModel* m = qobject_cast<ProxyModel*>(model))
            disconnect(m, &ProxyModel::modified, this, &ProxyModel::modified);
    }

    QSortFilterProxyModel::setSourceModel(model);
    if(SqlTableModel* m = qobject_cast<SqlTableModel*>(model))
    {
        mDeletedColumn = m->fieldIndex(QStringLiteral("deleted"));
        connect(m, &SqlTableModel::modified, this, &ProxyModel::modified);
    }
    else if(ProxyModel* m = qobject_cast<ProxyModel*>(model))
    {
        mDeletedColumn = m->fieldIndex(QStringLiteral("deleted"));
        connect(m, &ProxyModel::modified, this, &ProxyModel::modified);
    }
    else
    {
        mDeletedColumn = -1;
    }
    connect(model, &QAbstractItemModel::modelReset, this, &ProxyModel::invalidate);
}

QVariant ProxyModel::data(int row, int col, int role) const
{
    return data(index(row, col), role);
}

bool ProxyModel::setData(int row, int col, const QVariant &value, int role)
{
    return setData(index(row, col), value, role);
}

bool ProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (QSortFilterProxyModel::removeRows(row, count, parent))
    {
        invalidate();
        return true;
    }
    return false;
}

bool ProxyModel::removeRow(int arow, const QModelIndex &parent)
{
    if (QSortFilterProxyModel::removeRow(arow, parent))
    {
        invalidate();
        return true;
    }
    return false;
}

int ProxyModel::append(const QMap<int, QVariant> &values)
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
    {
        int idx = model->append(values);
        invalidate();
        return mapRowFromSource(idx);
    }
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
    {
        int idx = proxyModel->append(values);
        invalidate();
        return mapRowFromSource(idx);
    }
    return -1;
}

int ProxyModel::append(const QVariantMap &values)
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
    {
        int idx = model->append(values);
        invalidate();
        return mapRowFromSource(idx);
    }
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
    {
        int idx = proxyModel->append(values);
        invalidate();
        return mapRowFromSource(idx);
    }
    return -1;
}

bool ProxyModel::swap(int row1, int row2)
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
    {
        bool ret = model->swap(mapRowToSource(row1), mapRowToSource(row2));
        invalidate();
        return ret;
    }
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
    {
        bool ret = proxyModel->swap(mapRowToSource(row1), mapRowToSource(row2));
        invalidate();
        return ret;
    }
    return false;
}

int ProxyModel::mapRowToSource(int row) const
{
    QModelIndex idx = mapToSource(index(row, 0));
    return idx.row();
}

int ProxyModel::mapRowFromSource(int row) const
{
    QModelIndex index = mapFromSource(sourceModel()->index(row, 0));
    return index.row();
}

int ProxyModel::fieldIndex(const QString &fieldName) const
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
        return model->fieldIndex(fieldName);
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
        return proxyModel->fieldIndex(fieldName);
    return -1;
}

int ProxyModel::getRowWithValue(int col, const QVariant &value)
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
        return mapRowFromSource(model->getRowWithValue(col, value));
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
        return mapRowFromSource(proxyModel->getRowWithValue(col, value));
    return -1;
}

QVariant ProxyModel::getValueFromSameRow(int colKey, const QVariant &valueKey, int col)
{
    SqlTableModel* model = qobject_cast<SqlTableModel*>(sourceModel());
    if (model)
        return model->getValueFromSameRow(colKey, valueKey, col);
    ProxyModel* proxyModel = qobject_cast<ProxyModel*>(sourceModel());
    if (proxyModel)
        return proxyModel->getValueFromSameRow(colKey, valueKey, col);
    return QVariant();
}

void ProxyModel::setFilterString(const QString &pattern)
{
    QRegularExpression regExp(QRegularExpression::escape(pattern), QRegularExpression::CaseInsensitiveOption);
    setFilterRegularExpression(regExp);
}

int ProxyModel::sortColumn() const
{
    return QSortFilterProxyModel::sortColumn();
}

void ProxyModel::setSortColumn(int column)
{
    sort(column, sortOrder());
}

Qt::SortOrder ProxyModel::sortOrder() const
{
    return QSortFilterProxyModel::sortOrder();
}

void ProxyModel::setSortOrder(Qt::SortOrder order)
{
    sort(sortColumn(), order);
}

void ProxyModel::setFilterKeyColumns(const QList<int> &columns)
{
    mFilterColumns = columns;
}


bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool accept = true;
    if (mFilterColumns.empty())
    {
        accept = QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }
    else
    {
        accept = false;
      #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
        QRegularExpression rx = filterRegularExpression();
      #else
        QRegExp rx = filterRegExp();
      #endif
        for (int col : mFilterColumns)
        {
           QModelIndex idx = sourceModel()->index(source_row, col, source_parent);
            accept = sourceModel()->data(idx).toString().contains(rx);
            if (accept)
                break;
        }
    }
    if (accept && mDeletedColumn >= 0)
    {
        QModelIndex index = sourceModel()->index(source_row, mDeletedColumn, source_parent);
        if (index.isValid())
            accept = !sourceModel()->data(index).toBool();
    }
    return accept;
}
