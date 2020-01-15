#ifndef DOUBLESPINBOXSUD_H
#define DOUBLESPINBOXSUD_H

#include "doublespinbox.h"

class DoubleSpinBoxSud : public DoubleSpinBox
{
    Q_OBJECT

public:
    DoubleSpinBoxSud(QWidget *parent = nullptr);
    void setColumn(int col);
    int column() const;
    void updateValue();
private slots:
    void on_valueChanged(double value);
private:
    int mCol;
};

#endif // DOUBLESPINBOXSUD_H
