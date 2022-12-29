#include "chartviewbraudaten.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

ChartViewBraudaten::ChartViewBraudaten(QWidget *parent) :
    ChartView(parent)
{
  #ifdef QT_CHARTS_LIB
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append({tr("Kochbeginn"), tr("Kochende"), tr("Nach Hopfenseihen"), tr("Anstellen")});
    chart()->addAxis(axisX, Qt::AlignBottom);
    axisY = new QValueAxis();
    axisY->setTitleText(tr("Würzemenge [L]"));
    axisY->setMin(0);
    axisY->setMax(100);
    chart()->addAxis(axisY, Qt::AlignLeft);
    axisY2 = new QValueAxis();
    axisY2->setTitleText(tr("Stammwürze [°P]"));
    axisY2->setMin(0);
    axisY2->setMax(20);
    chart()->addAxis(axisY2, Qt::AlignRight);

    setMengeIst = new QBarSet(tr("Menge ist"));
    *setMengeIst << 0 << 0 << 0 << 0;
    setMengeSoll = new QBarSet(tr("Menge soll"));
    *setMengeSoll << 0 << 0 << 0 << 0;

    QBarSeries* seriesMenge = new QBarSeries();
    seriesMenge->append(setMengeIst);
    seriesMenge->append(setMengeSoll);
    chart()->addSeries(seriesMenge);
    setMengeSoll->setColor(setMengeIst->color().darker(200));
    seriesMenge->setLabelsVisible(true);
    seriesMenge->setLabelsAngle(-90);
    seriesMenge->setLabelsPrecision(4);
    seriesMenge->attachAxis(axisX);
    seriesMenge->attachAxis(axisY);

    seriesSwSoll = new QLineSeries();
    seriesSwSoll->setName(tr("SW soll"));
    *seriesSwSoll << QPointF(0,0) <<  QPointF(1,0) <<  QPointF(2,0) <<  QPointF(3,0);
    chart()->addSeries(seriesSwSoll);
    QPen pen = seriesSwSoll->pen();
    pen.setStyle(Qt::DashLine);
    seriesSwSoll->setPen(pen);
    seriesSwSoll->attachAxis(axisX);
    seriesSwSoll->attachAxis(axisY2);

    seriesSwIst = new QLineSeries();
    seriesSwIst->setName(tr("SW ist"));
    *seriesSwIst << QPointF(0,0) <<  QPointF(1,0) <<  QPointF(2,0) <<  QPointF(3,0);
    chart()->addSeries(seriesSwIst);
    seriesSwIst->attachAxis(axisX);
    seriesSwIst->attachAxis(axisY2);

    seriesSwSoll->setColor(seriesSwIst->color().darker(200));

    chart()->legend()->setAlignment(Qt::AlignBottom);
  #endif
}

void ChartViewBraudaten::update()
{
  #ifdef QT_CHARTS_LIB
    setMengeIst->replace(0, bh->sud()->getWuerzemengeKochbeginn());
    setMengeIst->replace(1, bh->sud()->getWuerzemengeVorHopfenseihen());
    setMengeIst->replace(2, bh->sud()->getWuerzemengeKochende());
    setMengeIst->replace(3, bh->sud()->getWuerzemengeAnstellen());

    setMengeSoll->replace(0, bh->sud()->getMengeSollKochbeginn());
    setMengeSoll->replace(1, bh->sud()->getMengeSollKochende());
    setMengeSoll->replace(2, bh->sud()->getMengeSollKochende());
    setMengeSoll->replace(3, bh->sud()->getMenge());

    double maxVal = 0;
    for (int i = 0; i < setMengeIst->count(); i++)
    {
        double val = setMengeIst->at(i);
        if (val > maxVal)
            maxVal = val;
    }
    for (int i = 0; i < setMengeSoll->count(); i++)
    {
        double val = setMengeSoll->at(i);
        if (val > maxVal)
            maxVal = val;
    }
    axisY->setMax(int(maxVal/10)*10+10);

    seriesSwIst->replace(0, QPointF(0,bh->sud()->getSWKochbeginn()));
    seriesSwIst->replace(1, QPointF(1,bh->sud()->getSWKochende()));
    seriesSwIst->replace(2, QPointF(2,bh->sud()->getSWKochende()));
    seriesSwIst->replace(3, QPointF(3,bh->sud()->getSWAnstellen()));

    seriesSwSoll->replace(0, QPointF(0,bh->sud()->getSWSollKochbeginn()));
    seriesSwSoll->replace(1, QPointF(1,bh->sud()->getSWSollKochende()));
    seriesSwSoll->replace(2, QPointF(2,bh->sud()->getSWSollKochende()));
    seriesSwSoll->replace(3, QPointF(3,bh->sud()->getSWSollAnstellen()));

    maxVal = 0;
    for (int i = 0; i < seriesSwIst->count(); i++)
    {
        double val = seriesSwIst->at(i).y();
        if (val > maxVal)
            maxVal = val;
    }
    for (int i = 0; i < seriesSwSoll->count(); i++)
    {
        double val = seriesSwSoll->at(i).y();
        if (val > maxVal)
            maxVal = val;
    }
    axisY2->setMax(int(maxVal/10)*10+10);

  #endif
}
