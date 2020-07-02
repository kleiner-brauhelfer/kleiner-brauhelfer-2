#include "dlgabout.h"
#include "ui_dlgabout.h"
#include <QCoreApplication>
#include "definitionen.h"
#include "settings.h"

extern Settings* gSettings;

DlgAbout::DlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAbout)
{
    ui->setupUi(this);
    ui->lblMain->setText(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion());
    ui->lblQtVersion->setText("Qt " + QString(QT_VERSION_STR));
    ui->lblEntwickler->setText(ENTWICKLER);
    ui->lblHomepage->setText(generateLink(URL_HOMEPAGE));
    ui->lblForum->setText(generateLink(URL_FORUM, URL_FORUM_NAME));
    adjustSize();
    gSettings->beginGroup("DlgAbout");
    resize(gSettings->value("size").toSize());
    gSettings->endGroup();
}

DlgAbout::~DlgAbout()
{
    gSettings->beginGroup("DlgAbout");
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

QString DlgAbout::generateLink(const QString &url, const QString &name)
{
    return "<a href=\"" + url + "\">" + (name.isEmpty() ? url : name) + "</a>";
}
