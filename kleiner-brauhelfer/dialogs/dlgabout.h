#ifndef DLGABOUT_H
#define DLGABOUT_H

#include "dlgabstract.h"
#include "ui_dlgabout.h"

namespace Ui {
class DlgAbout;
}

class DlgAbout : public DlgAbstract
{
    Q_OBJECT
    MAKE_TRANSLATABLE_DLG

public:
    static DlgAbout *Dialog;

public:
    explicit DlgAbout(QWidget *parent = nullptr);
    ~DlgAbout();

private:
    QString generateLink(const QString &url, const QString &name = QString());

private:
    Ui::DlgAbout *ui;
};

#endif // DLGABOUT_H
