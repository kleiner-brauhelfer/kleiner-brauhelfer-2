#include "chartabfuelldaten.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartAbfuelldaten::ChartAbfuelldaten(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, tr("Anstellen"));
    textTicker->addTick(2, tr("Jungbier"));
    textTicker->addTick(3, tr("Abgefüllt"));
    textTicker->addTick(4, tr("Rezept"));
    xAxis->setTicker(textTicker);
    xAxis->setRange(0, 5);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Würzemenge (L)"));

    bars = new QCPBars(xAxis, yAxis);
    bars->setPen(Qt::NoPen);
    bars->setBrush(QColor(32,159,223));

    textLabel1 = new QCPItemText(this);
    textLabel1->position->setAxes(xAxis,yAxis);
    textLabel1->setPen(Qt::NoPen);
    textLabel1->setColor(Qt::white);
    textLabel2 = new QCPItemText(this);
    textLabel2->position->setAxes(xAxis,yAxis);
    textLabel2->setPen(Qt::NoPen);
    textLabel2->setColor(Qt::white);
    textLabel3 = new QCPItemText(this);
    textLabel3->position->setAxes(xAxis,yAxis);
    textLabel3->setPen(Qt::NoPen);
    textLabel3->setColor(Qt::white);
    textLabel4 = new QCPItemText(this);
    textLabel4->position->setAxes(xAxis,yAxis);
    textLabel4->setPen(Qt::NoPen);
    textLabel4->setColor(Qt::white);
}

void ChartAbfuelldaten::update()
{
    QLocale locale = QLocale();

    double val1 = bh->sud()->getWuerzemengeAnstellen();
    double val2 = bh->sud()->getJungbiermengeAbfuellen();
    double val3 = bh->sud()->geterg_AbgefuellteBiermenge();
    double val4 = bh->sud()->getMenge();

    bars->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);

    textLabel1->position->setCoords(1, val1/2);
    textLabel1->setText(locale.toString(val1, 'f', 1) + tr(" L"));
    textLabel2->position->setCoords(2, val2/2);
    textLabel2->setText(locale.toString(val2, 'f', 1) + tr(" L"));
    textLabel3->position->setCoords(3, val3/2);
    textLabel3->setText(locale.toString(val3, 'f', 1) + tr(" L"));
    textLabel4->position->setCoords(4, val4/2);
    textLabel4->setText(locale.toString(val4, 'f', 1) + tr(" L"));

    double maxVal = qMax(val1, qMax(val2, qMax(val3, val4)));
    yAxis->setRange(0, int(maxVal/10)*10+10);

    replot();
}
