#include "expanderpushbutton.h"

ExpanderPushButton::ExpanderPushButton(QWidget *parent) :
    QPushButton(parent)
{
    setCheckable(true);
    setAutoDefault(false);
}

void ExpanderPushButton::setChild(QWidget* wdg)
{
    wdg->setVisible(isChecked());
    connect(this, &QAbstractButton::toggled, wdg, &QWidget::setVisible);
}
