#include "tablager.h"
#include "ui_tablager.h"

TabLager::TabLager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabLager)
{
    ui->setupUi(this);
}

TabLager::~TabLager()
{
    delete ui;
}
