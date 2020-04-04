#include "obrama.h"
#include <QtNetwork>
#include <QFileInfo>
#include <QMessageBox>

QMap<QString, QDateTime> OBraMa::mUpdateDates = QMap<QString, QDateTime>();

OBraMa::OBraMa(QObject *parent) :
    QObject(parent),
    mUrl("https://obrama.mueggelland.de/api/obrama.php?table=%1&format=%2")
{
    mNetManager.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

QByteArray OBraMa::download(const QString& table, const QString& format)
{
    QByteArray data;
    QEventLoop loop;
    QString url = mUrl.arg(table).arg(format);
    QNetworkRequest request(url);
    QNetworkReply *reply = mNetManager.get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->error() == QNetworkReply::NoError)
        data = reply->readAll();
    delete reply;
    if (data.isEmpty())
    {
        qWarning() << "oBraMa: Failed to download:" << url;
        QMessageBox::critical(nullptr, tr("oBraMa Datenbank"), tr("Die Tabelle \"%1\" konnte nicht heruntergeladen werden.").arg(table));
    }
    return data;
}

bool OBraMa::getUpdateDates()
{
    QByteArray data = download("updates", "json");
    if (data.isEmpty())
        return false;

    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(data, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        qWarning("oBraMa: Failed to parse JSON");
        return false;
    }

    QJsonObject obj = json.array().at(0).toObject();
    mUpdateDates.clear();
    for (QJsonObject::const_iterator it = obj.begin(); it != obj.end(); it++)
    {
        QDateTime dt = QDateTime::fromString(it.value().toString(), Qt::ISODate);
        mUpdateDates.insert(it.key(), dt);
    }
    return true;
}

bool OBraMa::getTable(const QString& table, const QString& suffix, const QString& fileName)
{
    if (mUpdateDates.empty())
    {
        if (!getUpdateDates())
        {
           QMessageBox::warning(nullptr, tr("oBraMa Datenbank"), tr("Die Tabelle \"%1\" konnte nicht aktualisiert werden.").arg(table));
        }
    }

    QFile file(fileName);
    QFileInfo fileInfo = QFileInfo(file);
    if (file.exists())
    {
        auto it = mUpdateDates.find(table);
        if (it == mUpdateDates.end())
            return false;
        if (fileInfo.lastModified() >= it.value())
            return true;
    }

    QByteArray data = download(table + suffix, "csv");
    if (isValid(data))
    {
        QDir().mkpath(fileInfo.path());
        if (file.open(QFile::ReadWrite))
        {
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
            file.write(data);
            file.close();
            return true;
        }
    }
    else
    {
        qWarning() << "oBraMa: Failed to download:" << table;
        QMessageBox::critical(nullptr, tr("oBraMa Datenbank"), tr("Die Tabelle \"%1\" konnte nicht heruntergeladen werden.").arg(table));
    }

    return false;
}

bool OBraMa::isValid(const QByteArray& data) const
{
    return !data.isEmpty() && data.contains(',');
}
