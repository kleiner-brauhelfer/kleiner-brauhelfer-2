#ifndef CHARTVIEWAUSBEUTE_H
#define CHARTVIEWAUSBEUTE_H

#include "chartview.h"

class ChartViewAusbeute : public ChartView
{
    Q_OBJECT
public:
    explicit ChartViewAusbeute(QWidget *parent = nullptr);
public slots:
    void update() Q_DECL_OVERRIDE;
private:
  #ifdef QT_CHARTS_LIB
    QBarSet* setAusbeute;
  #endif
};

#endif // CHARTVIEWAUSBEUTE_H
