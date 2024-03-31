#include "tabsude.h"
#include "ui_tabsude.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabSude::TabSude(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSude)
{
    ui->setupUi(this);
}

TabSude::~TabSude()
{
    saveSettings();
    delete ui;
}

void TabSude::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabSude::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabSude::restoreView()
{
}
