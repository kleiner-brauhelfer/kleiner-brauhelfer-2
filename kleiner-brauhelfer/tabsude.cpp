#include "tabsude.h"
#include "ui_tabsude.h"

TabSude::TabSude(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSude)
{
    ui->setupUi(this);
}

TabSude::~TabSude()
{
    delete ui;
}
