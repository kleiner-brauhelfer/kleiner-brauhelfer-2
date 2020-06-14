#include "dsvtablemodel.h"
#include <QTextStream>
#include <QFileInfo>

template<class T>
class DsvData
{
public:
    DsvData(int row = 0, int column = 0):
        mRowCount(row),
        mColumnCount(column)
    {
        if (rowCount() > 0 && columnCount() > 0)
        {
            for (int i = 0; i < rowCount(); ++i)
            {
                QList<T> rowData;
                for (int j = 0; j < columnCount(); ++j)
                    rowData.append(T());
                mData.insert(i, rowData);
            }
        }
    }

    T at(int row, int column) const
    {
        if (row >= rowCount() || column >= columnCount() || column >= mData.at(row).count())
            return T();
        return mData[row][column];
    }

    bool setValue(int row, int column, T value)
    {
        if (column >= columnCount())
        {
            mColumnCount = column + 1;
            for (int i = 0; i < rowCount(); ++i)
            {
                QList<T> rowData = mData.at(i);
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                mData[i] = rowData;
            }
        }
        if (row >= rowCount())
        {
            mRowCount = row + 1;
            while (mData.size() < rowCount())
            {
                QList<T> rowData;
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                mData.append(rowData);
            }
        }

        if (column >= mData.at(row).count())
        {
            QList<T> rowData = mData.at(row);
            while (rowData.size() < columnCount())
                rowData.append(T());
            mData[row] = rowData;
        }

        mData[row][column] = value;
        return true;
    }

    int rowCount() const
    {
        return mRowCount;
    }

    int columnCount() const
    {
        return mColumnCount;
    }

    void clear()
    {
        mRowCount = 0;
        mColumnCount = 0;
        mData.clear();
    }

    void append(const QList<T> &value)
    {
        if (value.size() > columnCount())
            mColumnCount = value.size();
        mData.append(value);
        ++mRowCount;
    }

    void insert(int i, const QList<T> &value)
    {
        if (value.size() > columnCount())
            mColumnCount = value.size();
        mData.insert(i, value);
        ++mRowCount;
    }

    void removeAt(int i)
    {
        mData.removeAt(i);
        --mRowCount;
    }

private:
    QList<QList<T> > mData;
    int mRowCount;
    int mColumnCount;
};

DsvTableModel::DsvTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    mData = new DsvData<QString>();
}

DsvTableModel::~DsvTableModel()
{
    delete mData;
}

int DsvTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData->rowCount();
}

int DsvTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData->columnCount();
}

QVariant DsvTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section >= 0 && section < mHeaderData.count())
                return mHeaderData[section];
        }
        else
        {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}

bool DsvTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && role == Qt::EditRole)
    {
        if (section >= 0 && section < mHeaderData.count())
        {
            mHeaderData[section] = value.toString();
            emit headerDataChanged(orientation, section, section);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags DsvTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QVariant DsvTableModel::data(const QModelIndex &index, int role) const
{         
    if (index.isValid() && (role == Qt::DisplayRole || role == Qt::EditRole))
    {
        return mData->at(index.row(), index.column());
    }
    return QVariant();
}

bool DsvTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        if (mData->setValue(index.row(), index.column(), value.toString()))
        {
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

bool DsvTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginInsertRows(QModelIndex(), row, row + count - 1);
    QList<QString> list;
    list.reserve(columnCount(parent));
    for (int c = 0; c < columnCount(parent); ++c)
        list.append("");
    for (int r = row; r < row + count; ++r)
        mData->insert(r, list);
    endInsertRows();
    return true;
}

bool DsvTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int r = 0; r < count; ++r)
        mData->removeAt(row);
    endRemoveRows();
    return true;
}

int DsvTableModel::fieldIndex(const QString &fieldName) const
{
    return mHeaderData.indexOf(fieldName);
}

