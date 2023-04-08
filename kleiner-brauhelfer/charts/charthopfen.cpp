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
}

void ChartHopfen::update()
{
    double yMax, yVal;

    clearPlottables();
    clearItems();

    QCPBars *bars = new QCPBars(xAxis, yAxis);
    bars->setPen(Qt::NoPen);
    bars->setBrush(gSettings->colorHopfen);

    QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(xAxis->ticker());
    textTicker.data()->clear();
    ProxyModel* model = bh->sud()->modelHopfengaben();
    switch ((Brauhelfer::BerechnungsartHopfen)bh->sud()->getberechnungsArtHopfen())
    {
    case Brauhelfer::BerechnungsartHopfen::Keine:
        yMax = 0;
        for (int row = 0; row < model->rowCount(); ++row)
        {
            yVal = model->data(row, ModelHopfengaben::Colerg_Menge).toDouble();
            textTicker.data()->addTick(row+1, model->data(row, ModelHopfengaben::ColName).toString());
            bars->addData(row+1, yVal);
            yMax = qMax(yMax, yVal);
        }
        yAxis->setLabel(tr("Menge (g)"));
        yAxis->setRange(0, yMax);
        yAxis2->setVisible(false);
        break;
    case Brauhelfer::BerechnungsartHopfen::Gewicht:
        yMax = 100;
        for (int row = 0; row < model->rowCount(); ++row)
        {
            yVal = model->data(row, ModelHopfengaben::ColProzent).toDouble();
            textTicker.data()->addTick(row+1, model->data(row, ModelHopfengaben::ColName).toString());
            bars->addData(row+1, yVal);
            yMax = qMax(yMax, yVal);
        }
        yAxis->setLabel(tr("Anteil (%)"));
        yAxis->setRange(0, int(yMax)+1);
        yAxis2->setVisible(false);
        break;
    case Brauhelfer::BerechnungsartHopfen::IBU:
        yMax = 100;
        for (int row = 0; row < model->rowCount(); ++row)
        {
            yVal = model->data(row, ModelHopfengaben::ColProzent).toDouble();
            textTicker.data()->addTick(row+1, model->data(row, ModelHopfengaben::ColName).toString());
            bars->addData(row+1, yVal);
            yMax = qMax(yMax, yVal);
        }
        yAxis->setLabel(tr("Anteil (%)"));
        yAxis2->setLabel(tr("Bittere (IBU)"));
        yAxis->setRange(0, int(yMax)+1);
        yAxis2->setRange(0, (int(yMax)+1)*bh->sud()->getIBU()/100);
        yAxis2->setVisible(true);
        break;
    }
    xAxis->setRange(0, model->rowCount()+1);
    replot();
}
