#include "dlgabout.h"
#include "ui_dlgabout.h"
#include <QCoreApplication>
#include "definitionen.h"

DlgAbout::DlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAbout)
{
    ui->setupUi(this);
    ui->lblMain->setText(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion());
    ui->lblQtVersion->setText("Qt " + QString(QT_VERSION_STR));
    ui->lblEntwickler->setText(ENTWICKLER);
    ui->lblHomepage->setText(generateLink(URL_HOMEPAGE));
    ui->lblSourceCode->setText(generateLink(URL_SOURCEN));
    ui->lblForum->setText(generateLink(URL_FORUM, URL_FORUM_NAME));
}

DlgAbout::~DlgAbout()
{
    delete ui;
}

QString DlgAbout::generateLink(const QString &url, const QString &name)
{
    return "<a href=\"" + url + "\">" + (name.isEmpty() ? url : name) + "</a>";
}
