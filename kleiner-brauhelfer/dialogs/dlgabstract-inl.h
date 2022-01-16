#ifndef DLG_ABSTRACT_INL_H
#define DLG_ABSTRACT_INL_H

#include <QApplication>
#include "dlgabstract.h"

template<typename DLG>
void DlgAbstract::modulesChanged(Settings::Modules modules)
{
    if (DLG::Dialog)
    {
        if(QCoreApplication::instance()->thread() != DLG::Dialog->thread())
            qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
        DLG::Dialog->modulesChanged(modules);
    }
    else
    {
        // auslassen wenn es fehlt
    }
}

template<typename DLG, typename MW>
bool DlgAbstract::showDialog(MW *parent)
{
    if(QCoreApplication::instance()->thread() != parent->thread())
        qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
    if (DLG::Dialog)
    {
        DLG::Dialog->raise();
        DLG::Dialog->activateWindow();
    }
    else
    {
        DLG::Dialog = new DLG(parent);
        connect(DLG::Dialog, &DLG::finished, []{
            DLG::Dialog->deleteLater();
            DLG::Dialog = nullptr;
            });
        DLG::Dialog->show();
        return true;
    }
    return false;
}

template<typename DLG, typename MW>
bool DlgAbstract::showToolDialog(MW *parent, QAction* action, void(MW::* slot_finished)() )
{
    auto ret = DlgAbstract::showDialog<DLG>(parent);
    if(ret)
    {
        connect(DLG::Dialog, &DLG::finished, parent, slot_finished);
    }
    else
    {
        action->setChecked(true);
    }
    return ret;
}

template<typename DLG>
void DlgAbstract::restoreView(bool full)
{
    if (DLG::Dialog)
    {
        if(QCoreApplication::instance()->thread() != DLG::Dialog->thread())
           qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
        DLG::Dialog->restoreView(full);
    }
    else
    {
        // auslassen wenn es fehlt
    }
}

#endif // DLG_ABSTRACT_INL_H

