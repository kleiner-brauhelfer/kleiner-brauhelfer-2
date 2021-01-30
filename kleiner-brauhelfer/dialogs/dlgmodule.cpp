#include "dlgmodule.h"
#include "ui_dlgmodule.h"

extern Settings* gSettings;
#include <QDebug>
DlgModule* DlgModule::Dialog = nullptr;

DlgModule::DlgModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgModule)
{
    ui->setupUi(this);

    ui->gbModuleGaerverlauf->setChecked(gSettings->module(Settings::ModuleGaerverlauf));
    ui->gbModuleZusammenfassung->setChecked(gSettings->module(Settings::ModuleZusammenfassung));
    ui->gbModuleEtikette->setChecked(gSettings->module(Settings::ModuleEtikette));
    ui->gbModuleBewertung->setChecked(gSettings->module(Settings::ModuleBewertung));
    ui->gbModuleBrauuebersicht->setChecked(gSettings->module(Settings::ModuleBrauuebersicht));
    ui->gbModuleAusruestung->setChecked(gSettings->module(Settings::ModuleAusruestung));
    ui->gbModuleLagerverwaltung->setChecked(gSettings->module(Settings::ModuleLagerverwaltung));
    ui->gbModuleSpeise->setChecked(gSettings->module(Settings::ModuleSpeise));
    ui->gbModuleWasseraufbereitung->setChecked(gSettings->module(Settings::ModuleWasseraufbereitung));
    ui->gbModulePreiskalkulation->setChecked(gSettings->module(Settings::ModulePreiskalkulation));
    ui->gbModuleDatenbank->setChecked(gSettings->module(Settings::ModuleDatenbank));

    adjustSize();
    gSettings->beginGroup(metaObject()->className());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();

    connect(this, &DlgModule::finished, this, []{Dialog->deleteLater();Dialog = nullptr;});
}

DlgModule::~DlgModule()
{
    gSettings->beginGroup(metaObject()->className());
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

void DlgModule::on_gbModuleGaerverlauf_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleGaerverlauf, checked);
}

void DlgModule::on_gbModuleZusammenfassung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleZusammenfassung, checked);
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

void DlgModule::on_gbModuleDatenbank_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleDatenbank, checked);
}
