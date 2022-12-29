#include "chartviewabfuelldaten.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

ChartViewAbfuelldaten::ChartViewAbfuelldaten(QWidget *parent) :
    ChartView(parent)
{
  #ifdef QT_CHARTS_LIB
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append({tr("Anstellen"), tr("Jungbier"), tr("Abgefüllt"), tr("Rezept")});
    chart()->addAxis(axisX, Qt::AlignBottom);
    axisY = new QValueAxis();
    axisY->setTitleText(tr("Würzemenge [L]"));
    axisY->setMin(0);
    axisY->setMax(100);
    chart()->addAxis(axisY, Qt::AlignLeft);

    setMenge = new QBarSet("Menge");
    *setMenge << 0 << 0 << 0 << 0;

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(setMenge);
    chart()->addSeries(barSeries);
    barSeries->setLabelsVisible(true);
    barSeries->setLabelsAngle(-90);
    barSeries->setLabelsPrecision(4);
    barSeries->attachAxis(axisX);
    barSeries->attachAxis(axisY);

    chart()->legend()->setVisible(false);
  #endif
}

void ChartViewAbfuelldaten::update()
{
  #ifdef QT_CHARTS_LIB
    setMenge->replace(0, bh->sud()->getWuerzemengeAnstellen());
    setMenge->replace(1, bh->sud()->getJungbiermengeAbfuellen());
    setMenge->replace(2, bh->sud()->geterg_AbgefuellteBiermenge());
    setMenge->replace(3, bh->sud()->getMenge());

    double maxVal = 0;
    for (int i = 0; i < setMenge->count(); i++)
    {
        double val = setMenge->at(i);
        if (val > maxVal)
            maxVal = val;
    }
    axisY->setMax(int(maxVal/10)*10+10);
  #endif
}
