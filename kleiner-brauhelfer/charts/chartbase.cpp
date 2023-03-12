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
        xAxis->setBasePen(QPen(colorForeground, 1));
        yAxis->setBasePen(QPen(colorForeground, 1));
        xAxis->setTickPen(QPen(colorForeground, 1));
        yAxis->setTickPen(QPen(colorForeground, 1));
        xAxis->setSubTickPen(QPen(colorForeground, 1));
        yAxis->setSubTickPen(QPen(colorForeground, 1));
        xAxis->setTickLabelColor(colorForeground);
        yAxis->setTickLabelColor(colorForeground);
        xAxis->setLabelColor(colorForeground);
        yAxis->setLabelColor(colorForeground);
    }
}
