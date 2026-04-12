#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QRadioButton>

class RadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit RadioButton(QWidget *parent = nullptr);
    RadioButton(const QString &text, QWidget *parent = nullptr);
    void addChangeDecorator();
    void setError(bool e);
private:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void updatePalette();
private:
    bool mError;
};

#endif // RADIOBUTTON_H
