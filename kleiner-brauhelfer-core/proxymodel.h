#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include "kleiner-brauhelfer-core_global.h"

class LIB_EXPORT ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY layoutChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY layoutChanged)

public:
    ProxyModel(QObject* parent = nullptr);

    virtual void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    using QSortFilterProxyModel::data;
    QVariant data(int row, int col, int role = Qt::DisplayRole) const;

    using QSortFilterProxyModel::setData;
    bool setData(int row, int col, const QVariant &value, int role = Qt::EditRole);

    Q_INVOKABLE int append(const QMap<int, QVariant> &values = QMap<int, QVariant>());
    Q_INVOKABLE int append(const QVariantMap &values);

    Q_INVOKABLE bool swap(int row1, int row2);

    Q_INVOKABLE int mapRowToSource(int row) const;
    Q_INVOKABLE int mapRowFromSource(int row) const;

    Q_INVOKABLE int fieldIndex(const QString &fieldName) const;

    int getRowWithValue(int col, const QVariant &value);
    QVariant getValueFromSameRow(int colKey, const QVariant &valueKey, int col);

    Q_INVOKABLE void setFilterString(const QString &text);

    int sortColumn() const;
    void setSortColumn(int column);

    Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder order);

    void setFilterKeyColumns(const QList<int> &columns);

    QVariant min(int col) const;
    QVariant max(int col) const;
    double sum(int col) const;
    double mean(int col) const;

signals:
    void modified();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private:
    int mDeletedColumn;
    QList<int> mFilterColumns;
};

#endif // PROXYMODEL_H
