#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#ifdef QT_CHARTS_LIB

#include <QtCharts>
class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr);
};

#else // QT_CHARTS_LIB

#include <QGraphicsView>
class ChartView : public QGraphicsView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr) : QGraphicsView(parent) {};
};

#endif // QT_CHARTS_LIB

#endif // CHARTVIEW_H
