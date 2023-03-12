#ifndef CHART3_H
#define CHART3_H

#include "qcustomplot.h"

class Chart3 : public QCustomPlot
{
    Q_OBJECT

public:
    explicit Chart3(QWidget *parent = nullptr);
    void clear();
    void setData1(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision);
    void setData2(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision);
    void setData3(const QVector<double>& x, const QVector<double>& y, const QString& label, const QString& unit, double precision);
    void setData1Limit(double value);
    void rescale();
    void select(int index);

signals:
    void selectionChanged(int index);

private slots:
    void onMousePress(QMouseEvent *event);
    void onMouseDoubleClick(QMouseEvent *event);

private:
    QCPAxis *yAxis3;
    QCPAxis *yAxisSelection;
    QCPGraph *graph1;
    QCPGraph *graph2;
    QCPGraph *graph3;
    QCPGraph *graph1Limit;
    QCPGraph *graphSelection;
    QCPItemText *lblValueX;
    QCPItemText *lblValueY1;
    QCPItemText *lblValueY2;
    QCPItemText *lblValueY3;
    QString unit1;
    QString unit2;
    QString unit3;
    int precision1;
    int precision2;
    int precision3;
};

#endif // CHART3_H
