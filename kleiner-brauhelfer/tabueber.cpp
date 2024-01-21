#include "tabueber.h"
#include "ui_tabueber.h"
#include "definitionen.h"

TabUeber::TabUeber(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabUeber)
{
    ui->setupUi(this);
    ui->lblVersion->setText(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion());
    ui->lblQtVersion->setText(QStringLiteral("Qt " QT_VERSION_STR));
    ui->lblEntwickler->setText(QStringLiteral(ENTWICKLER));
    ui->lblHomepage->setText(QStringLiteral("<a href=\"%1\">%2</a>").arg(URL_HOMEPAGE, URL_HOMEPAGE));
    ui->lblForum->setText(QStringLiteral("<a href=\"%1\">%2</a>").arg(URL_FORUM, URL_FORUM_NAME));
}

TabUeber::~TabUeber()
{
    delete ui;
}
