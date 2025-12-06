#ifndef TABABFUELLEN_H
#define TABABFUELLEN_H

#include <QWidget>
#include "settings.h"

class SudObject;

namespace Ui {
class TabAbfuellen;
}

class TabAbfuellen : public QWidget
{
    Q_OBJECT

public:
    explicit TabAbfuellen(QWidget *parent = nullptr);
    virtual ~TabAbfuellen() Q_DECL_OVERRIDE;
    void setup(SudObject *sud);
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);
    void checkEnabled();

private slots:
    void sudLoaded();
    void updateValues();
    void sudDataChanged(const QModelIndex& index);

    void on_tbAbfuelldatum_dateChanged(const QDate &date);
    void on_tbAbfuelldatumZeit_timeChanged(const QTime &time);
    void on_btnAbfuelldatumHeute_clicked();
    void on_tbReifung_dateChanged(const QDate &date);
    void on_btnReifungHeute_clicked();
    void on_cbSchnellgaerprobeAktiv_clicked(bool checked);
    void on_btnSWSchnellgaerprobe_clicked();
    void on_btnSWJungbier_clicked();
    void on_cbSpunden_clicked(bool checked);
    void on_tbZuckerFaktor_valueChanged(double);
    void on_tbFlaschengroesse_valueChanged(double);
    void on_btnSudAbgefuellt_clicked();
    void on_btnSudAbgefuelltReset_clicked();
    void on_btnSudVerbraucht_clicked();
    void on_btnSudVerbrauchtReset_clicked();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabAbfuellen *ui;
    SudObject *mSud;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterChartsState;
};

#endif // TABABFUELLEN_H
