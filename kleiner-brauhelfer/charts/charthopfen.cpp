#include "charthopfen.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartHopfen::ChartHopfen(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    xAxis->setTicker(textTicker);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Anteil (%)"));
}

void ChartHopfen::update()
{
    double yMax = 100;

    clearPlottables();
    clearItems();

    QCPBars *bars = new QCPBars(xAxis, yAxis);
    bars->setPen(Qt::NoPen);
    bars->setBrush(gSettings->colorHopfen);

    QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(xAxis->ticker());
    textTicker.data()->clear();
    ProxyModel* model = bh->sud()->modelHopfengaben();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        double val = model->data(row, ModelHopfengaben::ColProzent).toDouble();
        textTicker.data()->addTick(row+1, model->data(row, ModelHopfengaben::ColName).toString());
        bars->addData(row+1, val);
        yMax = qMax(yMax, val);
    }
    yAxis->setRange(0, int(yMax)+1);
    xAxis->setRange(0, model->rowCount()+1);
    replot();
}
