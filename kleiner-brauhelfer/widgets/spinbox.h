#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT

public:
    SpinBox(QWidget *parent = nullptr);
    void setEnabled(bool e);
    void setDisabled(bool d);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorRange(int min, int max);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
public slots:
    void setValue(int val);
private:
    bool mError;
    int mErrorLimitMin;
    int mErrorLimitMax;
};

#endif // SPINBOX_H
