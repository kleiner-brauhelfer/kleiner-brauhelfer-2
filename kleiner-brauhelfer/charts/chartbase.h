#ifndef CHARTBASE_H
#define CHARTBASE_H

#include "qcustomplot.h"

class ChartBase : public QCustomPlot
{
    Q_OBJECT

public:
    explicit ChartBase(QWidget *parent = nullptr);

};

#endif // CHARTBASE_H
