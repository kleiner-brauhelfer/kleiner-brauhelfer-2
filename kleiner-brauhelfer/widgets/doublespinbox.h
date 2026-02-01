#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

class DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DoubleSpinBox(QWidget *parent = nullptr);
    void setEnabled(bool e);
    void setDisabled(bool d);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorRange(double min, double max);
private:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void updatePalette();
public slots:
    void setValue(double val);
private:
    bool mError;
    double mErrorLimitMin;
    double mErrorLimitMax;
};

#endif // DOUBLESPINBOX_H
