#ifndef TABAUSRUESTUNG_H
#define TABAUSRUESTUNG_H

#include <QWidget>
#include "settings.h"

namespace Ui {
class TabAusruestung;
}

class TabAusruestung : public QWidget
{
    Q_OBJECT

public:
    explicit TabAusruestung(QWidget *parent = nullptr);
    ~TabAusruestung();
    void restoreView();

public slots:
    void add();
    void remove();

private:
    void loadSettings();
    void saveSettings();

private slots:
    void modulesChanged(Settings::Modules modules);
    void sudLoaded();
    void anlage_selectionChanged();
    void on_btnNeuesGeraet_clicked();
    void on_btnGeraetLoeschen_clicked();
    void updateValues();
    void updateDurchschnitt();
    void on_tbName_editingFinished();
    void on_cbTyp_activated(int index);
    void on_btnAusbeuteMittel_clicked();
    void on_btnVerdampfungsrate_clicked();
    void on_btnVerdampfungMittel_clicked();
    void on_tbAusbeute_editingFinished();
    void on_sliderAusbeuteSude_valueChanged(int value);
    void on_tbVerdampfung_editingFinished();
    void on_tbKorrekturNachguss_editingFinished();
    void on_tbKorrekturFarbe_editingFinished();
    void on_tbKorrekturSollmenge_editingFinished();
    void on_tbKosten_editingFinished();
    void on_tbMaischebottichHoehe_editingFinished();
    void on_tbMaischebottichDurchmesser_editingFinished();
    void on_tbMaischebottichMaxFuellhoehe_editingFinished();
    void on_tbSudpfanneHoehe_editingFinished();
    void on_tbSudpfanneDurchmesser_editingFinished();
    void on_tbSudpfanneMaxFuellhoehe_editingFinished();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);

private:
    Ui::TabAusruestung *ui;
    int mRow;
};

#endif // TABAUSRUESTUNG_H
