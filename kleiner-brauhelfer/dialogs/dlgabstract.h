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


#endif // DLG_ABSTRACT_H

#include "dlgabstract-inl.h"

