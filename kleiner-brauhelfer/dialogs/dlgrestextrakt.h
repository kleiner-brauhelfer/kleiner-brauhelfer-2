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
    void on_spinBox_S_SwPlato_valueChanged(double value);
    void on_spinBox_S_SwDichte_valueChanged(double value);
    void on_spinBox_S_Temperatur_valueChanged(double value);
    void on_spinBox_R_SwBrix_valueChanged(double);
    void on_comboBox_FormelBrixPlato_currentIndexChanged(const QString &value);
    void on_tbKorrekturFaktor_valueChanged(double value);

private:
    void calculateFromRefraktometer();

private:
    Ui::DlgRestextrakt *ui;
    double SWAnstellen;
};

#endif // DLGRESTEXTRAKT_H
