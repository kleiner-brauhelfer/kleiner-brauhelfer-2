#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QTimeEdit>

class TimeEdit : public QTimeEdit
{
    Q_OBJECT

public:
    TimeEdit(QWidget *parent = nullptr);
    void setEnabled(bool e);
    void setDisabled(bool d);
    void setReadOnly(bool r);
    void setError(bool e);
private:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void updatePalette();
private:
    bool mError;
};

#endif // DATETIMEEDIT_H
