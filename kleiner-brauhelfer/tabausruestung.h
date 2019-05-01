#ifndef TABAUSRUESTUNG_H
#define TABAUSRUESTUNG_H

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class TabAusruestung;
}

class TabAusruestung : public QWidget
{
    Q_OBJECT

public:
    explicit TabAusruestung(QWidget *parent = nullptr);
    ~TabAusruestung();
    void saveSettings();
    void restoreView();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void sudLoaded();
    void anlage_selectionChanged(const QItemSelection &selected);
    void on_btnNeueAnlage_clicked();
    void on_btnAnlageLoeschen_clicked();
    void on_btnNeuesGeraet_clicked();
    void on_btnGeraetLoeschen_clicked();
    void updateValues();
    void updateDurchschnitt();
    void on_btnAusbeuteMittel_clicked();
    void on_btnVerdampfungsziffer_clicked();
    void on_btnVerdampfungMittel_clicked();
    void on_tbAusbeute_valueChanged(double value);
    void on_sliderAusbeuteSude_valueChanged(int value);
    void on_tbVerdampfung_valueChanged(double value);
    void on_tbNachguss_valueChanged(double value);
    void on_tbFarbe_valueChanged(int value);
    void on_tbKosten_valueChanged(double value);
    void on_tbMaischebottichHoehe_valueChanged(double value);
    void on_tbMaischebottichDurchmesser_valueChanged(double value);
    void on_tbMaischebottichMaxFuellhoehe_valueChanged(double value);
    void on_tbSudpfanneHoehe_valueChanged(double value);
    void on_tbSudpfanneDurchmesser_valueChanged(double value);
    void on_tbSudpfanneMaxFuellhoehe_valueChanged(double value);
    void spalteAnzeigen(bool checked);
    void on_tableViewSude_customContextMenuRequested(const QPoint &pos);

private:
    QVariant data(const QString &fieldName) const;
    bool setData(const QString &fieldName, const QVariant &value);

private:
    Ui::TabAusruestung *ui;
    int mRow;
    QByteArray mDefaultTableState;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterLeftState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // TABAUSRUESTUNG_H
