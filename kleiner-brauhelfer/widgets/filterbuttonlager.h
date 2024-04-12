#ifndef FILTERBUTTONLAGER_H
#define FILTERBUTTONLAGER_H

#include "toolbutton.h"

class ProxyModelRohstoff;
class RadioButton;

class FilterButtonLager : public ToolButton
{
    Q_OBJECT

public:
    FilterButtonLager(QWidget *parent = nullptr);
    ProxyModelRohstoff* model() const;
    void setModel(ProxyModelRohstoff* model);

private slots:
    void updateFilter();

private:
    void updateChecked();

private:
    ProxyModelRohstoff* mModel;
    RadioButton* mRadioButtonAlle;
    RadioButton* mRadioButtonVorhanden;
    RadioButton* mRadioButtonNichtVorhanden;
    RadioButton* mRadioButtonInGebrauch;
};

#endif // FILTERBUTTONLAGER_H
