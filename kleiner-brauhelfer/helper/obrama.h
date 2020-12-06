#ifndef OBRAMA_H
#define OBRAMA_H

#if QT_NETWORK_LIB

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>
#include <QDateTime>

class OBraMa : public QObject
{
    Q_OBJECT
public:
    explicit OBraMa(QObject *parent = nullptr);
    bool getUpdateDates();
    bool getTable(const QString& table, const QString& suffix, const QString& fileName);
private:
    QByteArray download(const QString &table, const QString &format);
    bool isValid(const QByteArray& data) const;
private:
    const QString mUrl;
    QNetworkAccessManager mNetManager;
    static QMap<QString, QDateTime> mUpdateDates;
};

#endif // QT_NETWORK_LIB

#endif // OBRAMA_H
