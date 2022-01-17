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
    template<typename DLG> static void closeDialog();

    explicit DlgAbstract(const QString& settingsGroup, QWidget *parent = nullptr);
    virtual bool event(QEvent *event) Q_DECL_OVERRIDE;
    static void restoreView(const QString& settingsGroup);

    virtual void modulesChanged(Settings::Modules modules);

protected:
    virtual void saveSettings();
    virtual void loadSettings();

    void setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets);

private:
    const QString mSettingsGroup;
};


#endif // DLG_ABSTRACT_H

#include "dlgabstract-inl.h"
