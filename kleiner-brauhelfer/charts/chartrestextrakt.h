#ifndef CHARTRESTEXTRAKT_H
#define CHARTRESTEXTRAKT_H

#include "chartbase.h"

class ChartRestextrakt : public ChartBase
{
    Q_OBJECT
public:
    explicit ChartRestextrakt(QWidget *parent = nullptr);

public slots:
    void update();

private:
    QCPBars *bars;
    QCPItemText *textLabel1;
    QCPItemText *textLabel2;
    QCPItemText *textLabel3;
    QCPItemText *textLabel4;
};

#endif // CHARTRESTEXTRAKT_H
