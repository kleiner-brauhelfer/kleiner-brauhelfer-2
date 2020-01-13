#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QTimeEdit>

class TimeEdit : public QTimeEdit
{
public:
    TimeEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    void setError(bool e);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool mError;
};

#endif // DATETIMEEDIT_H
