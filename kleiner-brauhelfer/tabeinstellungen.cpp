#include "tabeinstellungen.h"
#include "ui_tabeinstellungen.h"
#include <QFileDialog>
#include "settings.h"

extern Settings* gSettings;

TabEinstellungen::TabEinstellungen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabEinstellungen)
{
    ui->setupUi(this);
    updateValue();

    connect(ui->btnRestoreView, &QAbstractButton::clicked, this, &TabEinstellungen::restoreView);
    connect(ui->btnCheckUpdate, &QAbstractButton::clicked, this, &TabEinstellungen::checkUpdate);
}

TabEinstellungen::~TabEinstellungen()
{
    delete ui;
}

void TabEinstellungen::updateValue()
{
    ui->tbDatabase->setText(gSettings->databasePath());

    switch (gSettings->theme())
    {
    case Settings::Theme::Light:
        ui->cbTheme->setCurrentIndex(0);
        break;
    case Settings::Theme::Dark:
        ui->cbTheme->setCurrentIndex(1);
        break;
    default:
        break;
    }

    QString language = gSettings->language();
    if (language == QStringLiteral("de"))
        ui->cbLanguage->setCurrentIndex(0);
    if (language == QStringLiteral("en"))
        ui->cbLanguage->setCurrentIndex(1);
    if (language == QStringLiteral("sv"))
        ui->cbLanguage->setCurrentIndex(2);
    if (language == QStringLiteral("nl"))
        ui->cbLanguage->setCurrentIndex(3);

    gSettings->beginGroup("General");
    ui->cbNumberFormat->setChecked(gSettings->value("UseLanguageLocale", true).toBool());
    ui->cbShowTooltips->setChecked(gSettings->value("TooltipsEnabled", true).toBool());
    ui->cbShowAnimations->setChecked(gSettings->value("Animations", true).toBool());
    ui->cbConfirmClose->setChecked(gSettings->value("BeendenAbfrage", true).toBool());
    ui->cbCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
    gSettings->endGroup();

    ui->cbModuleGaerverlauf->setChecked(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
    ui->cbModuleSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->cbModuleAusdruck->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusdruck));
    ui->cbModuleEtiketten->setChecked(gSettings->isModuleEnabled(Settings::ModuleEtikette));
    ui->cbModuleBewertungen->setChecked(gSettings->isModuleEnabled(Settings::ModuleBewertung));
    ui->cbModuleBrauuebersicht->setChecked(gSettings->isModuleEnabled(Settings::ModuleBrauuebersicht));
    ui->cbModuleAnlagen->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusruestung));
    ui->cbModuleLager->setChecked(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->cbModuleSpeise->setChecked(gSettings->isModuleEnabled(Settings::ModuleSpeise));
    ui->cbModuleWasseraufbereitung->setChecked(gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung));
    ui->cbModulePreiskalkulation->setChecked(gSettings->isModuleEnabled(Settings::ModulePreiskalkulation));

    ui->cbModuleSchnellgaerprobe->setEnabled(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
}

void TabEinstellungen::on_btnDatabase_clicked()
{
    QString databasePath = QFileDialog::getOpenFileName(this, tr("Datenbank auswählen"),
                                                       gSettings->databasePath(),
                                                       tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
    if (!databasePath.isEmpty())
    {
        ui->tbDatabase->setText(databasePath);
        gSettings->setDatabasePath(databasePath);
        emit requestRestart(1000);
    }
}

void TabEinstellungen::on_cbTheme_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        gSettings->setTheme(Settings::Theme::Light);
        emit requestRestart(1000);
        break;
    case 1:
        gSettings->setTheme(Settings::Theme::Dark);
        emit requestRestart(1000);
        break;
    }
}

void TabEinstellungen::on_cbLanguage_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        gSettings->setLanguage(QStringLiteral("de"));
        emit requestRestart(1001);
        break;
    case 1:
        gSettings->setLanguage(QStringLiteral("en"));
        emit requestRestart(1001);
        break;
    case 2:
        gSettings->setLanguage(QStringLiteral("sv"));
        emit requestRestart(1001);
        break;
    case 3:
        gSettings->setLanguage(QStringLiteral("nl"));
        emit requestRestart(1001);
        break;
    }
}

void TabEinstellungen::on_cbNumberFormat_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "UseLanguageLocale", checked);
    emit requestRestart(1001);
}

void TabEinstellungen::on_cbShowTooltips_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "TooltipsEnabled", checked);
}

void TabEinstellungen::on_cbShowAnimations_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "Animations", checked);
}

void TabEinstellungen::on_cbConfirmClose_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "BeendenAbfrage", checked);
}

void TabEinstellungen::on_cbCheckUpdate_toggled(bool checked)
{
    gSettings->setValueInGroup("General", "CheckUpdate", checked);
}

void TabEinstellungen::on_cbModuleGaerverlauf_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleGaerverlauf, checked);
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
    ui->cbModuleSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->cbModuleSchnellgaerprobe->setEnabled(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
}

void TabEinstellungen::on_cbModuleSchnellgaerprobe_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
}

void TabEinstellungen::on_cbModuleAusdruck_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusdruck, checked);
}

void TabEinstellungen::on_cbModuleEtiketten_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleEtikette, checked);
}

void TabEinstellungen::on_cbModuleBewertungen_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBewertung, checked);
}

void TabEinstellungen::on_cbModuleBrauuebersicht_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBrauuebersicht, checked);
}

void TabEinstellungen::on_cbModuleAnlagen_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusruestung, checked);
}

void TabEinstellungen::on_cbModuleLager_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleLagerverwaltung, checked);
}

void TabEinstellungen::on_cbModuleSpeise_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSpeise, checked);
}

void TabEinstellungen::on_cbModuleWasseraufbereitung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleWasseraufbereitung, checked);
}

void TabEinstellungen::on_cbModulePreiskalkulation_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModulePreiskalkulation, checked);
}
