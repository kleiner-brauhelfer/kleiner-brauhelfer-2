#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
public:
    SpinBox(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorOnLimit(bool e);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool mError;
    bool mErrorOnLimit;
};

#endif // SPINBOX_H
