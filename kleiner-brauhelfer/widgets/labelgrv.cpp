#include "labelgrv.h"
#include "units.h"

LabelGrV::LabelGrV(QWidget *parent) :
    QLabel(parent)
{
    setText(Units::text(Units::GravityUnit()));
}
