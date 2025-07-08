#ifndef DOUBLESPINBOXSUD_H
#define DOUBLESPINBOXSUD_H

#include "doublespinbox.h"

class SudObject;

class DoubleSpinBoxSud : public DoubleSpinBox
{
    Q_OBJECT

public:
    DoubleSpinBoxSud(QWidget *parent = nullptr);
    void setColumn(SudObject *sud, int col);
    int column() const;
    void updateValue();
private slots:
    void on_valueChanged(double val);
private:
    SudObject *mSud;
    int mCol;
};

#endif // DOUBLESPINBOXSUD_H
