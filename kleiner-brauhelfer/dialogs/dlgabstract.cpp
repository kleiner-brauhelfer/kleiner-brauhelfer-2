#include "dlgabstract.h"

extern Settings *gSettings;


DlgAbstract::DlgAbstract(QWidget *parent) :
    QDialog(parent)
{
}

void DlgAbstract::restoreSize()
{
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
}

void DlgAbstract::saveSettings()
{
    gSettings->setValue("size", geometry().size());
}


void DlgAbstract::restoreView(bool full)
{
    Q_UNUSED(full);
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
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


