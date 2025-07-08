#ifndef WDGMALZGABE_H
#define WDGMALZGABE_H

#include "wdgabstractproxy.h"

class SudObject;

namespace Ui {
class WdgMalzGabe;
}

class WdgMalzGabe : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgMalzGabe(SudObject *sud, int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgMalzGabe();
    bool isEnabled() const;
    bool isValid() const;
    QString name() const;
    double prozent() const;
    double fehlProzent() const;
    void setFehlProzent(double value);
    double prozentExtrakt() const;
    double fehlProzentExtrakt() const;
    void setFehlProzentExtrakt(double value);

public slots:
    void updateValues();

private slots:
    void on_btnZutat_clicked();
    void on_tbMengeProzent_valueChanged(double value);
    void on_tbMenge_valueChanged(double value);
    void on_tbExtrakt_valueChanged(double value);
    void on_tbExtraktProzent_valueChanged(double value);
    void on_btnLoeschen_clicked();
    void on_btnKorrektur_clicked();
    void on_btnKorrekturExtrakt_clicked();
    void on_btnAufbrauchen_clicked();
    void on_btnNachOben_clicked();
    void on_btnNachUnten_clicked();

private:
    void checkEnabled();

private:
    Ui::WdgMalzGabe *ui;
    SudObject *mSud;
    bool mEnabled;
    bool mValid;
    double mFehlProzent;
    double mFehlProzentExtrakt;
};

#endif // WDGMALZGABE_H
