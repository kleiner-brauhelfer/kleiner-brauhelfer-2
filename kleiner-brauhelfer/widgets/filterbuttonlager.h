#ifndef FILTERBUTTONLAGER_H
#define FILTERBUTTONLAGER_H

#include "toolbutton.h"
#include "settings.h"
#include <QMouseEvent>

class ProxyModelRohstoff;
class RadioButton;

class FilterButtonLager : public ToolButton
{
    Q_OBJECT

public:
    FilterButtonLager(QWidget *parent = nullptr);
    ProxyModelRohstoff* model() const;
    void setModel(ProxyModelRohstoff* model);

public slots:
    void clear();

private slots:
    void modulesChanged(Settings::Modules modules);
    void filterChanged();
    void updateFilter();

private:
    void updateChecked();
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    ProxyModelRohstoff* mModel;
    RadioButton* mRadioButtonAlle;
    RadioButton* mRadioButtonVorhanden;
    RadioButton* mRadioButtonNichtVorhanden;
    RadioButton* mRadioButtonInGebrauch;
};

#endif // FILTERBUTTONLAGER_H
