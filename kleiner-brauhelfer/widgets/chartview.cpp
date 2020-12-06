#ifdef QT_CHARTS_LIB

#include "chartview.h"
#include "settings.h"

extern Settings *gSettings;

ChartView::ChartView(QWidget *parent) :
    QChartView(parent)
{
    QChart *c = chart();
    c->layout()->setContentsMargins(0, 0, 0, 0);
    c->setBackgroundRoundness(0);
    if (gSettings->theme() == Settings::Theme::Dark)
        c->setTheme(QChart::ChartThemeDark);
    c->legend()->setAlignment(Qt::AlignRight);
    setRenderHint(QPainter::Antialiasing);
}

#endif // QT_CHARTS_LIB
