#ifndef CHARTMAISCHPLAN_H
#define CHARTMAISCHPLAN_H

#include "chartbase.h"

class ChartMaischplan : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartMaischplan(QWidget *parent = nullptr);

public slots:
    void update();
};

#endif // CHARTMAISCHPLAN_H
