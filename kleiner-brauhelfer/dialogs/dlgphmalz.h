#ifndef DLGPHMALZ_H
#define DLGPHMALZ_H

#include "dlgabstract.h"

namespace Ui {
class DlgPhMalz;
}

class DlgPhMalz : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgPhMalz(double farbe, QWidget *parent = nullptr);
    ~DlgPhMalz();
    double pH() const;
    void setPh(double value);

private slots:
    void on_cbTyp_currentIndexChanged(int index);

private:
    Ui::DlgPhMalz *ui;
    const double mFarbe;
};

#endif // DLGPHMALZ_H
