#include "chartviewausbeute.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

ChartViewAusbeute::ChartViewAusbeute(QWidget *parent) :
    ChartView(parent)
{
  #ifdef QT_CHARTS_LIB

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append({tr("SHA"), tr("Eff. SHA"), tr("Rezept")});
    chart()->addAxis(axisX, Qt::AlignBottom);
    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText(tr("Ausbeute [%]"));
    axisY->setMin(0);
    axisY->setMax(100);
    chart()->addAxis(axisY, Qt::AlignLeft);

    setAusbeute = new QBarSet(tr("SHA"));
    setAusbeute->setColor(QColor(232,163,95));
    *setAusbeute << 0 << 0 << 0;

    QBarSeries* seriesSHA = new QBarSeries();
    seriesSHA->append(setAusbeute);
    chart()->addSeries(seriesSHA);
    seriesSHA->setLabelsVisible(true);
    seriesSHA->setLabelsAngle(-90);
    seriesSHA->setLabelsPrecision(3);
    seriesSHA->attachAxis(axisX);
    seriesSHA->attachAxis(axisY);

    chart()->legend()->setVisible(false);
  #endif
}

void ChartViewAusbeute::update()
{
  #ifdef QT_CHARTS_LIB
    setAusbeute->replace(0, bh->sud()->geterg_Sudhausausbeute());
    setAusbeute->replace(1, bh->sud()->geterg_EffektiveAusbeute());
    setAusbeute->replace(2, bh->sud()->getSudhausausbeute());
  #endif
}
