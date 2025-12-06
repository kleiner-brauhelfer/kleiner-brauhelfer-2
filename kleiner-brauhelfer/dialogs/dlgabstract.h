#ifndef DLG_ABSTRACT_H
#define DLG_ABSTRACT_H

#include <QDialog>
#include <QAction>

#define MAKE_TRANSLATABLE_DLG \
protected: \
    void changeEvent(QEvent * event) Q_DECL_OVERRIDE \
    { \
        if (event->type() == QEvent::LanguageChange) \
            ui->retranslateUi(this); \
        DlgAbstract::changeEvent(event); \
    }

class DlgAbstract : public QDialog
{
    Q_OBJECT

public:
    template<class DLG> static DLG* showDialog(QWidget *parent, QAction* action = nullptr);
    template<typename DLG> static void closeDialog();

public:
    explicit DlgAbstract(const QString& settingsGroup, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    static void restoreView(const QString& settingsGroup, DlgAbstract* dlg = nullptr);

protected:
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void saveSettings() {};
    virtual void loadSettings() {};

private slots:
    void on_finished(int result);

private:
    const QString mSettingsGroup;
    QByteArray mDefaultGeometry;
};


#endif // DLG_ABSTRACT_H

#include "dlgabstract-inl.h"
