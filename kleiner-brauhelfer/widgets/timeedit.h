#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QTimeEdit>

class TimeEdit : public QTimeEdit
{
    Q_OBJECT

public:
    TimeEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
};

#endif // DATETIMEEDIT_H
