#ifndef WDGMALZGABE_H
#define WDGMALZGABE_H

#include "wdgabstractproxy.h"

namespace Ui {
class WdgMalzGabe;
}

class WdgMalzGabe : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgMalzGabe(int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgMalzGabe();
    bool isEnabled() const;
    bool isValid() const;
    QString name() const;
    double prozent() const;
    double fehlProzent() const;
    void setFehlProzent(double value);

public slots:
    void updateValues(bool full = false);

private slots:
    void on_btnZutat_clicked();
    void on_tbMengeProzent_valueChanged(double value);
    void on_tbMenge_valueChanged(double value);
    void on_tbExtrakt_valueChanged(double value);
    void on_tbExtraktProzent_valueChanged(double value);
    void on_btnLoeschen_clicked();
    void on_btnKorrektur_clicked();
    void on_btnAufbrauchen_clicked();
    void on_btnNachOben_clicked();
    void on_btnNachUnten_clicked();

private:
    void checkEnabled(bool force);

private:
    Ui::WdgMalzGabe *ui;
    bool mEnabled;
    bool mValid;
};

#endif // WDGMALZGABE_H
