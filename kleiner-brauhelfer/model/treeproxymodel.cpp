#include "treeproxymodel.h"

#define ID_TOP_LEVEL -1

TreeProxyModel::TreeProxyModel(QObject* parent) :
    QAbstractItemModel(parent),
    mModel(nullptr),
    mGroupColumn(0),
    mGroupRole(Qt::DisplayRole),
    mCustomGroups(false)
{
}

void TreeProxyModel::setSourceModel(QAbstractItemModel *model)
{
    if (mModel == model)
        return;
    beginResetModel();
    if (mModel)
    {
        disconnect(mModel, &QAbstractItemModel::rowsInserted, this, &TreeProxyModel::onSourceRowsInserted);
        disconnect(mModel, &QAbstractItemModel::rowsRemoved, this, &TreeProxyModel::onSourceRowsRemoved);
        disconnect(mModel, &QAbstractItemModel::dataChanged, this, &TreeProxyModel::onSourceDataChanged);
        disconnect(mModel, &QAbstractItemModel::destroyed, this, &TreeProxyModel::onSourceDestroyed);
    }
    mModel = model;
    if (mModel)
    {
        connect(mModel, &QAbstractItemModel::rowsInserted, this, &TreeProxyModel::onSourceRowsInserted);
        connect(mModel, &QAbstractItemModel::rowsRemoved, this, &TreeProxyModel::onSourceRowsRemoved);
        connect(mModel, &QAbstractItemModel::dataChanged, this, &TreeProxyModel::onSourceDataChanged);
        connect(mModel, &QAbstractItemModel::destroyed, this, &TreeProxyModel::onSourceDestroyed);
    }
    onSourceReset();
    endResetModel();
}

QAbstractItemModel *TreeProxyModel::sourceModel() const
{
    return mModel;
}

void TreeProxyModel::setGroupping(int column, int role)
{
    setGroupColumn(column);
    setGroupRole(role);
}

void TreeProxyModel::setGroupColumn(int column)
{
    if (mGroupColumn == column)
        return;
    mGroupColumn = column;
    onSourceReset();
}

int TreeProxyModel::groupColumn() const
{
    return mGroupColumn;
}

void TreeProxyModel::setGroupRole(int role)
{
    if (mGroupRole == role)
        return;
    mGroupRole = role;
    onSourceReset();
}

int TreeProxyModel::groupRole() const
{
    return mGroupRole;
}

void TreeProxyModel::setCustomGroups(const QVector<QVariant> &groups)
{
    if (groups.empty())
    {
        mCustomGroups = false;
        onSourceReset();
    }
    else
    {
        mCustomGroups = true;
        mGroups.clear();
        for (const QVariant& g : groups)
            mGroups.emplace_back(Group(g, 0));
        for (int i = 0; i < mModel->rowCount(); ++i)
        {
            QVariant value = sourceModel()->index(i, mGroupColumn).data(mGroupRole);
            auto it = std::find(mGroups.begin(), mGroups.end(), value);
            if (it != mGroups.end())
                ++it->childCount;
        }
    }
}

QModelIndex TreeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    int id = parent.isValid() ? parent.row() : ID_TOP_LEVEL;
    return createIndex(row, column, quintptr(id));
}

QModelIndex TreeProxyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();
    if (child.internalId() == ID_TOP_LEVEL)
        return QModelIndex();
    else
        return createIndex(child.internalId(), 0, quintptr(ID_TOP_LEVEL));
}

int TreeProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return parent.internalId() == ID_TOP_LEVEL ? mGroups[parent.row()].childCount : 0;
    return mGroups.size();
}

int TreeProxyModel::columnCount(const QModelIndex &) const
{
    return mModel ? mModel->columnCount() : 0;
}

int TreeProxyModel::sourceRowForGroup(const QVariant& key, int expectedRowProxy) const
{
    for (int rowSource = 0, rowProxy = 0; rowSource < mModel->rowCount(); rowSource++)
    {
        if (mModel->index(rowSource, mGroupColumn).data(mGroupRole) == key)
        {
            if (rowProxy == expectedRowProxy)
                return rowSource;
            rowProxy++;
        }
    }
    return -1;
}

QVariant TreeProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mModel)
        return QVariant();

    if (index.internalId() == ID_TOP_LEVEL)
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (index.column() != mGroupColumn)
            return QString();

        return mGroups[index.row()].key;
    }

    const QModelIndex parent = index.parent();
    if (!parent.isValid())
        return QVariant();

    const int row = sourceRowForGroup(mGroups[parent.row()].key, index.row());
    if (row == -1)
        return QVariant();
    else
        return mModel->index(row, index.column()).data(role);
}

bool TreeProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !mModel)
        return false;

    if (index.internalId() == ID_TOP_LEVEL)
        return false;

    const QModelIndex parent = index.parent();
    if (!parent.isValid())
        return false;

    const int row = sourceRowForGroup(mGroups[parent.row()].key, index.row());
    if (row == -1)
        return false;

    return mModel->setData(mModel->index(row, index.column()), value, role);
}

QVariant TreeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (mModel)
        return mModel->headerData(section, orientation, role);
    else
        return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags TreeProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || !mModel)
        return Qt::NoItemFlags;

    if (index.internalId() == ID_TOP_LEVEL)
    {
        if (index.column() == mGroupColumn)
            return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        return Qt::NoItemFlags;
    }

    const QModelIndex parent = index.parent();
    if (!parent.isValid())
        return Qt::NoItemFlags;

    const int row = sourceRowForGroup(mGroups[parent.row()].key, index.row());
    if (row == -1)
        return Qt::NoItemFlags;

    return mModel->flags(mModel->index(row, index.column()));
}

void TreeProxyModel::onSourceRowsInserted(const QModelIndex &, int first, int last)
{
    if (!mModel)
        return;
    for (int i = first; i <= last; ++i)
    {
        QVariant value = mModel->index(i, mGroupColumn).data(mGroupRole);
        auto it = std::find(mGroups.begin(), mGroups.end(), value);
        if (it == mGroups.end() && !mCustomGroups)
            mGroups.emplace_back(Group(value, 1));
        else
            ++it->childCount;
    }
}

void TreeProxyModel::onSourceRowsRemoved(const QModelIndex &, int first, int last)
{
    if (!mModel)
        return;
    for (int i = first; i <= last; ++i)
    {
        QVariant value = mModel->index(i, mGroupColumn).data(mGroupRole);
        auto it = std::find(mGroups.begin(), mGroups.end(), value);
        if (it != mGroups.end())
            --it->childCount;
        if (it->childCount == 0 && !mCustomGroups)
            mGroups.erase(it);
    }
}

void TreeProxyModel::onSourceDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &roles)
{
    if (roles.contains(mGroupRole))
        onSourceReset();
}

void TreeProxyModel::onSourceReset()
{
    if (mCustomGroups)
        return;
    mGroups.clear();
    if (!mModel)
        return;
    for (int i = 0; i < mModel->rowCount(); ++i)
    {
        QVariant value = sourceModel()->index(i, mGroupColumn).data(mGroupRole);
        auto it = std::find(mGroups.begin(), mGroups.end(), value);
        if (it == mGroups.end())
            mGroups.emplace_back(Group(value, 1));
        else
            ++it->childCount;
    }
}

void TreeProxyModel::onSourceDestroyed()
{
    beginResetModel();
    mModel = nullptr;
    mGroups.clear();
    endResetModel();
}
