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
    Brauhelfer::BerechnungsartHopfen art = (Brauhelfer::BerechnungsartHopfen)bh->sud()->getberechnungsArtHopfen();
    double yMax = (art == Brauhelfer::BerechnungsartHopfen::Keine) ? 0 : 100;
    double sum = 0;

    clearPlottables();
    clearItems();

    QSharedPointer<QCPAxisTickerText> textTicker = qSharedPointerDynamicCast<QCPAxisTickerText>(xAxis->ticker());
    textTicker->clear();
    ProxyModel* model = bh->sud()->modelHopfengaben();
    int nRows = model->rowCount();
    for (int row = 0; row < nRows; ++row)
    {
        QString name = model->data(row, ModelHopfengaben::ColName).toString();
        double val;
        switch (art)
        {
        case Brauhelfer::BerechnungsartHopfen::Keine:
            val = model->data(row, ModelHopfengaben::Colerg_Menge).toDouble();
            break;
        case Brauhelfer::BerechnungsartHopfen::Gewicht:
            val = model->data(row, ModelHopfengaben::ColProzent).toDouble();
            sum += model->data(row, ModelHopfengaben::Colerg_Menge).toDouble();
            break;
        case Brauhelfer::BerechnungsartHopfen::IBU:
            val = model->data(row, ModelHopfengaben::ColProzent).toDouble();
            break;
        }
        int typ = bh->modelHopfen()->getValueFromSameRow(ModelHopfen::ColName, name, ModelHopfen::ColTyp).toInt();
        QCPBars *bars = new QCPBars(xAxis, yAxis);
        bars->setPen(Qt::NoPen);
        if (typ >= 0 && typ < gSettings->HopfenTypBackgrounds.count())
            bars->setBrush(gSettings->HopfenTypBackgrounds[typ]);
        else
            bars->setBrush(gSettings->colorHopfen);
        bars->addData(row+1, val);
        if (nRows <= 3 || row % 2 == 0)
            textTicker->addTick(row+1, name);
        else
            textTicker->addTick(row+1, "\n" + name);
        yMax = qMax(yMax, val);
    }
    switch (art)
    {
    case Brauhelfer::BerechnungsartHopfen::Keine:
        yAxis->setLabel(tr("Menge") + " (g)");
        yAxis->setRange(0, std::ceil(yMax));
        yAxis2->setVisible(false);
        break;
    case Brauhelfer::BerechnungsartHopfen::Gewicht:
        yAxis->setLabel(tr("Anteil") + " (%)");
        yAxis->setRange(0, std::ceil(yMax));
        yAxis2->setLabel(tr("Menge") + " (g)");
        yAxis2->setRange(0, yAxis->range().upper * sum/100);
        yAxis2->setVisible(true);
        break;
    case Brauhelfer::BerechnungsartHopfen::IBU:
        yAxis->setLabel(tr("Anteil") + " (%)");
        yAxis->setRange(0, std::ceil(yMax));
        yAxis2->setLabel(tr("Bittere") + " (IBU)");
        yAxis2->setRange(0, yAxis->range().upper * bh->sud()->getIBU()/100);
        yAxis2->setVisible(true);
        break;
    }
    xAxis->setRange(0, model->rowCount()+1);
    replot();
}
