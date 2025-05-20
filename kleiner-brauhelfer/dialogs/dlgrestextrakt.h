#ifndef DLGRESTEXTRAKT_H
#define DLGRESTEXTRAKT_H

#include "dlgabstract.h"
#include <QDateTime>

namespace Ui {
class DlgRestextrakt;
}

class DlgRestextrakt : public DlgAbstract
{
    Q_OBJECT

public:
    explicit DlgRestextrakt(double value, double sw = 0.0, double temp = -1, const QDateTime& dt = QDateTime(), QWidget *parent = nullptr);
    ~DlgRestextrakt();
    double value() const;
    void setValue(double value);
    double temperatur() const;
    void setTemperatur(double value);
    QDateTime datum() const;
    void setDatum(const QDateTime& value);

private slots:
    void on_DlgRestextrakt_accepted();
    void on_cbAuswahl_currentIndexChanged(int index);
    void on_tbPlato_valueChanged(double value);
    void on_tbDichte_valueChanged(double value);
    void on_tbTemp_valueChanged(double value);
    void on_tbEichtemp_valueChanged(double value);
    void on_tbBrix_valueChanged(double);
    void on_cbEinheit_activated(int);
    void on_comboBox_FormelBrixPlato_currentTextChanged(const QString &value);
    void on_tbKorrekturFaktor_valueChanged(double value);
    void on_btnKorrekturFaktorDefault_clicked();
    void on_tbTempRefraktometer_valueChanged(double value);
    void on_tbPlatoManuell_valueChanged(double value);
    void on_tbTempManuell_valueChanged(double value);

private:
    void calculateFromRefraktometer();

private:
    Ui::DlgRestextrakt *ui;
    bool mSw;
};

#endif // DLGRESTEXTRAKT_H
