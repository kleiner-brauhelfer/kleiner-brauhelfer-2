#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include "widgetdecorator.h"

class ComboBox : public QComboBox, public WidgetDecorator
{
    Q_OBJECT

public:
    ComboBox(QWidget *parent = nullptr);
    void setError(bool e);
    void setToolTip(const QString &str);
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

#endif // COMBOBOX_H
