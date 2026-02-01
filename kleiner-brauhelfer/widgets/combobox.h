#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    ComboBox(QWidget *parent = nullptr);
    void setError(bool e);
    void setToolTip(const QString &str);
private:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void updatePalette();
private:
    bool mError;
};

#endif // COMBOBOX_H
