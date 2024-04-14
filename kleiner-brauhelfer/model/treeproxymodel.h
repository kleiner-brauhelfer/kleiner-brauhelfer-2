#ifndef TREEPROXYMODEL_H
#define TREEPROXYMODEL_H

#include <QAbstractItemModel>

class TreeProxyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeProxyModel(QObject* parent = nullptr);
    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;
    void setGroupping(int column, int role);
    void setGroupColumn(int column);
    int groupColumn() const;
    void setGroupRole(int role);
    int groupRole() const;
    void setCustomGroups(const QVector<QVariant> &groups);

public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onSourceRowsInserted(const QModelIndex &parent, int first, int last);
    void onSourceRowsRemoved(const QModelIndex &parent, int first, int last);
    void onSourceDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &roles);
    void onSourceReset();
    void onSourceDestroyed();

protected:
    bool isTopLevel(const QModelIndex &index) const;

private:
    int sourceRowForGroup(const QVariant& key, int expectedRowProxy) const;

private:
    struct Group
    {
        QVariant key;
        int childCount;

        Group(const QVariant& k, int c) : key(k), childCount(c) {}

        bool operator ==(const QVariant& value) {
            return key == value;
        }

        bool operator !=(const QVariant& value) {
            return key != value;
        }
    };

private:
    QAbstractItemModel* mModel;
    int mGroupColumn;
    int mGroupRole;
    bool mCustomGroups;
    QVector<Group> mGroups;
};

#endif // TREEPROXYMODEL_H