QString DsvTableModel::fieldName(int fieldIndex) const
{
    if (fieldIndex >= 0 && fieldIndex < mHeaderData.length())
        return mHeaderData.at(fieldIndex);
    return QString();
}

void DsvTableModel::checkString(QString &value, QList<QString> &row, const QChar &character, bool &isHeaderRow)
{
    if(value.count("\"") % 2 == 0)
    {
        if (value.startsWith(QChar('\"')) && value.endsWith( QChar('\"') ) )
        {
            value.remove(0, 1);
            value.chop(1);
        }
        value.replace("\"\"", "\"");
        row.append(value);
        if (character == QChar('\n'))
        {
            if (isHeaderRow)
                mHeaderData = row;
            else
                mData->append(row);
            isHeaderRow = false;
            row.clear();
        }
        value.clear();
    }
    else
    {
        value.append(character);
    }
}

bool DsvTableModel::loadFromFile(const QString &fileName, bool hasHeaderLine, QChar delim)
{
    QFile file(fileName);

    if (delim.isNull())
    {
        QString ext = QFileInfo(file).completeSuffix().toLower();
        if (ext == "csv")
            delim = QChar(',');
        else if (ext == "tsv")
            delim = QChar('\t');
        else
            return false;
    }

    if (delim == QChar('"'))
        return false;

    if (!file.open(QFile::ReadOnly | QFile::Text))
        return false;

    QString value;
    QTextStream in(&file);
    QList<QString> row;

    in.setCodec("UTF-8");
    beginResetModel();

    bool isHeaderRow = hasHeaderLine;
    mData->clear();
    while (true)
    {
        QChar character;
        in >> character;
        if (in.atEnd())
        {
            if (character == delim)
            {
                checkString(value, row, character, isHeaderRow);
                checkString(value, row, QChar('\n'), isHeaderRow);
            }
            else
            {
                checkString(value, row, QChar('\n'), isHeaderRow);
            }
            break;
        }
        else if (character == delim || character == QChar('\n'))
        {
            checkString(value, row, character, isHeaderRow);
        }
        else
        {
            value.append(character);
        }
    }
    file.close();
    if (!hasHeaderLine)
    {
        mHeaderData.reserve(mData->columnCount());
        for (int c = 0; c < mData->columnCount(); c++)
            mHeaderData.append(QString::number(c));
    }
    endResetModel();
    return true;
}

bool DsvTableModel::save(const QString &fileName, bool withHeaderLine, QChar delim)
{
    QFile file(fileName);

    if (delim.isNull())
    {
        QString ext = QFileInfo(file).completeSuffix().toLower();
        if (ext == "csv")
            delim = QChar(',');
        else if (ext == "tsv")
            delim = QChar('\t');
        else
            return false;
    }

    if (delim == QChar('"'))
        return false;

    if (!file.open(QFile::WriteOnly | QFile::Text))
        return false;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    if (withHeaderLine)
    {
        for (int i = 0; i < mHeaderData.count(); ++i)
        {
            QString value = mHeaderData[i];
            value.replace("\"", "\"\"");
            if (value.contains("\"") || value.contains(delim) || value.contains('\r') || value.contains('\n'))
                out << "\"" << value << "\"";
            else
                out << value;
            out << delim;
        }
      #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        out << Qt::endl;
      #else
        out << endl;
      #endif
    }
    for (int r = 0; r < mData->rowCount(); ++r)
    {
        for (int c = 0; c < mData->columnCount(); ++c)
        {
            QString value = mData->at(r, c);
            value.replace("\"", "\"\"");
            if (value.contains("\"") || value.contains(delim) || value.contains('\r') || value.contains('\n'))
                out << "\"" << value << "\"";
            else
                out << value;
            if (c == mData->columnCount() - 1)
              #if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
                out << Qt::endl;
              #else
                out << endl;
              #endif
            else
                out << delim;
        }
    }
    file.close();
    return true;
}
