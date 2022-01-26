#include "dlgabstract.h"

extern Settings *gSettings;

DlgAbstract::DlgAbstract(const QString &settingsGroup, QWidget *parent) :
    QDialog(parent),
    mSettingsGroup(settingsGroup)
{
    connect(this, SIGNAL(finished(int)), this, SLOT(on_finished(int)));
}

void DlgAbstract::showEvent(QShowEvent *event)
{
    modulesChanged(Settings::ModuleAlle);
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

void DlgAbstract::modulesChanged(Settings::Modules modules)
{
    Q_UNUSED(modules)
}

void DlgAbstract::setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets)
{
    bool visible = gSettings->isModuleEnabled(module);
    for (const auto& it : widgets)
        it->setVisible(visible);
}
