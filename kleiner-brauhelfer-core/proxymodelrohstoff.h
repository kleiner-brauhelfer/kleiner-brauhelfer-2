#ifndef PROXYMODELROHSTOFF_H
#define PROXYMODELROHSTOFF_H

#include "kleiner-brauhelfer-core_global.h"
#include "proxymodel.h"

class LIB_EXPORT ProxyModelRohstoff : public ProxyModel
{
    Q_OBJECT

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    Q_PROPERTY(Filter filter READ filter WRITE setFilter NOTIFY layoutChanged)
  #endif

public:
    enum Filter
    {
        Alle,
        Vorhanden,
        InGebrauch
    };
    Q_ENUM(Filter)

public:
    ProxyModelRohstoff(QObject* parent = nullptr);
    Filter filter() const;
    void setFilter(Filter value);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private slots:
    void onSourceModelChanged();

private:
    int mAmountColumn;
    int mInUsedColumn;
    Filter mFilter;
};

#endif // PROXYMODELROHSTOFF_H
