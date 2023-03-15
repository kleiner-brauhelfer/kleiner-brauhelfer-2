#include "dlgmodule.h"
#include "ui_dlgmodule.h"
#include "settings.h"

extern Settings* gSettings;

DlgModule* DlgModule::Dialog = nullptr;

DlgModule::DlgModule(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgModule)
{
    ui->setupUi(this);
    ui->gbModuleGaerverlauf->setChecked(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
    ui->cbSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->gbModuleZusammenfassung->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusdruck));
    ui->gbModuleEtikette->setChecked(gSettings->isModuleEnabled(Settings::ModuleEtikette));
    ui->gbModuleBewertung->setChecked(gSettings->isModuleEnabled(Settings::ModuleBewertung));
    ui->gbModuleBrauuebersicht->setChecked(gSettings->isModuleEnabled(Settings::ModuleBrauuebersicht));
    ui->gbModuleAusruestung->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusruestung));
    ui->gbModuleLagerverwaltung->setChecked(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->gbModuleSpeise->setChecked(gSettings->isModuleEnabled(Settings::ModuleSpeise));
    ui->gbModuleWasseraufbereitung->setChecked(gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung));
    ui->gbModulePreiskalkulation->setChecked(gSettings->isModuleEnabled(Settings::ModulePreiskalkulation));
}

DlgModule::~DlgModule()
{
    delete ui;
}

void DlgModule::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgModule::on_gbModuleGaerverlauf_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleGaerverlauf, checked);
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
    ui->cbSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
}

void DlgModule::on_cbSchnellgaerprobe_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
}

void DlgModule::on_gbModuleZusammenfassung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusdruck, checked);
}

void DlgModule::on_gbModuleEtikette_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleEtikette, checked);
}

void DlgModule::on_gbModuleBewertung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBewertung, checked);
}

void DlgModule::on_gbModuleBrauuebersicht_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBrauuebersicht, checked);
}

void DlgModule::on_gbModuleAusruestung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusruestung, checked);
}

void DlgModule::on_gbModuleLagerverwaltung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleLagerverwaltung, checked);
}

void DlgModule::on_gbModuleSpeise_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSpeise, checked);
}

void DlgModule::on_gbModuleWasseraufbereitung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleWasseraufbereitung, checked);
}

void DlgModule::on_gbModulePreiskalkulation_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModulePreiskalkulation, checked);
}
