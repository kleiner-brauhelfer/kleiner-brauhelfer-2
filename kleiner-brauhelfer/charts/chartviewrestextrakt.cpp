#include "chartviewrestextrakt.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

ChartViewRestextrakt::ChartViewRestextrakt(QWidget *parent) :
    ChartView(parent)
{
  #ifdef QT_CHARTS_LIB
    axisX = new QBarCategoryAxis();
    axisX->setCategories({"dummy"});
    chart()->addAxis(axisX, Qt::AlignBottom);
    axisY = new QValueAxis();
    axisY->setTitleText(tr("Restextrakt [°P]"));
    axisY->setMin(0);
    axisY->setMax(100);
    chart()->addAxis(axisY, Qt::AlignLeft);

    setExtrakt = new QBarSet("Restextrakt");
    setExtrakt->setColor(QColor(232,163,95));
    *setExtrakt << 0 << 0 << 0 << 0;

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(setExtrakt);
    chart()->addSeries(barSeries);
    barSeries->setLabelsVisible(true);
    barSeries->setLabelsAngle(-90);
    barSeries->setLabelsPrecision(2);
    barSeries->attachAxis(axisX);
    barSeries->attachAxis(axisY);

    chart()->legend()->setVisible(false);
  #endif
}

void ChartViewRestextrakt::update()
{
  #ifdef QT_CHARTS_LIB
    if (bh->sud()->getSchnellgaerprobeAktiv())
    {
        if (axisX->count() != 4)
        {
            axisX->setCategories({tr("Schnellgärprobe"), tr("Gemessen"), tr("Erwartet"), tr("Rezept")});
        }
        setExtrakt->replace(0, bh->sud()->getSWSchnellgaerprobe());
        setExtrakt->replace(1, bh->sud()->getSWJungbier());
        setExtrakt->replace(2, bh->sud()->getSREErwartet());
        setExtrakt->replace(3, bh->sud()->getSRESoll());
    }
    else
    {
        if (axisX->count() != 3)
        {
            axisX->setCategories({tr("Gemessen"), tr("Erwartet"), tr("Rezept")});
        }
        setExtrakt->replace(0, bh->sud()->getSWJungbier());
        setExtrakt->replace(1, bh->sud()->getSREErwartet());
        setExtrakt->replace(2, bh->sud()->getSRESoll());
    }

    double maxVal = 0;
    for (int i = 0; i < setExtrakt->count(); i++)
    {
        double val = setExtrakt->at(i);
        if (val > maxVal)
            maxVal = val;
    }
    axisY->setMax(int(maxVal)+1);
  #endif
}
