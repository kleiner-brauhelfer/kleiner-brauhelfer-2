#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit ComboBox(QWidget *parent = nullptr);
    void setError(bool e);
    void setToolTip(const QString &str);
private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
};

#endif // COMBOBOX_H
