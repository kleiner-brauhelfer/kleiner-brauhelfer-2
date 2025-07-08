#ifndef DLGMAISCHPLANMALZ_H
#define DLGMAISCHPLANMALZ_H

#include "dlgabstract.h"

class SudObject;

namespace Ui {
class DlgMaischplanMalz;
}

class DlgMaischplanMalz : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgMaischplanMalz(SudObject *sud, double value, QWidget *parent = nullptr);
    ~DlgMaischplanMalz();
    static void restoreView();
    double value() const;

private slots:
    void updateAnteile();
    void updateValue();
    void on_tbMenge_valueChanged(double value);

private:
    Ui::DlgMaischplanMalz *ui;
    SudObject *mSud;
};

#endif // DLGMAISCHPLANMALZ_H
