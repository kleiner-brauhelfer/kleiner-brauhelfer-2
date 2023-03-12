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
    QCPItemText *textLabel1;
    QCPItemText *textLabel2;
    QCPItemText *textLabel3;
    QCPItemText *textLabel4;
};

#endif // CHARTBRAUDATEN_H
