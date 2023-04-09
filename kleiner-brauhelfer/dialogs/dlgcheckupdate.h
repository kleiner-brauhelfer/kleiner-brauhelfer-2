#ifndef DLGCHECKUPDATE_H
#define DLGCHECKUPDATE_H

#if QT_NETWORK_LIB

#include "dlgabstract.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

namespace Ui {
class DlgCheckUpdate;
}

class DlgCheckUpdate : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgCheckUpdate(const QString &url, const QDate& since, QWidget *parent = nullptr);
    ~DlgCheckUpdate();
    void checkForUpdate();
    bool hasUpdate() const;
    bool ignoreUpdate() const;
    bool doCheckUpdate() const;

signals:
    void checkUpdatefinished();

private slots:
    void httpFinished();

private:
    bool parseReplyGithub(const QByteArray &str);

private:
    Ui::DlgCheckUpdate *ui;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    bool mHasUpdate;
    QDate mDateSince;
};

#endif // QT_NETWORK_LIB

#endif // DLGCHECKUPDATE_H
