#include "tabsud.h"
#include "ui_tabsud.h"

TabSud::TabSud(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSud)
{
    ui->setupUi(this);

    ui->btnExpRezept->setChild(ui->wdgExpRezept);
    ui->btnExpRezeptStammdaten->setChild(ui->wdgExpRezeptStammdaten);
    ui->btnExpRezeptMaischen->setChild(ui->wdgExpRezeptMaischen);
    ui->btnExpRezeptMaischeplan->setChild(ui->wdgExpRezeptMaischeplan);
    ui->btnExpRezeptWuerzekochen->setChild(ui->wdgExpRezeptWuerzekochen);
    ui->btnExpRezeptGaerung->setChild(ui->wdgExpRezeptGaerung);
    ui->btnExpRezeptWasseraufbereitung->setChild(ui->wdgExpRezeptWasseraufbereitung);
    ui->btnExpBraudaten->setChild(ui->wdgExpBraudaten);
    ui->btnExpHauptgaerung->setChild(ui->wdgExpHauptgaerung);
    ui->btnExpAbfuelldaten->setChild(ui->wdgExpAbfuelldaten);
    ui->btnExpNachgaerung->setChild(ui->wdgExpNachgaerung);
    ui->btnExpTags->setChild(ui->wdgExpTags);
    ui->btnExpAnhaenge->setChild(ui->wdgExpAnhaenge);
}

TabSud::~TabSud()
{
    delete ui;
}
