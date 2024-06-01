#ifndef EXPANDERPUSHBUTTON_H
#define EXPANDERPUSHBUTTON_H

#include <QPushButton>

class ExpanderPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ExpanderPushButton(QWidget *parent = nullptr);
    void setChild(QWidget* wdg);
};

#endif // EXPANDERPUSHBUTTON_H
