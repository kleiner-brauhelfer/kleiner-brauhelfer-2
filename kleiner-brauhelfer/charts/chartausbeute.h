#ifndef CHARTAUSBEUTE_H
#define CHARTAUSBEUTE_H

#include "chartbase.h"

class SudObject;

class ChartAusbeute : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartAusbeute(QWidget *parent = nullptr);

public slots:
    void update(const SudObject* sud);

private:
    QCPBars *bars;
    QCPItemText *textLabel1;
    QCPItemText *textLabel2;
    QCPItemText *textLabel3;
};

#endif // CHARTAUSBEUTE_H
