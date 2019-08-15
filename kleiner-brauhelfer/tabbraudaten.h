#ifndef TABBRAUDATEN_H
#define TABBRAUDATEN_H

#include <QWidget>

namespace Ui {
class TabBraudaten;
}

class TabBraudaten : public QWidget
{
    Q_OBJECT

public:
    explicit TabBraudaten(QWidget *parent = nullptr);
    ~TabBraudaten();
    void saveSettings();
    void restoreView();
    void checkEnabled();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void sudLoaded();
    void updateValues();
    void sudDataChanged(const QModelIndex& index);

    void on_tbBraudatum_dateTimeChanged(const QDateTime &dateTime);
    void on_btnBraudatumHeute_clicked();
    void on_tbWuerzemengeKochbeginn_valueChanged(double value);
    void on_btnWuerzemengeKochbeginn_clicked();
    void on_tbTempKochbeginn_valueChanged(double);
    void on_tbWuerzemengeKochende_valueChanged(double value);
    void on_btnWuerzemengeKochende_clicked();
    void on_tbTempKochende_valueChanged(double);
    void on_tbSWKochende_valueChanged(double value);
    void on_btnSWKochende_clicked();
    void on_tbSWAnstellen_valueChanged(double value);
    void on_btnSWAnstellen_clicked();
    void on_btnWasserVerschneidung_clicked();
    void on_tbWuerzemengeAnstellenTotal_valueChanged(double value);
    void on_btnWuerzemengeAnstellenTotal_clicked();
    void on_tbSpeiseSRE_valueChanged(double);
    void on_btnSpeisemengeNoetig_clicked();
    void on_tbSpeisemenge_valueChanged(double value);
    void on_tbWuerzemengeAnstellen_valueChanged(double value);

    void on_tbNebenkosten_valueChanged(double value);
    void on_cbDurchschnittIgnorieren_clicked(bool checked);
    void on_btnSudGebraut_clicked();
    void on_btnSudTeilen_clicked(); 

private:
    Ui::TabBraudaten *ui;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // TABBRAUDATEN_H
