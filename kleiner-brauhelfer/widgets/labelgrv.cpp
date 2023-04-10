#include "labelgrv.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

LabelGrV::LabelGrV(QWidget *parent) :
  QLabel(parent)
{
    clear();
    setText(gSettings->GravityUnitString());
}

//void LabelGrV::paintEvent(QPaintEvent *event)
//{
//    this->clear();
//    this->setText(gSettings->GravityUnitString());
//}
