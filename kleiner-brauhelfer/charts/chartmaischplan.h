#ifndef CHARTMAISCHPLAN_H
#define CHARTMAISCHPLAN_H

#include "chartbase.h"

class SudObject;

class ChartMaischplan : public ChartBase
{
    Q_OBJECT

public:
    explicit ChartMaischplan(QWidget *parent = nullptr);

public slots:
    void update(const SudObject* sud);
};

#endif // CHARTMAISCHPLAN_H
