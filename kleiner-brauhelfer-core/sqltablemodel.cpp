#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlError>

QLoggingCategory SqlTableModel::loggingCategory("SqlTableModel", QtWarningMsg);

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    mSignalModifiedBlocked(false),
    mSetDataCnt(0)
{
    setEditStrategy(EditStrategy::OnManualSubmit);
    mVirtualField.append("deleted");
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value;
    if (role == Qt::DisplayRole || role == Qt::EditRole || role > Qt::UserRole)
    {
        int col = (role > Qt::UserRole) ? (role - Qt::UserRole - 1) : index.column();
        if (col == QSqlTableModel::columnCount())
        {
            value = headerData(index.row(), Qt::Vertical).toString() == "!";
        }
        else
        {
            const QModelIndex index2 = this->index(index.row(), col);
            value = dataExt(index2);
            if (!value.isValid())
                value = QSqlTableModel::data(index2);
        }
    }
    else
    {
        value = QSqlTableModel::data(index, role);
    }
    return value;
}

QVariant SqlTableModel::data(int row, const QString &fieldName, int role) const
{
    return data(this->index(row, fieldIndex(fieldName)), role);
}

bool SqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole || role > Qt::UserRole)
    {
        int col = (role > Qt::UserRole) ? (role - Qt::UserRole - 1) : index.column();
        if (col == QSqlTableModel::columnCount())
            return false;
        qDebug(loggingCategory) << "setData():" << tableName() << "row" << index.row() << "col" << fieldName(col) << "=" << value.toString();
        const QModelIndex index2 = this->index(index.row(), col);
        ++mSetDataCnt;
        bool ret = setDataExt(index2, value);
        if (!ret)
            ret = QSqlTableModel::setData(index2, value);
        if (ret)
        {
            if (mSetDataCnt == 1)
            {
                emit rowChanged(index);
                if (!mSignalModifiedBlocked)
                    emit modified();
            }
        }
        --mSetDataCnt;
        return ret;
    }
    return QSqlTableModel::setData(index, value, role);
}

bool SqlTableModel::setData(int row, const QString &fieldName, const QVariant &value, int role)
{
    return setData(this->index(row, fieldIndex(fieldName)), value, role);
}

bool SqlTableModel::setData(int row, const QVariantMap &values, int role)
{
    bool ret = true;
    QVariantMap::const_iterator it = values.constBegin();
    while (it != values.constEnd())
    {
        ret &= setData(row, it.key(), it.value(), role);
        ++it;
    }
    return ret;
}

QHash<int, QByteArray> SqlTableModel::roleNames() const
{
    return mRoles;
}

Qt::ItemFlags SqlTableModel::flags(const QModelIndex &index) const
{
    if (index.column() < QSqlTableModel::columnCount())
        return QSqlTableModel::flags(index);
    if (index.column() == QSqlTableModel::columnCount())
        return QSqlTableModel::flags(index);
    if (index.column() < columnCount())
        return QSqlTableModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return Qt::NoItemFlags;
}

int SqlTableModel::columnCount(const QModelIndex &index) const
{
    int columns = QSqlTableModel::columnCount(index);
    if (columns > 0)
        return columns + mVirtualField.size();
    else
        return columns;
}

int SqlTableModel::fieldIndex(const QString &fieldName) const
{
    int index = QSqlTableModel::fieldIndex(fieldName);
    if (index == -1 && columnCount() > 0)
    {
        for(int i = 0; i < mVirtualField.size(); ++i)
        {
            if (mVirtualField.at(i) == fieldName)
            {
                index = i + QSqlTableModel::columnCount();
                break;
            }
        }
    }
    return index;
}

QString SqlTableModel::fieldName(int fieldIndex) const
{
    if (fieldIndex >= 0 && fieldIndex < columnCount())
    {
        if (fieldIndex < QSqlTableModel::columnCount())
            return record().fieldName(fieldIndex);
        else
            return mVirtualField[fieldIndex - QSqlTableModel::columnCount()];
    }
    return QString();
}

void SqlTableModel::setSortByFieldName(const QString &fieldName, Qt::SortOrder order)
{
    int idx = fieldIndex(fieldName);
    if (idx != -1)
    {
        QSqlTableModel::setSort(idx, order);
    }
}

void SqlTableModel::setTable(const QString &tableName)
{
    if (tableName != this->tableName())
    {
        qInfo(loggingCategory) << "setTable():" << tableName;
        QSqlTableModel::setTable(tableName);
        if (lastError().isValid())
            qCritical(loggingCategory) << lastError();
        // hack for setHeaderData() (crash in proxy model when loading new database in App)
        beginResetModel();
        endResetModel();
        // hack end
        QSqlRecord rec = record();
        mRoles.clear();
        for(int i = 0; i < columnCount(); ++i)
        {
            QString field = fieldName(i);
            mRoles.insert(Qt::UserRole + i + 1, QVariant(field).toByteArray());
            setHeaderData(i, Qt::Horizontal, field);
        }
        emit tableChanged();
    }
}

bool SqlTableModel::select()
{
    qInfo(loggingCategory) << "select():" << tableName();
    qDebug(loggingCategory) << selectStatement();
    if (QSqlTableModel::select())
    {
        fetchAll();
        return true;
    }
    if (lastError().isValid())
        qCritical(loggingCategory) << lastError();
    return false;
}

void SqlTableModel::fetchAll()
{
    while (canFetchMore())
    {
        qInfo(loggingCategory) << "fetchMore():" << tableName();
        fetchMore();
    }
}

