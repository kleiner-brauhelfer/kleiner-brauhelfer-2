#ifndef DLGSUDTEILEN_H
#define DLGSUDTEILEN_H

#include "dlgabstract.h"

namespace Ui {
class DlgSudTeilen;
}

class DlgSudTeilen : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgSudTeilen(const QString& name, double amount, QWidget *parent = 0);
    ~DlgSudTeilen();
    double prozent() const;
    QString nameTeil1() const;
    QString nameTeil2() const;

private slots:
    void on_horizontalSlider_Percent_valueChanged(int position);
    void on_spinBox_Percent1_valueChanged(int position);
    void on_spinBox_Percent2_valueChanged(int position);
    void on_doubleSpinBox_Amount1_valueChanged(double amount);
    void on_doubleSpinBox_Amount2_valueChanged(double amount);

private:
    Ui::DlgSudTeilen *ui;
    double mTotalAmount;
};

#endif // DLGSUDTEILEN_H
