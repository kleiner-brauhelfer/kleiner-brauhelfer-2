#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDateTime>
#include "kleiner-brauhelfer-core_global.h"

class LIB_EXPORT ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY layoutChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY layoutChanged)
    Q_PROPERTY(int dateColumn READ filterDateColumn WRITE setFilterDateColumn NOTIFY layoutChanged)
    Q_PROPERTY(QDateTime minDate READ filterMinimumDate WRITE setFilterMinimumDate NOTIFY layoutChanged)
    Q_PROPERTY(QDateTime maxDate READ filterMaximumDate WRITE setFilterMaximumDate NOTIFY layoutChanged)
  #endif

public:
    ProxyModel(QObject* parent = nullptr);

    virtual void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    using QSortFilterProxyModel::data;
    QVariant data(int row, int col, int role = Qt::DisplayRole) const;

    using QSortFilterProxyModel::setData;
    bool setData(int row, int col, const QVariant &value, int role = Qt::EditRole);

    Q_INVOKABLE bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool removeRow(int arow, const QModelIndex &parent = QModelIndex());

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

    int filterDateColumn() const;
    void setFilterDateColumn(int column);

    QDateTime filterMinimumDate() const;
    void setFilterMinimumDate(const QDateTime &dt);

    QDateTime filterMaximumDate() const;
    void setFilterMaximumDate(const QDateTime &dt);

signals:
    void modified();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private:
    bool dateInRange(const QDateTime &dt) const;

private:
    int mDeletedColumn;
    int mDateColumn;
    QList<int> mFilterColumns;
    QDateTime mMinDate;
    QDateTime mMaxDate;
};

#endif // PROXYMODEL_H
