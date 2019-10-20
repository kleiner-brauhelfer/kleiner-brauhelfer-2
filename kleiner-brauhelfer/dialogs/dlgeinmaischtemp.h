#ifndef DLGEINMAISCHTEMP_H
#define DLGEINMAISCHTEMP_H

#include <QDialog>

namespace Ui {
class DlgEinmaischTemp;
}

class DlgEinmaischTemp : public QDialog
{
    Q_OBJECT

public:
    explicit DlgEinmaischTemp(double schuettung, int schuettungTemp, double hauptguss, int rastTemp, QWidget *parent = nullptr);
    ~DlgEinmaischTemp();
    int value() const;

private slots:
    void on_doubleSpinBox_Schuettung_valueChanged(double arg1);
    void on_spinBox_SchuettungTemp_valueChanged(int arg1);
    void on_doubleSpinBox_Hauptguss_valueChanged(double arg1);
    void on_spinBox_RastTemp_valueChanged(int arg1);

private:
    void doCalc();

private:
    Ui::DlgEinmaischTemp *ui;
};

#endif // DLGEINMAISCHTEMP_H
