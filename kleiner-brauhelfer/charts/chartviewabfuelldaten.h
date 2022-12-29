#ifndef CHARTVIEWABFUELLDATEN_H
#define CHARTVIEWABFUELLDATEN_H

#include "chartview.h"

class ChartViewAbfuelldaten : public ChartView
{
    Q_OBJECT
public:
    explicit ChartViewAbfuelldaten(QWidget *parent = nullptr);
public slots:
    void update() Q_DECL_OVERRIDE;
private:
    QBarSet* setMenge;
    QValueAxis* axisY;
};

#endif // CHARTVIEWABFUELLDATEN_H
