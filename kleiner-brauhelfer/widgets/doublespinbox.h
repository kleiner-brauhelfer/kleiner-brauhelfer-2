#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DoubleSpinBox(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorRange(double min, double max);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
public slots:
    void setValue(double val);
private:
    bool mError;
    double mErrorLimitMin;
    double mErrorLimitMax;
};

#endif // DOUBLESPINBOX_H
