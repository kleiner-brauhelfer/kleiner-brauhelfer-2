#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

private slots:
    void databaseModified();
    void updateValues();
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void loadSud(int sudId);
    void changeStyle();
    void checkMessageFinished();
    void on_tabMain_currentChanged();
    void on_actionNeuen_Sud_anlegen_triggered();
    void on_actionSud_kopieren_triggered();
    void on_actionSud_teilen_triggered();
    void on_actionSud_l_schen_triggered();
    void on_actionRezept_importieren_triggered();
    void on_actionRezept_exportieren_triggered();
    void on_actionSpeichern_triggered();
    void on_actionVerwerfen_triggered();
    void on_actionDruckvorschau_triggered();
    void on_actionDrucken_triggered();
    void on_actionBereinigen_triggered();
    void on_actionBeenden_triggered();
    void on_actionSudGebraut_triggered();
    void on_actionSudAbgefuellt_triggered();
    void on_actionSudVerbraucht_triggered();
    void on_actionHefeZugabeZuruecksetzen_triggered();
    void on_actionWeitereZutaten_triggered();
    void on_actionEingabefelderEntsperren_changed();
    void on_actionWiederherstellen_triggered();
    void on_actionThemeHell_triggered();
    void on_actionThemeDunkel_triggered();
    void on_actionSchriftart_triggered(bool checked);
    void on_actionOeffnen_triggered();
    void on_actionReiterGaerverlauf_triggered(bool checked);
    void on_actionReiterZusammenfassung_triggered(bool checked);
    void on_actionReiterEtikette_triggered(bool checked);
    void on_actionReiterBewertung_triggered(bool checked);
    void on_actionReiterBrauuebersicht_triggered(bool checked);
    void on_actionReiterAusruestung_triggered(bool checked);
    void on_actionReiterDatenbank_triggered(bool checked);
    void on_actionLog_triggered();
	void on_actionCheckUpdate_triggered(bool checked);
    void on_actionBestaetigungBeenden_triggered(bool checked);
    void on_actionTooltips_triggered(bool checked);
    void on_actionAnimationen_triggered(bool checked);
    void on_actionDeutsch_triggered();
    void on_actionEnglisch_triggered();
    void on_actionSchwedisch_triggered();
    void on_actionNiederlaendisch_triggered();
    void on_actionSpende_triggered();
    void on_actionBierspende_triggered();
    void on_actionFormelsammlung_triggered();
    void on_actionUeber_triggered();

private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void restart(int retCode = 1000);
    void save();
    void saveSettings();
    void restoreView(bool full);
    void checkForUpdate(bool force);

private:
    Ui::MainWindow *ui;
    QByteArray mDefaultState;
    int mTabIndexGaerverlauf;
    int mTabIndexZusammenfassung;
    int mTabIndexEtikette;
    int mTabIndexBewertung;
    int mTabIndexBrauuebersicht;
    int mTabIndexAusruestung;
    int mTabIndexDatenbank;
};

#endif // MAINWINDOW_H
