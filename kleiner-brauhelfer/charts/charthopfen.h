#ifndef CHARTHOPFEN_H
#define CHARTHOPFEN_H

#include "chartbase.h"

class SudObject;

class ChartHopfen : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartHopfen(QWidget *parent = nullptr);

public slots:
    void update(const SudObject* sud);
};

#endif // CHARTHOPFEN_H
