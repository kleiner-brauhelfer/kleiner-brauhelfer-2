#include "chartbraudaten.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartBraudaten::ChartBraudaten(QWidget *parent) :
    ChartBase(parent)
{
    legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    legend->setVisible(true);
    legend->setBorderPen(Qt::NoPen);
    legend->setBrush(Qt::NoBrush);
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    plotLayout()->addElement(1, 0, subLayout);
    subLayout->addElement(0, 0, new QCPLayoutElement);
    subLayout->addElement(0, 1, legend);
    subLayout->addElement(0, 2, new QCPLayoutElement);
    plotLayout()->setRowStretchFactor(1, 0.001);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, tr("Kochbeginn"));
    textTicker->addTick(2, tr("Kochende"));
    textTicker->addTick(3, tr("Nach Hopfenseihen"));
    textTicker->addTick(4, tr("Anstellen"));
    xAxis->setTicker(textTicker);
    xAxis->setRange(0, 5);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Würzemenge (L)"));
    yAxis2->setLabel(tr("Stammwürze (")+gSettings->GravityUnitString()+tr(")"));
    yAxis2->setVisible(true);

    barsMengeIst = new QCPBars(xAxis, yAxis);
    barsMengeIst->setName(tr("Menge"));
    barsMengeIst->setPen(Qt::NoPen);
    barsMengeIst->setBrush(QColor(32,159,223));
    barsMengeIst->setWidthType(QCPBars::WidthType::wtPlotCoords);
    barsMengeIst->setWidth(0.4);
    barsMengeSoll = new QCPBars(xAxis, yAxis);
    barsMengeSoll->setName(tr("Menge Rezept"));
    barsMengeSoll->setBrush(QColor(137,193,221));
    barsMengeSoll->setPen(Qt::NoPen);
    barsMengeSoll->setWidthType(QCPBars::WidthType::wtPlotCoords);
    barsMengeSoll->setWidth(0.4);

    graphSwSoll = new QCPGraph(xAxis, yAxis2);
    graphSwSoll->setName(tr("SW Rezept"));
    graphSwSoll->setPen(QPen(Qt::gray, 3, Qt::DashLine));
    graphSwSoll->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::gray, Qt::white, 6));
    graphSwIst = new QCPGraph(xAxis, yAxis2);
    graphSwIst->setName(tr("SW"));
    graphSwIst->setPen(QPen(QColor(232,163,95), 3, Qt::SolidLine));
    graphSwIst->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(232,163,95), Qt::white, 6));
}

void ChartBraudaten::update()
{
    BierCalc::GravityUnit grvunit = static_cast<BierCalc::GravityUnit>(gSettings->GravityUnit());
    double val1 = bh->sud()->getWuerzemengeKochbeginn();
    double val2 = bh->sud()->getWuerzemengeVorHopfenseihen();
    double val3 = bh->sud()->getWuerzemengeKochende();
    double val4 = bh->sud()->getWuerzemengeAnstellen();

    barsMengeIst->setData({0.8, 1.8, 2.8, 3.8}, {val1, val2, val3, val4}, true);

    double maxVal = qMax(val1, qMax(val2, qMax(val3, val4)));

    val1 = bh->sud()->getMengeSollKochbeginn();
    val2 = bh->sud()->getMengeSollKochende();
    val3 = bh->sud()->getMengeSollKochende();
    val4 = bh->sud()->getMenge();
    barsMengeSoll->setData({1.2, 2.2, 3.2, 4.2}, {val1, val2, val3, val4}, true);
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
