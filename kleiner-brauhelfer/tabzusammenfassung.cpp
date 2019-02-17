#include "tabzusammenfassung.h"
#include "ui_tabzusammenfassung.h"

TabZusammenfassung::TabZusammenfassung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabZusammenfassung)
{
    ui->setupUi(this);
}

TabZusammenfassung::~TabZusammenfassung()
{
    delete ui;
}

void TabZusammenfassung::saveSettings()
{

}

void TabZusammenfassung::restoreView()
{

}
