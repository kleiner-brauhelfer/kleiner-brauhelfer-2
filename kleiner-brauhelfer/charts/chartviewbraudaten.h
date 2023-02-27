#ifndef CHARTVIEWBRAUDATEN_H
#define CHARTVIEWBRAUDATEN_H

#include "chartview.h"

class ChartViewBraudaten : public ChartView
{
    Q_OBJECT
public:
    explicit ChartViewBraudaten(QWidget *parent = nullptr);
public slots:
    void update() Q_DECL_OVERRIDE;
private:
  #ifdef QT_CHARTS_LIB
    QBarSet* setMengeIst;
    QBarSet* setMengeSoll;
    QLineSeries* seriesSwIst;
    QLineSeries* seriesSwSoll;
    QValueAxis* axisY;
    QValueAxis* axisY2;
  #endif
};

#endif // CHARTVIEWBRAUDATEN_H
