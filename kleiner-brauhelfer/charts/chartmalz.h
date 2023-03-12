#ifndef CHARTMALZ_H
#define CHARTMALZ_H

#include "chartbase.h"

class ChartMalz : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartMalz(QWidget *parent = nullptr);

public slots:
    void update();
};

#endif // CHARTMALZ_H
