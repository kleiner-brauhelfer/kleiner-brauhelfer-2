#ifndef DATETIMEEDIT_H
#define DATETIMEEDIT_H

#include <QDateTimeEdit>

class DateTimeEdit : public QDateTimeEdit
{
public:
    DateTimeEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    bool hasFocus() const;
    void setError(bool e);
private:
    void updatePalette();
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool mError;
};

#endif // DATETIMEEDIT_H
