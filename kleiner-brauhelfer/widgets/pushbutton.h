#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>
#include "widgetdecorator.h"

class PushButton : public QPushButton, public WidgetDecorator
{
    Q_OBJECT

public:
    PushButton(QWidget *parent = nullptr);
    void setError(bool e);
private:
    void updatePalette();
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_valueChanged();
private:
    bool mError;
};

#endif // PUSHBUTTON_H
