#ifndef DOUBLESPINBOXGRV_H
#define DOUBLESPINBOXGRV_H

#include "doublespinbox.h"


class DoubleSpinBoxGrV : public DoubleSpinBox
{
    Q_OBJECT

public:
    DoubleSpinBoxGrV(QWidget *parent = nullptr);
    void setColumn(int col);
    int column() const;
    void updateValue();
private slots:
    void on_valueChanged(double value);
private:
    int mCol;
    QString mSource;
};

#endif // DOUBLESPINBOXGRV_H
