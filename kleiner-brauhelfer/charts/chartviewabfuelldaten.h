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
  #ifdef QT_CHARTS_LIB
    QBarSet* setMenge;
    QValueAxis* axisY;
  #endif
};

#endif // CHARTVIEWABFUELLDATEN_H
