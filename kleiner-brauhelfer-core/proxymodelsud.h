#ifndef PROXYMODELSUD_H
#define PROXYMODELSUD_H

#include "kleiner-brauhelfer-core_global.h"
#include "proxymodel.h"

class LIB_EXPORT ProxyModelSud : public ProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool filterMerkliste READ filterMerkliste WRITE setFilterMerkliste NOTIFY filterChanged)
    Q_PROPERTY(FilterStatus filterStatus READ filterStatus WRITE setFilterStatus NOTIFY filterChanged)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterChanged)

public:
    enum FilterStatus
    {
        Alle,
        NichtGebraut,
        Gebraut,
        NichtAbgefuellt,
        GebrautNichtAbgefuellt,
        Abgefuellt,
        NichtVerbraucht,
        Verbraucht
    };
    Q_ENUMS(FilterStatus)

public:
    ProxyModelSud(QObject* parent = nullptr);

    bool filterMerkliste() const;
    void setFilterMerkliste(bool value);

    FilterStatus filterStatus() const;
    void setFilterStatus(FilterStatus state);

    QString filterText() const;
    void setFilterText(const QString& text);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private slots:
    void onSourceModelChanged();

private:
    int mColumnId;
    int mColumnSudname;
    int mColumnAuswahlHefe;
    int mColumnKommentar;
    int mColumnBierWurdeGebraut;
    int mColumnBierWurdeAbgefuellt;
    int mColumnBierWurdeVerbraucht;
    int mColumnMerklistenID;
    bool mFilterMerkliste;
    FilterStatus mFilterStatus;
    QString mFilterText;
};

#endif // PROXYMODELSUD_H
