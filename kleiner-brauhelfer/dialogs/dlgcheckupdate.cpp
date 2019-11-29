#include "dlgcheckupdate.h"
#include "ui_dlgcheckupdate.h"
#include <QJsonDocument>
#include <QJsonObject>

DlgCheckUpdate::DlgCheckUpdate(const QString &url, const QDate& since, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCheckUpdate),
    url(url),
    mHasUpdate(false),
    mDateSince(since)
{
    ui->setupUi(this);
    adjustSize();
}

DlgCheckUpdate::~DlgCheckUpdate()
{
    delete ui;
}

void DlgCheckUpdate::checkForUpdate()
{
    mHasUpdate = false;
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}

bool DlgCheckUpdate::hasUpdate() const
{
    return mHasUpdate;
}

bool DlgCheckUpdate::ignoreUpdate() const
{
    return ui->checkBox_ignor->isChecked();
}

void DlgCheckUpdate::httpFinished()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray byteArray = reply->readAll();
        if (url.host() == "api.github.com")
            mHasUpdate = parseReplyGithub(byteArray);
        else
            qWarning() << "DlgCheckUpdate: Unknown host:" << url.host();
    }
    else
    {
        qWarning() << "DlgCheckUpdate: Network error:" << reply->error();
    }
    emit finished();
}

bool DlgCheckUpdate::parseReplyGithub(const QByteArray& str)
{
    QJsonParseError jsonError;
    QJsonDocument reply = QJsonDocument::fromJson(str, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        qWarning("DlgCheckUpdate: Failed to parse JSON");
        return false;
    }

    QDate dtPublished = QDate::fromString(reply["published_at"].toString(), Qt::ISODate);
    if (dtPublished > mDateSince)
    {
        QLocale locale = QLocale();
        ui->lblName->setText(reply["name"].toString());
        ui->lblDate->setText(locale.toString(dtPublished, QLocale::ShortFormat));
        ui->textEdit->setText(reply["body"].toString());
        ui->lblUrl->setText("<a href=\"" + reply["html_url"].toString() + "\">Download</a>");
        return true;
    }

    return false;
}
