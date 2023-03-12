#ifndef CHARTHOPFEN_H
#define CHARTHOPFEN_H

#include "chartbase.h"

class ChartHopfen : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartHopfen(QWidget *parent = nullptr);

public slots:
    void update();
};

#endif // CHARTHOPFEN_H
