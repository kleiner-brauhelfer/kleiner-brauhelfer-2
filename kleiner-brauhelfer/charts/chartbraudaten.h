#ifndef CHARTBRAUDATEN_H
#define CHARTBRAUDATEN_H

#include "chartbase.h"

class ChartBraudaten : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartBraudaten(QWidget *parent = nullptr);

public slots:
    void update();

private:
    QCPBars *barsMengeIst;
    QCPBars *barsMengeSoll;
    QCPGraph *graphSwIst;
    QCPGraph *graphSwSoll;
};

#endif // CHARTBRAUDATEN_H
