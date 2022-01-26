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
    template<class DLG> static void showDialog(QWidget *parent, QAction* action = nullptr);
    template<typename DLG> static void closeDialog();
    template<typename DLG> static void modulesChanged(Settings::Modules modules);

public:
    explicit DlgAbstract(const QString& settingsGroup, QWidget *parent = nullptr);
    static void restoreView(const QString& settingsGroup);
    virtual void modulesChanged(Settings::Modules modules);

protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void saveSettings();
    virtual void loadSettings();
    void setVisibleModule(Settings::Module module, const QVector<QWidget*>& widgets);

private slots:
    void on_finished(int result);

private:
    const QString mSettingsGroup;
};


#endif // DLG_ABSTRACT_H

#include "dlgabstract-inl.h"
