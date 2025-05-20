#include "dlgabstract.h"
#include "settings.h"

extern Settings *gSettings;

DlgAbstract::DlgAbstract(const QString &settingsGroup, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent,f),
    mSettingsGroup(settingsGroup)
{
    connect(this, &QDialog::finished, this, &DlgAbstract::on_finished);
}

void DlgAbstract::showEvent(QShowEvent *event)
{
    gSettings->beginGroup(mSettingsGroup);
    restoreGeometry(gSettings->value("geometry").toByteArray());
    gSettings->endGroup();
    loadSettings();
    QDialog::showEvent(event);
}

void DlgAbstract::on_finished(int result)
{
    Q_UNUSED(result)
    gSettings->beginGroup(mSettingsGroup);
    gSettings->setValue("geometry", saveGeometry());
    gSettings->endGroup();
    saveSettings();
}

void DlgAbstract::restoreView(const QString& settingsGroup)
{
    gSettings->beginGroup(settingsGroup);
    gSettings->remove("geometry");
    gSettings->endGroup();
}

void DlgAbstract::saveSettings()
{
}

void DlgAbstract::loadSettings()
{
}
