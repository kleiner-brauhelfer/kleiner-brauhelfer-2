#include "chartview.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))

ChartView::ChartView(QWidget *parent) :
    QChartView(parent)
{
    QChart *c = chart();
    c->layout()->setContentsMargins(0, 0, 0, 0);
    c->setBackgroundRoundness(0);
    c->legend()->setAlignment(Qt::AlignRight);
    setRenderHint(QPainter::Antialiasing);
}

#endif
