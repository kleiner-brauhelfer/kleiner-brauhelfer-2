#ifndef DLG_ABSTRACT_H
#define DLG_ABSTRACT_H

#include <QDialog>
#include <QWidget>
#include "settings.h"

class DlgAbstract : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAbstract(QWidget *parent = nullptr);

    virtual void saveSettings();
    virtual void restoreView(bool full);
    virtual void modulesChanged(Settings::Modules modules);

protected:
    void setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets);

private:
    // TODO closeEvent

};

#endif // TABABSTRACT_H
