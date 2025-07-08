#ifndef TABBRAUDATEN_H
#define TABBRAUDATEN_H

#include <QWidget>
#include "settings.h"

class SudObject;

namespace Ui {
class TabBraudaten;
}

class TabBraudaten : public QWidget
{
    Q_OBJECT

public:
    explicit TabBraudaten(QWidget *parent = nullptr);
    virtual ~TabBraudaten() Q_DECL_OVERRIDE;
    void setup(SudObject *sud);
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);
    void checkEnabled();

private slots:
    void sudLoaded();
    void updateValues();
    void sudDataChanged(const QModelIndex& index);

    void on_tbBraudatum_dateChanged(const QDate &date);
    void on_tbBraudatumZeit_timeChanged(const QTime &time);
    void on_btnBraudatumHeute_clicked();
    void on_btnSWKochbeginn_clicked();
    void on_btnWuerzemengeKochbeginn_clicked();
    void on_tbTempKochbeginn_valueChanged(double);
    void on_btnWuerzemengeVorHopfenseihen_clicked();
    void on_btnWuerzemengeKochende_clicked();
    void on_tbTempKochende_valueChanged(double);
    void on_btnSWKochende_clicked();
    void on_btnSWAnstellen_clicked();
    void on_btnWuerzemengeAnstellenTotal_clicked();
    void on_btnWuerzemengeAnstellen_clicked();
    void on_tbSpeiseSRE_valueChanged(double);
    void on_tbSpeiseT_valueChanged(double);
    void on_tbWasserVerschneidungSW_valueChanged(double);
    void on_tbSWHefestarterBrix_valueChanged(double);
    void on_tbWasserVerschneidungBrix_valueChanged(double);
    void on_btnVerduennung_clicked();
    void on_btnSpeisemengeNoetig_clicked();
    void on_cbDurchschnittIgnorieren_clicked(bool checked);
    void on_btnSudGebraut_clicked();
    void on_btnSudGebrautReset_clicked();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabBraudaten *ui;
    SudObject *mSud;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterChartsState;
};

#endif // TABBRAUDATEN_H