void SqlTableModel::setFilter(const QString &filter)
{
    if (filter != this->filter())
    {
        qInfo(loggingCategory) << "setFilter():" << tableName() << filter;
        QSqlTableModel::setFilter(filter);
        emit filterChanged();
    }
}

bool SqlTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qInfo(loggingCategory) << "removeRows():" << tableName() << row << count;
    if (QSqlTableModel::removeRows(row, count, parent))
    {
        for (int i = 0; i < count; ++i)
        {
            QModelIndex index = this->index(row + i, fieldIndex("deleted"));
            if (index.isValid())
            {
                emit dataChanged(index, index, QVector<int>());
                emit rowChanged(index);
            }
        }
        emit layoutAboutToBeChanged();
        emit layoutChanged();
        emit modified();
        return true;
    }
    if (lastError().isValid())
        qCritical(loggingCategory) << lastError();
    return false;
}

bool SqlTableModel::removeRow(int arow, const QModelIndex &parent)
{
     return QSqlTableModel::removeRow(arow, parent);
}

int SqlTableModel::append(const QVariantMap &values)
{
    qInfo(loggingCategory) << "append():" << tableName();
    QVariantMap val = values;
    defaultValues(val);
    if (insertRow(rowCount()))
    {
        int row = rowCount() - 1;
        QVariantMap::const_iterator it = val.constBegin();
        bool wasBlocked = blockSignals(true);
        while (it != val.constEnd())
        {
            QSqlTableModel::setData(index(row, fieldIndex(it.key())), it.value());
            ++it;
        }
        it = val.constBegin();
        while (it != val.constEnd())
        {
            setDataExt(index(row, fieldIndex(it.key())), it.value());
            ++it;
        }
        blockSignals(wasBlocked);
        emit modified();
        return row;
    }
    if (lastError().isValid())
        qCritical(loggingCategory) << lastError();
    return -1;
}

int SqlTableModel::appendDirect(const QVariantMap &values)
{
    qInfo(loggingCategory) << "appendDirect():" << tableName();
    if (insertRow(rowCount()))
    {
        int row = rowCount() - 1;
        QVariantMap::const_iterator it = values.constBegin();
        bool wasBlocked = blockSignals(true);
        while (it != values.constEnd())
        {
            QSqlTableModel::setData(index(row, fieldIndex(it.key())), it.value());
            ++it;
        }
        blockSignals(wasBlocked);
        emit modified();
        return row;
    }
    if (lastError().isValid())
        qCritical(loggingCategory) << lastError();
    return -1;
}

bool SqlTableModel::submitAll()
{
    qInfo(loggingCategory) << "submitAll():" << tableName();
    if (QSqlTableModel::submitAll())
    {
        emit submitted();
        emit modified();
        return true;
    }
    if (lastError().isValid())
        qCritical(loggingCategory) << lastError();
    return false;
}

void SqlTableModel::revertAll()
{
    qInfo(loggingCategory) << "revertAll():" << tableName();
    QSqlTableModel::revertAll();
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit reverted();
    emit modified();
}

int SqlTableModel::getRowWithValue(const QString &fieldName, const QVariant &value)
{
    int col = fieldIndex(fieldName);
    if (col != -1)
    {
        for (int row = 0; row < rowCount(); ++row)
        {
            if (data(this->index(row, col)) == value)
                return row;
        }
    }
    return -1;
}

QVariant SqlTableModel::getValueFromSameRow(const QString &fieldNameKey, const QVariant &valueKey, const QString &fieldName)
{
    int row = getRowWithValue(fieldNameKey, valueKey);
    if (row == -1)
        return QVariant();
    return data(row, fieldName);
}

QVariant SqlTableModel::dataExt(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QVariant();
}

bool SqlTableModel::setDataExt(const QModelIndex &index, const QVariant &value)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    return false;
}

bool SqlTableModel::isUnique(const QModelIndex &index, const QVariant &value, bool ignoreIndexRow) const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        if (!ignoreIndexRow && row == index.row())
            continue;
        if (index.sibling(row, index.column()).data() == value)
            return false;
    }
    return true;
}

QString SqlTableModel::getUniqueName(const QModelIndex &index, const QVariant &value, bool ignoreIndexRow) const
{
    int cnt = 1;
    QString name = value.toString();
    while (!isUnique(index, name, ignoreIndexRow))
        name = value.toString() + "_" + QString::number(cnt++);
    return name;
}

int SqlTableModel::getNextId() const
{
    QSqlIndex primary = primaryKey();
    if (!primary.isEmpty())
    {
        int maxId = 0;
        int colSudId = primary.value(0).toInt();
        for (int i = 0; i < rowCount(); ++i)
        {
            int sudId = index(i, colSudId).data().toInt();
            if (sudId > maxId)
                maxId = sudId;
        }
        return maxId + 1;
    }
    return 1;
}

void SqlTableModel::defaultValues(QVariantMap &values) const
{
    Q_UNUSED(values)
}

QVariantMap SqlTableModel::copyValues(int row) const
{
    QVariantMap values;
    QSqlRecord rec = record(row);
    for (int i = 0; i < rec.count(); ++i)
        values.insert(rec.fieldName(i), rec.value(i));
    QSqlIndex primary = primaryKey();
    if (!primary.isEmpty())
        values.remove(fieldName(primary.value(0).toInt()));
    return values;
}
