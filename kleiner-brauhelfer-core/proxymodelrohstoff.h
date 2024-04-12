#ifndef PROXYMODELROHSTOFF_H
#define PROXYMODELROHSTOFF_H

#include "kleiner-brauhelfer-core_global.h"
#include "proxymodel.h"

class LIB_EXPORT ProxyModelRohstoff : public ProxyModel
{
    Q_OBJECT

    Q_PROPERTY(Filter filter READ filter WRITE setFilter NOTIFY filterChanged)

public:
    enum Filter
    {
        Alle,
        Vorhanden,
        InGebrauch,
        NichtVorhanden
    };
    Q_ENUM(Filter)

public:
    ProxyModelRohstoff(QObject* parent = nullptr);
    Filter filter() const;

public slots:
    void setFilter(Filter value);

signals:
    void filterChanged(Filter value);

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
