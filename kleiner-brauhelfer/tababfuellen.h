#ifndef TABABFUELLEN_H
#define TABABFUELLEN_H

#include <QWidget>

namespace Ui {
class TabAbfuellen;
}

class TabAbfuellen : public QWidget
{
    Q_OBJECT

public:
    explicit TabAbfuellen(QWidget *parent = nullptr);
    ~TabAbfuellen();
    void saveSettings();
    void restoreView();
    void checkEnabled();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void sudLoaded();
    void updateValues();
    void sudDataChanged(const QModelIndex& index);

    void on_tbAbfuelldatum_dateTimeChanged(const QDateTime &dateTime);
    void on_btnAbfuelldatumHeute_clicked();
    void on_cbSchnellgaerprobeAktiv_clicked(bool checked);
    void on_tbSWSchnellgaerprobe_valueChanged(double value);
    void on_btnSWSchnellgaerprobe_clicked();
    void on_tbSWJungbier_valueChanged(double value);
    void on_btnSWJungbier_clicked();
    void on_tbTemperaturJungbier_valueChanged(double value);
    void on_cbSpunden_clicked(bool checked);
    void on_tbJungbiermengeAbfuellen_valueChanged(double value);
    void on_tbBiermengeAbfuellen_valueChanged(double value);
    void on_tbSpeisemengeAbgefuellt_valueChanged(double value);
    void on_tbZuckerFaktor_valueChanged(double);
    void on_tbFlasche_valueChanged(double);

    void on_tbNebenkosten_valueChanged(double value);
    void on_btnSudAbgefuellt_clicked();
    void on_btnSudTeilen_clicked(); 

    void on_btnSudVerbraucht_clicked();

private:
    Ui::TabAbfuellen *ui;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // TABABFUELLEN_H
