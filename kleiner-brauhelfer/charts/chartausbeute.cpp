#include "chartausbeute.h"
#include "sudobject.h"

ChartAusbeute::ChartAusbeute(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, tr("SHA"));
    textTicker->addTick(2, tr("Eff. SHA"));
    textTicker->addTick(3, tr("Rezept"));
    xAxis->setTicker(textTicker);
    xAxis->setRange(0, 4);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Ausbeute") + " (%)");
    yAxis->setRange(0, 100);

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
}

void ChartAusbeute::update(const SudObject* sud)
{
    QLocale locale = QLocale();

    double val1 = sud->geterg_Sudhausausbeute();
    double val2 = sud->geterg_EffektiveAusbeute();
    double val3 = sud->getSudhausausbeute();

    bars->setData({1, 2, 3}, {val1, val2, val3}, true);

    textLabel1->position->setCoords(1, val1/2);
    textLabel1->setText(locale.toString(val1, 'f', 1) + " %");
    textLabel2->position->setCoords(2, val2/2);
    textLabel2->setText(locale.toString(val2, 'f', 1) + " %");
    textLabel3->position->setCoords(3, val3/2);
    textLabel3->setText(locale.toString(val3, 'f', 1) + " %");

    replot();
}
