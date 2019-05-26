#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))

#include <QtCharts>
class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr);
};

#else

#include <QGraphicsView>
class ChartView : public QGraphicsView
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr) : QGraphicsView(parent) {};
};

#endif

#endif // CHARTVIEW_H
