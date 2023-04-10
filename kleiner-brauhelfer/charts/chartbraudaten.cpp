#include "chartbraudaten.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartBraudaten::ChartBraudaten(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, tr("Kochbeginn"));
    textTicker->addTick(2, tr("Kochende"));
    textTicker->addTick(3, tr("Nach Hopfenseihen"));
    textTicker->addTick(4, tr("Anstellen"));
    xAxis->setTicker(textTicker);
    xAxis->setRange(0, 5);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Würzemenge (L)"));
    yAxis2->setLabel(tr("Stammwürze (")+gSettings->GravityUnit()+tr(")"));
    yAxis2->setVisible(true);

    barsMengeIst = new QCPBars(xAxis, yAxis);
    barsMengeIst->setPen(Qt::NoPen);
    barsMengeIst->setName(tr("Menge"));
    barsMengeIst->setBrush(QColor(32,159,223));
    barsMengeSoll = new QCPBars(xAxis, yAxis);
    barsMengeSoll->setName(tr("Menge Rezept"));
    barsMengeSoll->setPen(Qt::NoPen);
    //setMengeSoll->setColor(setMengeIst->color().darker(200));

    graphSwSoll = new QCPGraph(xAxis, yAxis2);
    graphSwSoll->setName(tr("SW Rezept"));
    graphSwSoll->setPen(QPen(QColor(232,163,95).darker(200), 2, Qt::DashLine));
    graphSwIst = new QCPGraph(xAxis, yAxis2);
    graphSwIst->setName(tr("SW"));
    graphSwIst->setPen(QPen(QColor(232,163,95), 2, Qt::SolidLine));
    //seriesSwSoll->setColor(seriesSwIst->color());

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

    legend->setVisible(true);
}

void ChartBraudaten::update()
{
    QLocale locale = QLocale();
    BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
    double val1 = bh->sud()->getWuerzemengeKochbeginn();
    double val2 = bh->sud()->getWuerzemengeVorHopfenseihen();
    double val3 = bh->sud()->getWuerzemengeKochende();
    double val4 = bh->sud()->getWuerzemengeAnstellen();

    barsMengeIst->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);
    double maxVal = qMax(val1, qMax(val2, qMax(val3, val4)));

    textLabel1->position->setCoords(1, val1/2);
    textLabel1->setText(locale.toString(val1, 'f', 1) + tr(" L"));
    textLabel2->position->setCoords(2, val2/2);
    textLabel2->setText(locale.toString(val2, 'f', 1) + tr(" L"));
    textLabel3->position->setCoords(3, val3/2);
    textLabel3->setText(locale.toString(val3, 'f', 1) + tr(" L"));
    textLabel4->position->setCoords(4, val4/2);
    textLabel4->setText(locale.toString(val4, 'f', 1) + tr(" L"));

    val1 = bh->sud()->getMengeSollKochbeginn();
    val2 = bh->sud()->getMengeSollKochende();
    val3 = bh->sud()->getMengeSollKochende();
    val4 = bh->sud()->getMenge();
    barsMengeSoll->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);
    maxVal = qMax(maxVal, qMax(val1, qMax(val2, qMax(val3, val4))));

    yAxis->setRange(0, int(maxVal/10)*10+10);

    val1 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWKochbeginn());
    val2 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWKochende());
    val3 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWKochende());
    val4 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWAnstellen());
    graphSwIst->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);
    maxVal = qMax(val1, qMax(val2, qMax(val3, val4)));
    val1 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWSollKochbeginn());
    val2 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWSollKochende());
    val3 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWSollKochende());
    val4 = BierCalc::convertGravity(BierCalc::GravityUnit::Plato,grvunit,bh->sud()->getSWSollAnstellen());
    graphSwSoll->setData({1, 2, 3, 4}, {val1, val2, val3, val4}, true);
    maxVal = qMax(maxVal, qMax(val1, qMax(val2, qMax(val3, val4))));
    if (grvunit == BierCalc::GravityUnit::SG) {
        yAxis2->setRange(1.000, maxVal+(maxVal/10));
    } else {
    yAxis2->setRange(0, int(maxVal/10)*10+10);
    }
    replot();
}
