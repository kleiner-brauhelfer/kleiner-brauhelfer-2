#ifndef DLG_ABSTRACT_H
#define DLG_ABSTRACT_H

#include <QAction>
#include <QDialog>
#include <QWidget>
#include "settings.h"

class DlgAbstract : public QDialog
{
    Q_OBJECT

public:

    template<typename DLG> static void modulesChanged(Settings::Modules modules);
    template<class DLG, typename MW> static bool showDialog(MW *receiver);
    template<class DLG, typename MW> static bool showToolDialog(MW *parent, QAction* action, void(MW::* slot_finished)());
    template<class DLG> static void restoreView(bool full);

    explicit DlgAbstract(QWidget *parent = nullptr);

    virtual void saveSettings();
    virtual void modulesChanged(Settings::Modules modules);
    virtual void restoreSize();
    virtual void restoreView(bool full);


protected:
    void setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets);

};


#include <QApplication>


template<typename DLG>
void DlgAbstract::modulesChanged(Settings::Modules modules)
{
    if (DLG::Dialog)
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() == DLG::Dialog->thread(), "Dialog handling", "Access to dialog outside outside the main thread context is unsafe"); // TODO downgrade to logging
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
    Q_ASSERT_X(QCoreApplication::instance()->thread() == parent->thread(), "Dialog handling", "Access to dialog outside outside the main thread context is unsafe"); // TODO downgrade to logging
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
        Q_ASSERT_X(QCoreApplication::instance()->thread() == DLG::Dialog->thread(), "Dialog handling", "Access to dialog outside the main thread context is unsafe"); // TODO downgrade to logging
        DLG::Dialog->restoreView(full);
    }
    else
    {
        // auslassen wenn es fehlt
    }
}


#endif // TABABSTRACT_H
