#ifndef DATEEDIT_H
#define DATEEDIT_H

#include <QDateEdit>

class DateEdit : public QDateEdit
{
    Q_OBJECT

public:
    DateEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    bool hasFocus() const;
    void setError(bool e);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    bool mError;
};

#endif // DATEEDIT_H
