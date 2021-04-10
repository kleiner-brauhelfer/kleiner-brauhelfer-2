#ifndef TABBEWERTUNG_H
#define TABBEWERTUNG_H

#include "tababstract.h"

namespace Ui {
class TabBewertung;
}

class TabBewertung : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabBewertung(QWidget *parent = nullptr);
    virtual ~TabBewertung() Q_DECL_OVERRIDE;

private slots:
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void modelModified();
    void updateValues();
    void clicked(int index);
    void on_btnNeueBewertung_clicked();
    void sterneChanged(int sterne);
    void on_tbDatum_dateChanged(const QDate &date);
    void on_btnDatumHeute_clicked();
    void on_rbFarbe_0_clicked();
    void on_rbFarbe_1_clicked();
    void on_rbFarbe_2_clicked();
    void on_rbFarbe_3_clicked();
    void on_rbFarbe_4_clicked();
    void on_rbFarbe_5_clicked();
    void on_rbFarbe_6_clicked();
    void on_rbFarbe_7_clicked();
    void on_rbFarbe_8_clicked();
    void on_rbFarbe_9_clicked();
    void on_rbFarbe_10_clicked();
    void on_rbFarbe_ka_clicked();
    void on_rbFarbe_ka2_clicked();
    void on_tbFarbe_textChanged();
    void on_rbSchaum_0_clicked();
    void on_rbSchaum_1_clicked();
    void on_rbSchaum_2_clicked();
    void on_rbSchaum_3_clicked();
    void on_rbSchaum_4_clicked();
    void on_rbSchaum_5_clicked();
    void on_rbSchaum_6_clicked();
    void on_rbSchaum_7_clicked();
    void on_rbSchaum_8_clicked();
    void on_rbSchaum_9_clicked();
    void on_rbSchaum_10_clicked();
    void on_rbSchaum_ka_clicked();
    void on_rbSchaum_ka2_clicked();
    void on_rbSchaum_ka3_clicked();
    void on_rbSchaum_ka4_clicked();
    void on_tbSchaum_textChanged();
    void on_rbGeruch_0_clicked();
    void on_rbGeruch_1_clicked();
    void on_rbGeruch_2_clicked();
    void on_rbGeruch_3_clicked();
    void on_rbGeruch_4_clicked();
    void on_rbGeruch_5_clicked();
    void on_rbGeruch_6_clicked();
    void on_rbGeruch_7_clicked();
    void on_rbGeruch_8_clicked();
    void on_rbGeruch_9_clicked();
    void on_rbGeruch_10_clicked();
    void on_rbGeruch_11_clicked();
    void on_tbGeruch_textChanged();
    void on_rbGeschmack_0_clicked();
    void on_rbGeschmack_1_clicked();
    void on_rbGeschmack_2_clicked();
    void on_rbGeschmack_3_clicked();
    void on_rbGeschmack_4_clicked();
    void on_rbGeschmack_5_clicked();
    void on_rbGeschmack_6_clicked();
    void on_rbGeschmack_7_clicked();
    void on_rbGeschmack_8_clicked();
    void on_rbGeschmack_9_clicked();
    void on_rbGeschmack_10_clicked();
    void on_rbGeschmack_11_clicked();
    void on_rbGeschmack_12_clicked();
    void on_tbGeschmack_textChanged();
    void on_rbAntrunk_0_clicked();
    void on_rbAntrunk_1_clicked();
    void on_rbAntrunk_2_clicked();
    void on_rbAntrunk_3_clicked();
    void on_rbAntrunk_4_clicked();
    void on_rbAntrunk_5_clicked();
    void on_rbAntrunk_6_clicked();
    void on_rbAntrunk_7_clicked();
    void on_rbAntrunk_ka_clicked();
    void on_tbAntrunk_textChanged();
    void on_rbHaupttrunk_0_clicked();
    void on_rbHaupttrunk_1_clicked();
    void on_rbHaupttrunk_2_clicked();
    void on_rbHaupttrunk_3_clicked();
    void on_rbHaupttrunk_4_clicked();
    void on_rbHaupttrunk_ka_clicked();
    void on_tbHaupttrunk_textChanged();
    void on_rbNachtrunk_0_clicked();
    void on_rbNachtrunk_1_clicked();
    void on_rbNachtrunk_2_clicked();
    void on_rbNachtrunk_3_clicked();
    void on_rbNachtrunk_4_clicked();
    void on_rbNachtrunk_5_clicked();
    void on_rbNachtrunk_6_clicked();
    void on_rbNachtrunk_7_clicked();
    void on_rbNachtrunk_8_clicked();
    void on_rbNachtrunk_ka_clicked();
    void on_tbNachtrunk_textChanged();
    void on_rbGesamteindruck_0_clicked();
    void on_rbGesamteindruck_1_clicked();
    void on_rbGesamteindruck_2_clicked();
    void on_rbGesamteindruck_3_clicked();
    void on_rbGesamteindruck_4_clicked();
    void on_rbGesamteindruck_5_clicked();
    void on_rbGesamteindruck_6_clicked();
    void on_rbGesamteindruck_7_clicked();
    void on_rbGesamteindruck_ka_clicked();
    void on_tbGesamteindruck_textChanged();

private:
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);
    void setFarbe();
    void setSchaum();
    void setGeruch();
    void setGeschmack();
    void setAntrunk();
    void setHaupttrunk();
    void setNachtrunk();
    void setGesamteindruck();

private:
    Ui::TabBewertung *ui;
    int mIndex;
};

#endif // TABBEWERTUNG_H
