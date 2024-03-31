#include "tabausruestung.h"
#include "ui_tabausruestung.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabAusruestung::TabAusruestung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabAusruestung)
{
    ui->setupUi(this);
}

TabAusruestung::~TabAusruestung()
{
    saveSettings();
    delete ui;
}

void TabAusruestung::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabAusruestung::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabAusruestung::restoreView()
{
}
