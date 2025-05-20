#ifndef WDGRAST_H
#define WDGRAST_H

#include "wdgabstractproxy.h"

namespace Ui {
class WdgRast;
}

class WdgRast : public WdgAbstractProxy
{
    Q_OBJECT

public:
    explicit WdgRast(int row, QLayout *parentLayout, QWidget *parent = nullptr);
    ~WdgRast();
    bool isEnabled() const;
    QString name() const;
    double prozentWasser() const;
    void setFehlProzentWasser(double value);
    double prozentMalz() const;
    void setFehlProzentMalz(double value);

public slots:
    void updateListe();
    void updateValues();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void cbRastTextEdited();
    void on_cbRast_currentTextChanged(const QString &text);
    void on_cbRast_currentIndexChanged(int index);
    void on_cbTyp_currentIndexChanged(int index);
    void on_tbMengeWasser_valueChanged(double value);
    void on_tbAnteilWasser_valueChanged(double value);
    void on_tbTempWasser_valueChanged(double value);
    void on_tbMengeMalz_valueChanged(double value);
    void on_tbAnteilMalz_valueChanged(double value);
    void on_btnAnteilMalz_clicked();
    void on_tbTempMalz_valueChanged(double value);
    void on_tbMengeMaische_valueChanged(double value);
    void on_tbAnteilMaische_valueChanged(double value);
    void on_tbDauerExtra2_valueChanged(int value);
    void on_tbTempExtra2_valueChanged(double value);
    void on_tbDauerExtra1_valueChanged(int value);
    void on_tbTempExtra1_valueChanged(double value);
    void on_tbDauerRast_valueChanged(int value);
    void on_tbTempRast_valueChanged(double value);
    void on_btnLoeschen_clicked();
    void on_btnNachUnten_clicked();
    void on_btnNachOben_clicked();

private:
    struct Rast
    {
        QString name;
        double temperatur;
        double dauer;
    };
    static const QList<Rast> rasten;

private:
    void checkEnabled();

private:
    Ui::WdgRast *ui;
    bool mEnabled;
    bool mRastNameManuallyEdited;
};

#endif // WDGRAST_H
