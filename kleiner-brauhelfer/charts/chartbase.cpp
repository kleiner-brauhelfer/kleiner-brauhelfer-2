#include "chartbase.h"
#include "settings.h"

extern Settings *gSettings;

ChartBase::ChartBase(QWidget *parent) :
    QCustomPlot(parent)
{
    if (gSettings->theme() == Settings::Theme::Dark)
    {
        setBackground(gSettings->palette.color(QPalette::Active, QPalette::Base));
        QColor colorForeground = gSettings->palette.color(QPalette::Active, QPalette::Text);
        QPen penForeground = QPen(colorForeground, 1);
        xAxis->setBasePen(penForeground);
        yAxis->setBasePen(penForeground);
        xAxis->setTickPen(penForeground);
        yAxis->setTickPen(penForeground);
        xAxis->setSubTickPen(penForeground);
        yAxis->setSubTickPen(penForeground);
        xAxis->setTickLabelColor(colorForeground);
        yAxis->setTickLabelColor(colorForeground);
        xAxis->setLabelColor(colorForeground);
        yAxis->setLabelColor(colorForeground);
        legend->setTextColor(colorForeground);
    }
}
