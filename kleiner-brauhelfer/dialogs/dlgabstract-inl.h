#ifndef DLG_ABSTRACT_INL_H
#define DLG_ABSTRACT_INL_H

#include <QApplication>
#include "dlgabstract.h"

template<typename DLG>
bool DlgAbstract::showDialog(QWidget *parent, QAction* action)
{
    if(QCoreApplication::instance()->thread() != parent->thread())
        qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
    if (DLG::Dialog)
    {
        DLG::Dialog->raise();
        DLG::Dialog->activateWindow();
        if (action)
        {
            action->setChecked(true);
        }
        return false;
    }
    else
    {
        DLG::Dialog = new DLG(parent);
        connect(DLG::Dialog, &DLG::finished, []{
            DLG::Dialog->deleteLater();
            DLG::Dialog = nullptr;
            });
        DLG::Dialog->show();
        if (action)
        {
            connect(DLG::Dialog, &DLG::finished, [action]{action->setChecked(false);});
            action->setChecked(true);
        }
        return true;
    }
}

template<typename DLG>
void DlgAbstract::closeDialog()
{
    if (DLG::Dialog)
    {
        if(QCoreApplication::instance()->thread() != DLG::Dialog->thread())
            qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
        DLG::Dialog->close();
    }
}

template<typename DLG>
void DlgAbstract::modulesChanged(Settings::Modules modules)
{
    if (DLG::Dialog)
    {
        if(QCoreApplication::instance()->thread() != DLG::Dialog->thread())
            qWarning("DlgAbstract: Access to dialog outside outside the main thread context is unsafe");
        DLG::Dialog->modulesChanged(modules);
    }
}

#endif // DLG_ABSTRACT_INL_H

