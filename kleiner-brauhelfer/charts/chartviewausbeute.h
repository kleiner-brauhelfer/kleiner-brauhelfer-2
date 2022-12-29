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
    QBarSet* setAusbeute;
};

#endif // CHARTVIEWAUSBEUTE_H
