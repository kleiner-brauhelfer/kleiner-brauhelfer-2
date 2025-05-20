#ifndef DLGMAISCHPLANMALZ_H
#define DLGMAISCHPLANMALZ_H

#include "dlgabstract.h"

namespace Ui {
class DlgMaischplanMalz;
}

class DlgMaischplanMalz : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgMaischplanMalz(double value, QWidget *parent = nullptr);
    ~DlgMaischplanMalz();
    double value() const;

private slots:
    void updateAnteile();
    void updateValue();
    void on_tbMenge_valueChanged(double value);

private:
    Ui::DlgMaischplanMalz *ui;
};

#endif // DLGMAISCHPLANMALZ_H
