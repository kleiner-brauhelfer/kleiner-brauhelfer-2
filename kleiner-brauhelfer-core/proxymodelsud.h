#ifndef PROXYMODELSUD_H
#define PROXYMODELSUD_H

#include "kleiner-brauhelfer-core_global.h"
#include "proxymodel.h"
#include <QFlag>

class LIB_EXPORT ProxyModelSud : public ProxyModel
{
    Q_OBJECT

  #if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
    Q_PROPERTY(bool filterMerkliste READ filterMerkliste WRITE setFilterMerkliste NOTIFY layoutChanged)
    Q_PROPERTY(FilterStatus filterStatus READ filterStatus WRITE setFilterStatus NOTIFY layoutChanged)
    Q_PROPERTY(QDateTime minDate READ filterMinimumDate WRITE setFilterMinimumDate NOTIFY layoutChanged)
    Q_PROPERTY(QDateTime maxDate READ filterMaximumDate WRITE setFilterMaximumDate NOTIFY layoutChanged)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY layoutChanged)
  #endif

public:
    enum FilterStatusPart
    {
        Keine = 0x00,
        Rezept = 0x01,
        Gebraut = 0x02,
        Abgefuellt = 0x04,
        Verbraucht = 0x08,
        Alle = Rezept | Gebraut | Abgefuellt | Verbraucht
    };
    Q_DECLARE_FLAGS(FilterStatus, FilterStatusPart)
    Q_FLAG(FilterStatus)

public:
    ProxyModelSud(QObject* parent = nullptr);

    bool filterMerkliste() const;
    void setFilterMerkliste(bool value);

    FilterStatus filterStatus() const;
    void setFilterStatus(FilterStatus status);

    QDateTime filterMinimumDate() const;
    void setFilterMinimumDate(const QDateTime &dt = QDateTime());

    QDateTime filterMaximumDate() const;
    void setFilterMaximumDate(const QDateTime &dt = QDateTime());

    void setFilterDate(const QDateTime &min = QDateTime(), const QDateTime &max = QDateTime());

    QString filterText() const;
    void setFilterText(const QString& text);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private:
    bool dateInRange(const QDateTime &dt) const;

private:
    bool mFilterMerkliste;
    FilterStatus mFilterStatus;
    QDateTime mMinDate;
    QDateTime mMaxDate;
    QString mFilterText;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ProxyModelSud::FilterStatus)

#endif // PROXYMODELSUD_H
