#ifndef DLGRESTEXTRAKT_H
#define DLGRESTEXTRAKT_H

#include <QDialog>

namespace Ui {
class DlgRestextrakt;
}

class DlgRestextrakt : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRestextrakt(double value, double sw = 0.0, double temp = 20.0, QWidget *parent = nullptr);
    ~DlgRestextrakt();
    double value() const;
    double temperature() const;

private slots:
    void on_DlgRestextrakt_accepted();
    void on_tbPlato_valueChanged(double value);
    void on_tbDichte_valueChanged(double value);
    void on_tbTemp_valueChanged(double value);
    void on_tbBrix_valueChanged(double);
    void on_comboBox_FormelBrixPlato_currentIndexChanged(const QString &value);
    void on_tbKorrekturFaktor_valueChanged(double value);
    void on_btnKorrekturFaktorDefault_clicked();

private:
    void calculateFromRefraktometer();

private:
    Ui::DlgRestextrakt *ui;
    bool mSw;
};

#endif // DLGRESTEXTRAKT_H
