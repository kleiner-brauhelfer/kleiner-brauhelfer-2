#ifndef CHARTMALZ_H
#define CHARTMALZ_H

#include "chartbase.h"

class SudObject;

class ChartMalz : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartMalz(QWidget *parent = nullptr);

public slots:
    void update(const SudObject* sud);
};

#endif // CHARTMALZ_H
