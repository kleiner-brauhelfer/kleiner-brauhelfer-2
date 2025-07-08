#ifndef SPINBOXSUD_H
#define SPINBOXSUD_H

#include "spinbox.h"

class SudObject;


class SpinBoxSud : public SpinBox
{
    Q_OBJECT

public:
    SpinBoxSud(QWidget *parent = nullptr);
    void setColumn(SudObject *sud, int col);
    int column() const;
    void updateValue();
private slots:
    void on_valueChanged(int val);
private:
    SudObject *mSud;
    int mCol;
};

#endif // SPINBOXSUD_H
