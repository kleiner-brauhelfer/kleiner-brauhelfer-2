#ifndef DATETIMEEDIT_H
#define DATETIMEEDIT_H

#include <QDateTimeEdit>

class DateTimeEdit : public QDateTimeEdit
{
    Q_OBJECT

public:
    explicit DateTimeEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    bool hasFocus() const;
    void setError(bool e);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
};

#endif // DATETIMEEDIT_H
