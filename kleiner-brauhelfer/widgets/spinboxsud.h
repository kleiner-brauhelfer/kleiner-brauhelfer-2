#ifndef SPINBOXSUD_H
#define SPINBOXSUD_H

#include "spinbox.h"

class SpinBoxSud : public SpinBox
{
    Q_OBJECT

public:
    SpinBoxSud(QWidget *parent = nullptr);
    void setColumn(int col);
    int column() const;
    void updateValue();
private slots:
    void on_valueChanged(int val);
private:
    int mCol;
};

#endif // SPINBOXSUD_H
