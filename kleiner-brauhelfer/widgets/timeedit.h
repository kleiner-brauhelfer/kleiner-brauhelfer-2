#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QTimeEdit>
#include "widgetdecorator.h"

class TimeEdit : public QTimeEdit, public WidgetDecorator
{
    Q_OBJECT

public:
    TimeEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
private slots:
    void on_valueChanged();
private:
    bool mError;
};

#endif // DATETIMEEDIT_H
