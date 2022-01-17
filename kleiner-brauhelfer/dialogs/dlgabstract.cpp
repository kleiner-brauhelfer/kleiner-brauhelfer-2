#include "dlgabstract.h"

extern Settings *gSettings;

DlgAbstract::DlgAbstract(const QString &settingsGroup, QWidget *parent) :
    QDialog(parent),
    mSettingsGroup(settingsGroup)
{
}

bool DlgAbstract::event(QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        modulesChanged(Settings::ModuleAlle);
        gSettings->beginGroup(mSettingsGroup);
        restoreGeometry(gSettings->value("geometry").toByteArray());
        gSettings->endGroup();
        loadSettings();
    }
    else if (event->type() == QEvent::Close)
    {
        gSettings->beginGroup(mSettingsGroup);
        gSettings->setValue("geometry", saveGeometry());
        gSettings->endGroup();
        saveSettings();
    }
    return QDialog::event(event);
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
