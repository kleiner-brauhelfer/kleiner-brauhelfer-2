#ifndef WDGWASSERAUFBEREITUNG_H
#define WDGWASSERAUFBEREITUNG_H

#include "wdgabstractproxy.h"

namespace Ui {
class WdgWasseraufbereitung;
}

class WdgWasseraufbereitung : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgWasseraufbereitung(int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgWasseraufbereitung();

public slots:
    void updateValues();

private slots:
    void on_tbName_textChanged(const QString &text);
    void on_btnAuswahl_clicked();
    void on_tbMenge_valueChanged(double value);
    void on_tbMengeGesamt_valueChanged(double value);
    void on_tbRestalkalitaet_valueChanged(double value);
    void on_btnNachOben_clicked();
    void on_btnNachUnten_clicked();
    void on_btnLoeschen_clicked();

private:
    void checkEnabled();

private:
    Ui::WdgWasseraufbereitung *ui;
    bool mEnabled;
};

#endif // WDGWASSERAUFBEREITUNG_H
