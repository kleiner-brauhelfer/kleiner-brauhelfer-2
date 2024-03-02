#include "tabausruestung.h"
#include "ui_tabausruestung.h"

TabAusruestung::TabAusruestung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabAusruestung)
{
    ui->setupUi(this);
}

TabAusruestung::~TabAusruestung()
{
    delete ui;
}
