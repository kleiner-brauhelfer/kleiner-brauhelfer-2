#include "dlgabstract.h"

extern Settings *gSettings;


DlgAbstract::DlgAbstract(QWidget *parent) :
    QDialog(parent)
{
}

void DlgAbstract::restoreSize()
{
    restoreGeometry(gSettings->value("geometry").toByteArray());
}

void DlgAbstract::saveSettings()
{
    gSettings->setValue("geometry", saveGeometry());
}


void DlgAbstract::restoreView()
{
    // TODO: set default size (not called if dialog is not opened)
    restoreGeometry(gSettings->value("geometry").toByteArray());
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


