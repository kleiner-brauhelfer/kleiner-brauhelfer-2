#ifndef CHARTABFUELLDATEN_H
#define CHARTABFUELLDATEN_H

#include "chartbase.h"

class SudObject;

class ChartAbfuelldaten : public ChartBase
{
    Q_OBJECT
public:
    explicit ChartAbfuelldaten(QWidget *parent = nullptr);

public slots:
    void update(const SudObject* sud);

private:
    QCPBars *bars;
    QCPItemText *textLabel1;
    QCPItemText *textLabel2;
    QCPItemText *textLabel3;
    QCPItemText *textLabel4;
};

#endif // CHARTABFUELLDATEN_H
