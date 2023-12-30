#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton
{
    Q_OBJECT

public:
    PushButton(QWidget *parent = nullptr);
    void setDefaultPalette(const QPalette &p);
    void setError(bool e);
private:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    QPalette mDefaultPalette;
    bool mError;
};

#endif // PUSHBUTTON_H
