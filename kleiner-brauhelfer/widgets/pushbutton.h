#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT

public:
    PushButton(QWidget *parent = nullptr);
    void setError(bool e);
private:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool mError;
};

#endif // PUSHBUTTON_H
