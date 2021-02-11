#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include "widgetdecorator.h"

class DoubleSpinBox : public QDoubleSpinBox, public WidgetDecorator
{
    Q_OBJECT

public:
    DoubleSpinBox(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
    void setErrorOnLimit(bool e);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_valueChanged();
private:
    bool mError;
    bool mErrorOnLimit;
};

#endif // DOUBLESPINBOX_H
