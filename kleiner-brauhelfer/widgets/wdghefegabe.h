#ifndef WDGHEFEGABE_H
#define WDGHEFEGABE_H

#include "wdgabstractproxy.h"

namespace Ui {
class WdgHefeGabe;
}

class WdgHefeGabe : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgHefeGabe(int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgHefeGabe();
    bool isEnabled() const;
    bool isValid() const;
    QString name() const;
    int menge() const;

public slots:
    void updateValues();

private slots:
    void on_btnZutat_clicked();
    void on_tbMenge_valueChanged(int value);
    void on_tbTage_valueChanged(int value);
    void on_tbDatum_dateChanged(const QDate &date);
    void on_btnZugeben_clicked();
    void on_btnLoeschen_clicked();
    void on_btnAufbrauchen_clicked();
    void on_btnNachOben_clicked();
    void on_btnNachUnten_clicked();

private:
    void checkEnabled();

private:
    Ui::WdgHefeGabe *ui;
    bool mEnabled;
    bool mValid;
};

#endif // WDGHEFEGABE_H
