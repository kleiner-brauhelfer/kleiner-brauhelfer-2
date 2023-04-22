#include "chartbase.h"
#include "settings.h"

extern Settings *gSettings;

ChartBase::ChartBase(QWidget *parent) :
    QCustomPlot(parent)
{
    colorText = gSettings->palette.color(QPalette::Active, QPalette::Text);
    colorBack = gSettings->palette.color(QPalette::Active, QPalette::Base);
    if (gSettings->theme() == Settings::Theme::Dark)
    {
        setBackground(colorBack);
        QPen penForeground = QPen(colorText, 1);
        xAxis->setBasePen(penForeground);
        yAxis->setBasePen(penForeground);
        xAxis->setTickPen(penForeground);
        yAxis->setTickPen(penForeground);
        xAxis->setSubTickPen(penForeground);
        yAxis->setSubTickPen(penForeground);
        xAxis->setTickLabelColor(colorText);
        yAxis->setTickLabelColor(colorText);
        xAxis->setLabelColor(colorText);
        yAxis->setLabelColor(colorText);
        legend->setTextColor(colorText);
    }
}
