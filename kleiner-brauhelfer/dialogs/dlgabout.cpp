#include "dlgabout.h"
#include "ui_dlgabout.h"
#include <QCoreApplication>
#include "definitionen.h"

DlgAbout::DlgAbout(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgAbout)
{
    ui->setupUi(this);
    ui->lblMain->setText(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion());
    ui->lblQtVersion->setText(QStringLiteral("Qt " QT_VERSION_STR));
    ui->lblEntwickler->setText(QStringLiteral(ENTWICKLER));
    ui->lblHomepage->setText(generateLink(QStringLiteral(URL_HOMEPAGE)));
    ui->lblForum->setText(generateLink(QStringLiteral(URL_FORUM), QStringLiteral(URL_FORUM_NAME)));
    adjustSize();
}

DlgAbout::~DlgAbout()
{
    delete ui;
}

QString DlgAbout::generateLink(const QString &url, const QString &name)
{
    return "<a href=\"" + url + "\">" + (name.isEmpty() ? url : name) + "</a>";
}
