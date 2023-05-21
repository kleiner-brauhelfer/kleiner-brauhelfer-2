#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>
#include "widgetdecorator.h"

class SpinBox : public QSpinBox, public WidgetDecorator
{
    Q_OBJECT

public:
    SpinBox(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorRange(int min, int max);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_valueChanged();
private:
    bool mError;
    int mErrorLimitMin;
    int mErrorLimitMax;
};

#endif // SPINBOX_H
