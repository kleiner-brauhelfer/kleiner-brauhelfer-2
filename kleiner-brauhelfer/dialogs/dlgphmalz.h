#ifndef DLGPHMALZ_H
#define DLGPHMALZ_H

#include <QDialog>

namespace Ui {
class DlgPhMalz;
}

class DlgPhMalz : public QDialog
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
