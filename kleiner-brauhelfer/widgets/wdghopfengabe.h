#ifndef WDGHOPFENGABE_H
#define WDGHOPFENGABE_H

#include "wdgabstractproxy.h"

namespace Ui {
class WdgHopfenGabe;
}

class WdgHopfenGabe : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgHopfenGabe(int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgHopfenGabe();
    bool isEnabled() const;
    bool isValid() const;
    QString name() const;
    double prozent() const;
    void setFehlProzent(double value);

public slots:
    void updateValues();

private slots:
    void on_btnZutat_clicked();
    void on_tbMengeProzent_valueChanged(double value);
    void on_tbAnteilProzent_valueChanged(double value);
    void on_btnMengeKorrektur_clicked();
    void on_btnAnteilKorrektur_clicked();
    void on_tbMenge_valueChanged(double value);
    void on_tbMengeProLiter_valueChanged(double value);
    void on_tbKochdauer_valueChanged(int dauer);
    void on_cbZeitpunkt_currentIndexChanged(int index);
    void on_btnLoeschen_clicked();
    void on_btnAufbrauchen_clicked();
    void on_btnNachOben_clicked();
    void on_btnNachUnten_clicked();

private:
    void checkEnabled();

private:
    Ui::WdgHopfenGabe *ui;
    bool mEnabled;
    bool mValid;
    double mFehlProzent;
};

#endif // WDGHOPFENGABE_H
