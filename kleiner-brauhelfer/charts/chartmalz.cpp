#include "chartmalz.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartMalz::ChartMalz(QWidget *parent) :
    ChartBase(parent)
{
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    xAxis->setTicker(textTicker);
    xAxis->setTickPen(Qt::NoPen);
    yAxis->setLabel(tr("Anteil (%)"));
    yAxis2->setLabel(tr("Menge (kg)"));
    yAxis2->setVisible(true);
}

void ChartMalz::update()
{
    double yMax = 100;

    clearPlottables();
    clearItems();

    QCPBars *bars = new QCPBars(xAxis, yAxis);
    bars->setPen(Qt::NoPen);
    bars->setBrush(gSettings->colorMalz);

    QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(xAxis->ticker());
    textTicker.data()->clear();
    ProxyModel* model = bh->sud()->modelMalzschuettung();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        double val = model->data(row, ModelMalzschuettung::ColProzent).toDouble();
        textTicker.data()->addTick(row+1, model->data(row, ModelMalzschuettung::ColName).toString());
        bars->addData(row+1, val);
        yMax = qMax(yMax, val);
    }
    yAxis->setRange(0, int(yMax)+1);
    yAxis2->setRange(0, (int(yMax)+1)*bh->sud()->geterg_S_Gesamt()/100);
    xAxis->setRange(0, model->rowCount()+1);
    replot();
}
