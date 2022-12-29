#ifndef CHARTVIEWRESTEXTRAKT_H
#define CHARTVIEWRESTEXTRAKT_H

#include "chartview.h"

class ChartViewRestextrakt : public ChartView
{
    Q_OBJECT
public:
    explicit ChartViewRestextrakt(QWidget *parent = nullptr);
public slots:
    void update() Q_DECL_OVERRIDE;
private:
    QBarSet* setExtrakt;
    QBarCategoryAxis* axisX;
    QValueAxis* axisY;
};

#endif // CHARTVIEWRESTEXTRAKT_H
