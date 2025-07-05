#include "dlgeinstellungen.h"
#include "ui_dlgeinstellungen.h"
#include <QApplication>
#include <QFileDialog>
#include <QFontDialog>
#include "settings.h"

extern Settings* gSettings;

DlgEinstellungen* DlgEinstellungen::Dialog = nullptr;

DlgEinstellungen::DlgEinstellungen(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgEinstellungen)
{
    ui->setupUi(this);
    updateValues();

    connect(ui->btnRestoreView, &QAbstractButton::clicked, this, &DlgEinstellungen::restoreViewTriggered);
    connect(ui->btnCheckUpdate, &QAbstractButton::clicked, this, [this](){emit checkUpdateTriggered(true);});
}

DlgEinstellungen::~DlgEinstellungen()
{
    delete ui;
}

void DlgEinstellungen::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className(), Dialog);
}

void DlgEinstellungen::updateValues()
{
    ui->tbDatabase->setText(gSettings->databasePath());

    switch (gSettings->theme())
    {
    case Qt::ColorScheme::Light:
        ui->cbTheme->setCurrentIndex(0);
        break;
    case Qt::ColorScheme::Dark:
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

    ui->cbFont->setChecked(gSettings->useSystemFont());

    gSettings->beginGroup("General");
    ui->cbNumberFormat->setChecked(gSettings->value("UseLanguageLocale", true).toBool());
    ui->cbShowTooltips->setChecked(gSettings->value("TooltipsEnabled", true).toBool());
    ui->cbShowAnimations->setChecked(gSettings->value("Animations", true).toBool());
    ui->cbConfirmClose->setChecked(gSettings->value("BeendenAbfrage", true).toBool());
    ui->cbCheckUpdate->setChecked(gSettings->value("CheckUpdate", true).toBool());
    gSettings->endGroup();

    ui->tbLogLevel->setValue(gSettings->logLevel());

    ui->cbModuleGaerverlauf->setChecked(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
    ui->cbModuleSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->cbModuleSchnellgaerprobe->setEnabled(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
    ui->cbModuleAusdruck->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusdruck));
    ui->cbModuleEtiketten->setChecked(gSettings->isModuleEnabled(Settings::ModuleEtikett));
    ui->cbModuleBewertungen->setChecked(gSettings->isModuleEnabled(Settings::ModuleBewertung));
    ui->cbModuleBrauuebersicht->setChecked(gSettings->isModuleEnabled(Settings::ModuleBrauuebersicht));
    ui->cbModuleAnlagen->setChecked(gSettings->isModuleEnabled(Settings::ModuleAusruestung));
    ui->cbModuleLager->setChecked(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
    ui->cbModuleSpeise->setChecked(gSettings->isModuleEnabled(Settings::ModuleSpeise));
    ui->cbModuleWasseraufbereitung->setChecked(gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung));
    ui->cbModulePreiskalkulation->setChecked(gSettings->isModuleEnabled(Settings::ModulePreiskalkulation));
}

void DlgEinstellungen::on_btnDatabase_clicked()
{
    QString databasePath = QFileDialog::getOpenFileName(this, tr("Datenbank auswählen"),
                                                       gSettings->databasePath(),
                                                       tr("Datenbank (*.sqlite);;Alle Dateien (*.*)"));
    if (!databasePath.isEmpty())
    {
        gSettings->setDatabasePath(databasePath);
    }
}

void DlgEinstellungen::on_cbTheme_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        gSettings->setTheme(Qt::ColorScheme::Light);
        break;
    case 1:
        gSettings->setTheme(Qt::ColorScheme::Dark);
        break;
    }
}

void DlgEinstellungen::on_cbLanguage_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        gSettings->setLanguage(QStringLiteral("de"));
        break;
    case 1:
        gSettings->setLanguage(QStringLiteral("en"));
        break;
    case 2:
        gSettings->setLanguage(QStringLiteral("sv"));
        break;
    case 3:
        gSettings->setLanguage(QStringLiteral("nl"));
        break;
    }
}

void DlgEinstellungen::on_cbFont_clicked(bool checked)
{
    if (checked)
    {
        gSettings->setUseSystemFont(true);
        qApp->setFont(gSettings->font);
    }
    else
    {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, gSettings->font, this);
        if (ok)
        {
            gSettings->setUseSystemFont(false);
            gSettings->setFont(font);
            qApp->setFont(gSettings->font);
        }
        else
        {
            ui->cbFont->setChecked(true);
        }
    }
}

void DlgEinstellungen::on_cbNumberFormat_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "UseLanguageLocale", checked);
}

void DlgEinstellungen::on_cbShowTooltips_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "TooltipsEnabled", checked);
}

void DlgEinstellungen::on_cbShowAnimations_clicked(bool checked)
{
    gSettings->setAnimationsEnabled(checked);
}

void DlgEinstellungen::on_cbConfirmClose_clicked(bool checked)
{
    gSettings->setValueInGroup("General", "BeendenAbfrage", checked);
}

void DlgEinstellungen::on_cbCheckUpdate_toggled(bool checked)
{
    gSettings->setValueInGroup("General", "CheckUpdate", checked);
}

void DlgEinstellungen::on_tbLogLevel_valueChanged(int level)
{
    gSettings->setLogLevel(level);
}

void DlgEinstellungen::on_cbModuleGaerverlauf_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleGaerverlauf, checked);
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
    ui->cbModuleSchnellgaerprobe->setChecked(gSettings->isModuleEnabled(Settings::ModuleSchnellgaerprobe));
    ui->cbModuleSchnellgaerprobe->setEnabled(gSettings->isModuleEnabled(Settings::ModuleGaerverlauf));
}

void DlgEinstellungen::on_cbModuleSchnellgaerprobe_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSchnellgaerprobe, checked);
}

void DlgEinstellungen::on_cbModuleAusdruck_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusdruck, checked);
}

void DlgEinstellungen::on_cbModuleEtiketten_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleEtikett, checked);
}

void DlgEinstellungen::on_cbModuleBewertungen_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBewertung, checked);
}

void DlgEinstellungen::on_cbModuleBrauuebersicht_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleBrauuebersicht, checked);
}

void DlgEinstellungen::on_cbModuleAnlagen_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleAusruestung, checked);
}

void DlgEinstellungen::on_cbModuleLager_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleLagerverwaltung, checked);
}

void DlgEinstellungen::on_cbModuleSpeise_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleSpeise, checked);
}

void DlgEinstellungen::on_cbModuleWasseraufbereitung_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModuleWasseraufbereitung, checked);
}

void DlgEinstellungen::on_cbModulePreiskalkulation_clicked(bool checked)
{
    gSettings->enableModule(Settings::ModulePreiskalkulation, checked);
}
