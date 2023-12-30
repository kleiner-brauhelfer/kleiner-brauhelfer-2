#include "chartmalz.h"
#include "biercalc.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

ChartMalz::ChartMalz(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    xAxis->setTicker(textTicker);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Anteil") + " (%)");
    yAxis2->setLabel(tr("Menge") + " (kg)");
    yAxis2->setVisible(true);
}

void ChartMalz::update()
{
    double yMax = 100;

    clearPlottables();
    clearItems();

    QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(xAxis->ticker());
    textTicker->clear();
    ProxyModel* model = bh->sud()->modelMalzschuettung();
    int nRows = model->rowCount();
    for (int row = 0; row < nRows; ++row)
    {
        QString name = model->data(row, ModelMalzschuettung::ColName).toString();
        double val = model->data(row, ModelMalzschuettung::ColProzent).toDouble();
        double ebc = model->data(row, ModelMalzschuettung::ColFarbe).toDouble();
        QCPBars *bars = new QCPBars(xAxis, yAxis);
        bars->setPen(Qt::NoPen);
        bars->setBrush(QBrush(BierCalc::ebcToColor(ebc)));
        bars->addData(row+1, val);
        if (nRows <= 3 || row % 2 == 0)
            textTicker->addTick(row+1, name);
        else
            textTicker->addTick(row+1, "\n" + name);
        yMax = qMax(yMax, val);
    }
    yAxis->setRange(0, std::ceil(yMax));
    yAxis2->setRange(0, yAxis->range().upper * bh->sud()->geterg_S_Gesamt()/100);
    xAxis->setRange(0, model->rowCount()+1);
    replot();
}
