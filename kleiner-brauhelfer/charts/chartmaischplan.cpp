#include "chartmaischplan.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

ChartMaischplan::ChartMaischplan(QWidget *parent) :
    ChartBase(parent)
{
    QColor colRect = gSettings->colorRast;
    colRect.setAlpha(80);

    xAxis->setLabel(tr("Zeit") + " (min)");
    yAxis->setLabel(tr("Temperatur") + " (°C)");

    QCPItemRect* rect = new QCPItemRect(this);
    rect->bottomRight->setAxes(xAxis2, yAxis);
    rect->topLeft->setAxes(xAxis2, yAxis);
    rect->bottomRight->setCoords(1, 70);
    rect->topLeft->setCoords(0, 74);
    rect->setPen(Qt::NoPen);
    rect->setBrush(colRect);

    QCPItemText* text = new QCPItemText(this);
    text->position->setAxes(xAxis2, yAxis);
    text->position->setCoords(0.01, 72);
    text->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    text->setColor(colorText);
    text->setText(tr("α-Amylase"));

    rect = new QCPItemRect(this);
    rect->bottomRight->setAxes(xAxis2, yAxis);
    rect->topLeft->setAxes(xAxis2, yAxis);
    rect->bottomRight->setCoords(1, 60);
    rect->topLeft->setCoords(0, 68);
    rect->setPen(Qt::NoPen);
    rect->setBrush(colRect);

    text = new QCPItemText(this);
    text->position->setAxes(xAxis2, yAxis);
    text->position->setCoords(0.01, 64);
    text->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    text->setColor(colorText);
    text->setText(tr("β-Amylase"));

    xAxis2->setRange(0, 1);
}

void ChartMaischplan::update()
{
    clearPlottables();

    QCPGraph *graph = new QCPGraph(xAxis, yAxis);
    QCPGraph *graphAux;

    double tTotal = 0;
    double TMin = 55;
    double TMax = 75;
    double t, T, T2, lastT = 0, temp;

    graph->setPen(QPen(QBrush(QColor(32,159,223)), 2, Qt::SolidLine));

    ProxyModel* model = bh->sud()->modelMaischplan();
    for (int row = 0; row < model->rowCount(); ++row)
    {
        T = model->data(row, ModelMaischplan::ColTempRast).toInt();
        t = model->data(row, ModelMaischplan::ColDauerRast).toInt();
        switch (static_cast<Brauhelfer::RastTyp>(model->data(row, ModelMaischplan::ColTyp).toInt()))
        {
        case Brauhelfer::RastTyp::Zubruehen:
            graphAux = new QCPGraph(xAxis, yAxis);
            graphAux->setPen(QPen(QBrush(gSettings->DiagramLinie3), 2, Qt::DashLine));
            T2 = model->data(row, ModelMaischplan::ColTempWasser).toInt();
            graphAux->setData({tTotal, tTotal}, {T2, T}, true);
            TMax = 100;
            if (T2 < TMin)
                TMin = T2;
            if (T2 > TMax)
                TMax = T2;
            break;
        case Brauhelfer::RastTyp::Zuschuetten:
            graphAux = new QCPGraph(xAxis, yAxis);
            graphAux->setPen(QPen(QBrush(gSettings->DiagramLinie1), 2, Qt::DashLine));
            T2 = model->data(row, ModelMaischplan::ColTempWasser).toInt();
            graphAux->setData({tTotal, tTotal}, {T2, T}, true);
            TMax = 100;
            if (T2 < TMin)
                TMin = T2;
            if (T2 > TMax)
                TMax = T2;
            break;
        case Brauhelfer::RastTyp::Dekoktion:
            graphAux = new QCPGraph(xAxis, yAxis);
            graphAux->setPen(QPen(QBrush(gSettings->DiagramLinie2), 2, Qt::DashLine));
            graphAux->addData(tTotal, lastT);
            temp = model->data(row, ModelMaischplan::ColDauerExtra2).toInt();
            if (temp > 0)
            {
                int T3 = model->data(row, ModelMaischplan::ColTempExtra2).toInt();
                graphAux->addData(tTotal, T3);
                tTotal += temp;
                graphAux->addData(tTotal, T3);
                if (T3 < TMin)
                    TMin = T3;
                if (T3 > TMax)
                    TMax = T3;
            }
            temp = model->data(row, ModelMaischplan::ColDauerExtra1).toInt();
            if (temp > 0)
            {
                T2 = model->data(row, ModelMaischplan::ColTempExtra1).toInt();
                graphAux->addData(tTotal, T2);
                tTotal += temp;
                graphAux->addData(tTotal, T2);
                TMax = 100;
                if (T2 < TMin)
                    TMin = T2;
                if (T2 > TMax)
                    TMax = T2;
            }
            graphAux->addData(tTotal, T);
            graph->addData(tTotal, lastT);
            break;
        default:
            break;
        }

        graph->addData(tTotal, T);
        tTotal += t;
        graph->addData(tTotal, T);

        if (T < TMin)
            TMin = T;
        if (T > TMax)
            TMax = T;

        lastT = T;
    }

    xAxis->setRange(0, tTotal);
    yAxis->setRange(qCeil(TMin/5)*5-5, qFloor(TMax/5)*5+5);

    replot();
}
