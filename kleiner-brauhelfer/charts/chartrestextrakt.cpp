#include "chartrestextrakt.h"
#include "brauhelfer.h"
#include "settings.h"
#include "units.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartRestextrakt::ChartRestextrakt(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, tr("Schnellgärprobe"));
    textTicker->addTick(2, tr("Gemessen"));
    textTicker->addTick(3, tr("Erwartet"));
    textTicker->addTick(4, tr("Rezept"));
    xAxis->setTicker(textTicker);
    xAxis->setRange(0,5);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Restextrakt (%1)").arg(Units::text(Units::GravityUnit())));

    bars = new QCPBars(xAxis, yAxis);
    bars->setPen(Qt::NoPen);
    bars->setBrush(QColor(232,163,95));

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

void ChartRestextrakt::update()
{
    QLocale locale = QLocale();
    Units::Unit grvunit = Units::GravityUnit();
    double coordC = 0;

    if(grvunit == Units::SG) {
        coordC = 0.5;
    }
    double val1 = 0;
    double val2 = Units::convert(Units::Plato, grvunit, bh->sud()->getSWJungbier());
    double val3 = Units::convert(Units::Plato, grvunit, bh->sud()->getSREErwartet());
    double val4 = Units::convert(Units::Plato, grvunit, bh->sud()->getSRESoll());

    if (bh->sud()->getSchnellgaerprobeAktiv())
    {
        xAxis->setRange(0, 5);
        val1 = Units::convert(Units::Plato, grvunit, bh->sud()->getSWSchnellgaerprobe());
        textLabel1->position->setCoords(1, (val1/2)+coordC);
        textLabel1->setText(Units::toString(grvunit, val1, true, locale));
        textLabel1->setVisible(true);
    }
    else
    {
        xAxis->setRange(1.001, 5);
        textLabel1->setVisible(false);
    }

    bars->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);

    textLabel2->position->setCoords(2,(val2/2)+coordC);
    textLabel2->setText(Units::toString(grvunit, val2, true, locale));
    textLabel3->position->setCoords(3, (val3/2)+coordC);
    textLabel3->setText(Units::toString(grvunit, val3, true, locale));
    textLabel4->position->setCoords(4, (val4/2)+coordC);
    textLabel4->setText(Units::toString(grvunit, val4, true, locale));

    double maxVal = qMax(val1, qMax(val2, qMax(val3, val4)));
    if(grvunit == Units::SG)
        yAxis->setRange(1.000,maxVal+(maxVal/100));
    else
        yAxis->setRange(0, int(maxVal)+1);

    replot();
}
