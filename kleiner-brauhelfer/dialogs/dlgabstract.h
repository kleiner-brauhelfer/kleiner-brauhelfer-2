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
    template<class DLG> static bool showDialog(QWidget *parent, QAction* action = nullptr);
    template<class DLG> static void restoreView();

    explicit DlgAbstract(QWidget *parent = nullptr);

    virtual void saveSettings();
    virtual void modulesChanged(Settings::Modules modules);
    virtual void restoreSize();
    virtual void restoreView();


protected:
    void setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets);

};


#endif // DLG_ABSTRACT_H

#include "dlgabstract-inl.h"

