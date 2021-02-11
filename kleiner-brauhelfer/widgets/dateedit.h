#ifndef DATEEDIT_H
#define DATEEDIT_H

#include <QDateEdit>
#include "widgetdecorator.h"

class DateEdit : public QDateEdit, public WidgetDecorator
{
    Q_OBJECT

public:
    DateEdit(QWidget *parent = nullptr);
    void setReadOnly(bool r);
    bool hasFocus() const;
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

#endif // DATEEDIT_H
