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
    void on_actionOeffnen_triggered();
    void on_actionSpeichern_triggered();
    void on_actionVerwerfen_triggered();
    void on_actionBereinigen_triggered();
    void on_actionBeenden_triggered();
    void on_actionSudGebraut_triggered();
    void on_actionSudAbgefuellt_triggered();
    void on_actionSudVerbraucht_triggered();
    void on_actionHefeZugabeZuruecksetzen_triggered();
    void on_actionWeitereZutaten_triggered();
    void on_actionEingabefelderEntsperren_changed();
    void on_actionWiederherstellen_triggered();
    void on_actionThemeSystem_triggered();
    void on_actionThemeHell_triggered();
    void on_actionThemeDunkel_triggered();
    void on_actionSchriftart_triggered(bool checked);
    void on_actionReiterDatenbank_triggered(bool checked);
	void on_actionCheckUpdate_triggered(bool checked);
    void on_actionBestaetigungBeenden_triggered(bool checked);
    void on_actionTooltips_triggered(bool checked);
    void on_actionSpende_triggered();
    void on_actionUeber_triggered();

private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void restart();
    void save();
    void saveSettings();
    void restoreView(bool onUpdate = false);
    void checkForUpdate(bool force);

private:
    Ui::MainWindow *ui;
    QByteArray mDefaultState;
    int mTabIndexDatenbank;
};

#endif // MAINWINDOW_H
