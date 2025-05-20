#ifndef DLGVERDAMPFUNG_H
#define DLGVERDAMPFUNG_H

#include "dlgabstract.h"

namespace Ui {
class DlgVerdampfung;
}

class DlgVerdampfung : public DlgAbstract
{
	Q_OBJECT

public:
    explicit DlgVerdampfung(QWidget *parent = nullptr);
	~DlgVerdampfung();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	void on_dSpinBox_Menge1_valueChanged(double arg1);
	void on_dSpinBox_Menge2_valueChanged(double arg1);
    void on_spinBox_Kochdauer_valueChanged(int arg1);
    void on_dSpinBox_cmVonObenMenge1_valueChanged(double arg1);
    void on_dSpinBox_cmVomBodenMenge1_valueChanged(double arg1);
    void on_dSpinBox_cmVonObenMenge2_valueChanged(double arg1);
    void on_dSpinBox_cmVomBodenMenge2_valueChanged(double arg1);

private:
	Ui::DlgVerdampfung *ui;
	void berechne();
    double mFlaeche;
    double Hoehe;

public:
  void setMenge1(double value);
  void setMenge2(double value);
  void setKochdauer(int value);
  double getVerdampfungsziffer();
  double getVerdampfungsrate();
  void setDurchmesser(double value);
  void setHoehe(double value);
};

#endif // DLGVERDAMPFUNG_H
