#ifndef FILTERBUTTONLAGER_H
#define FILTERBUTTONLAGER_H

#include <QToolButton>
#include "settings.h"

class ProxyModelRohstoff;
class QRadioButton;

class FilterButtonLager : public QToolButton
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

private:
    ProxyModelRohstoff* mModel;
    QRadioButton* mRadioButtonAlle;
    QRadioButton* mRadioButtonVorhanden;
    QRadioButton* mRadioButtonNichtVorhanden;
    QRadioButton* mRadioButtonInGebrauch;
};

#endif // FILTERBUTTONLAGER_H
