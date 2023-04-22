#if QT_NETWORK_LIB

#include "dlgcheckupdate.h"
#include "ui_dlgcheckupdate.h"
#include <QJsonDocument>
#include <QJsonObject>

DlgCheckUpdate::DlgCheckUpdate(const QString &url, const QDate& since, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgCheckUpdate),
    url(url),
    mHasUpdate(false),
    mDateSince(since)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
  qnam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
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
    connect(reply, &QNetworkReply::finished, this, &DlgCheckUpdate::httpFinished);
}

bool DlgCheckUpdate::hasUpdate() const
{
    return mHasUpdate;
}

bool DlgCheckUpdate::ignoreUpdate() const
{
    return ui->checkBox_ignor->isChecked();
}

bool DlgCheckUpdate::doCheckUpdate() const
{
    return ui->cbCheckForUpdates->isChecked();
}

void DlgCheckUpdate::httpFinished()
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray byteArray = reply->readAll();
        if (url.host() == QStringLiteral("api.github.com"))
            mHasUpdate = parseReplyGithub(byteArray);
        else
            qWarning() << "DlgCheckUpdate: Unknown host:" << url.host();
    }
    else
    {
        qWarning() << "DlgCheckUpdate: Network error:" << reply->errorString();
    }
    emit checkUpdatefinished();
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

    QDate dtPublished = QDate::fromString(reply.object().value(QStringLiteral("published_at")).toString(), Qt::ISODate);
    if (dtPublished > mDateSince)
    {
        QLocale locale = QLocale();
        ui->lblName->setText(reply.object().value(QStringLiteral("name")).toString());
        ui->lblDate->setText(locale.toString(dtPublished, QLocale::ShortFormat));
      #if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        ui->textEdit->setMarkdown(reply.object().value(QStringLiteral("body")).toString());
      #else
        ui->textEdit->setText(reply.object().value("body").toString());
      #endif
        ui->lblUrl->setText("<a href=\"" + reply.object().value(QStringLiteral("html_url")).toString() + "\">Download</a>");
        return true;
    }

    return false;
}

#endif // QT_NETWORK_LIB
