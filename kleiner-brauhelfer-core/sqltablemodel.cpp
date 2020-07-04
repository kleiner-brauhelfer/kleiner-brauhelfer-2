#include "sqltablemodel.h"
#include <QSqlRecord>
#include <QSqlIndex>
#include <QSqlError>

QLoggingCategory SqlTableModel::loggingCategory("SqlTableModel", QtWarningMsg);

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    mSignalModifiedBlocked(false),
    mRoles(QHash<int, QByteArray>()),
    mSetDataCnt(0)
{
    setEditStrategy(EditStrategy::OnManualSubmit);
    mVirtualField.append("deleted");
}

QVariant SqlTableModel::data(const QModelIndex &idx, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole || role > Qt::UserRole)
    {
        int col = (role > Qt::UserRole) ? (role - Qt::UserRole - 1) : idx.column();
        if (col == QSqlTableModel::columnCount())
        {
            return headerData(idx.row(), Qt::Vertical).toString() == "!";
        }
        else
        {
            const QModelIndex idx2 = index(idx.row(), col);
            QVariant value = dataExt(idx2);
            if (!value.isValid())
                value = QSqlTableModel::data(idx2);
            return value;
        }
    }
    return QSqlTableModel::data(idx, role);
}

QVariant SqlTableModel::data(int row, int col, int role) const
{
    return data(index(row, col), role);
}

bool SqlTableModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (role == Qt::DisplayRole || role == Qt::EditRole || role > Qt::UserRole)
    {
        int col = (role > Qt::UserRole) ? (role - Qt::UserRole - 1) : idx.column();
        if (col == QSqlTableModel::columnCount())
            return false;
        qDebug(loggingCategory) << "setData():" << tableName() << "row" << idx.row() << "col" << fieldName(col) << "=" << value.toString();
        ++mSetDataCnt;
        const QModelIndex idx2 = index(idx.row(), col);
        bool ret = setDataExt(idx2, value);
        if (!ret)
            ret = QSqlTableModel::setData(idx2, value);
        if (ret && mSetDataCnt == 1)
        {
            emit rowChanged(idx);
            if (!mSignalModifiedBlocked)
                emit modified();
        }
        --mSetDataCnt;
        return ret;
    }
    return QSqlTableModel::setData(idx, value, role);
}

bool SqlTableModel::setData(int row, int col, const QVariant &value, int role)
{
    return setData(index(row, col), value, role);
}

bool SqlTableModel::setData(int row, const QMap<int, QVariant> &values, int role)
{
    bool ret = true;
    QMap<int, QVariant>::const_iterator it = values.constBegin();
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
            QModelIndex idx = index(row + i, fieldIndex("deleted"));
            if (idx.isValid())
            {
                emit dataChanged(idx, idx, QVector<int>());
                emit rowChanged(idx);
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

int SqlTableModel::append(const QMap<int, QVariant> &values)
{
    qInfo(loggingCategory) << "append():" << tableName();
    QMap<int, QVariant> val = values;
    defaultValues(val);
    if (insertRow(rowCount()))
    {
        int row = rowCount() - 1;
        QMap<int, QVariant>::const_iterator it = val.constBegin();
        bool wasBlocked = blockSignals(true);
        while (it != val.constEnd())
        {
            QSqlTableModel::setData(index(row, it.key()), it.value());
            ++it;
        }
        it = val.constBegin();
        while (it != val.constEnd())
        {
            setDataExt(index(row, it.key()), it.value());
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

int SqlTableModel::append(const QVariantMap &values)
{
    QMap<int, QVariant> val;
    for (QVariantMap::const_iterator it = values.constBegin(); it != values.constEnd(); it++)
    {
        int col = fieldIndex(it.key());
        if (col >= 0)
            val.insert(col, it.value());
    }
    return append(val);
}

int SqlTableModel::appendDirect(const QMap<int, QVariant> &values)
{
    qInfo(loggingCategory) << "appendDirect():" << tableName();
    if (insertRow(rowCount()))
    {
        int row = rowCount() - 1;
        QMap<int, QVariant>::const_iterator it = values.constBegin();
        bool wasBlocked = blockSignals(true);
        while (it != values.constEnd())
        {
            QSqlTableModel::setData(index(row, it.key()), it.value());
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
    QMap<int, QVariant> val;
    for (QVariantMap::const_iterator it = values.constBegin(); it != values.constEnd(); it++)
    {
        int col = fieldIndex(it.key());
        if (col >= 0)
            val.insert(col, it.value());
    }
    return appendDirect(val);
}

bool SqlTableModel::swap(int row1, int row2)
{
    if (row1 >= 0 && row1 < rowCount() && row2 >= 0 && row2 < rowCount())
    {
        QMap<int, QVariant> values1 = copyValues(row1);
        QMap<int, QVariant> values2 = copyValues(row2);
        QMap<int, QVariant>::const_iterator it = values2.constBegin();
        while (it != values2.constEnd())
        {
            QSqlTableModel::setData(index(row1, it.key()), it.value());
            ++it;
        }
        it = values1.constBegin();
        while (it != values1.constEnd())
        {
            QSqlTableModel::setData(index(row2, it.key()), it.value());
            ++it;
        }
        emit modified();
        return true;
    }
    return false;
}

void SqlTableModel::emitModified()
{
    emit modified();
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

int SqlTableModel::getRowWithValue(int col, const QVariant &value) const
{
    if (col != -1)
    {
        for (int row = 0; row < rowCount(); ++row)
        {
            if (data(row, col) == value && !data(row, fieldIndex("deleted")).toBool())
                return row;
        }
    }
    return -1;
}

QVariant SqlTableModel::getValueFromSameRow(int colKey, const QVariant &valueKey, int col) const
{
    int row = getRowWithValue(colKey, valueKey);
    if (row == -1)
        return QVariant();
    return data(row, col);
}

QVariant SqlTableModel::dataExt(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QVariant();
}

bool SqlTableModel::setDataExt(const QModelIndex &index, const QVariant &value)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    return false;
}

bool SqlTableModel::isUnique(const QModelIndex &index, const QVariant &value, bool ignoreIndexRow) const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        if (!ignoreIndexRow && row == index.row())
            continue;
        if (data(row, fieldIndex("deleted")).toBool())
            continue;
        if (data(row, index.column()) == value)
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
        int colId = primary.value(0).toInt();
        for (int row = 0; row < rowCount(); ++row)
        {
            int sudId = index(row, colId).data().toInt();
            if (sudId > maxId)
                maxId = sudId;
        }
        return maxId + 1;
    }
    return 1;
}

void SqlTableModel::defaultValues(QMap<int, QVariant> &values) const
{
    Q_UNUSED(values)
}

QMap<int, QVariant> SqlTableModel::copyValues(int row) const
{
    QMap<int, QVariant> values;
    int colPrimary = -1;
    QSqlIndex primary = primaryKey();
    if (!primary.isEmpty())
        colPrimary = primary.value(0).toInt();
    QSqlRecord rec = record(row);
    for (int i = 0; i < rec.count(); ++i)
        if (i != colPrimary)
            values.insert(i, rec.value(i));
    return values;
}

QVariantMap SqlTableModel::toVariantMap(int row, QList<int> ignoreCols) const
{
    QVariantMap map;
    QSqlRecord rec = record(row);
    for (int i = 0; i < rec.count(); ++i)
    {
        if (ignoreCols.contains(i))
            continue;
        map.insert(rec.fieldName(i), rec.value(i));
    }
    return map;
}
