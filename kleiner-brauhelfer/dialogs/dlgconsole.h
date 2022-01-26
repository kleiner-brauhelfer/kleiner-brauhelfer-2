#ifndef DLGCONSOLE_H
#define DLGCONSOLE_H

#include "dlgabstract.h"

namespace Ui {
class DlgConsole;
}

class DlgConsole : public DlgAbstract
{
    Q_OBJECT

public:
    static DlgConsole *Dialog;

public:
    explicit DlgConsole(QWidget *parent = nullptr);
    ~DlgConsole();
    static void restoreView();
    void append(const QString &text);

private slots:
    void on_btnClear_clicked();
    void on_sbLevel_valueChanged(int level);

private:
    Ui::DlgConsole *ui;
};

#endif // DLGCONSOLE_H
