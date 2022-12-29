#ifndef TABABFUELLEN_H
#define TABABFUELLEN_H

#include "tababstract.h"
#include <QTimer>

namespace Ui {
class TabAbfuellen;
}

class TabAbfuellen : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabAbfuellen(QWidget *parent = nullptr);
    virtual ~TabAbfuellen() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView() Q_DECL_OVERRIDE;
    void modulesChanged(Settings::Modules modules) Q_DECL_OVERRIDE;
    void checkEnabled();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
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
    void on_btnSudVerbraucht_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;

private:
    Ui::TabAbfuellen *ui;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterChartsState;
};

#endif // TABABFUELLEN_H
