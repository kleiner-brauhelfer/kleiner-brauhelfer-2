#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>
#include "settings.h"

namespace Ui {
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget *parent = nullptr);
    ~MainWindow2() Q_DECL_OVERRIDE;

private slots:
    void updateTabs(Settings::Modules modules);
    void updateValues();
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void loadSud(int sudId);
    void modulesChanged(Settings::Modules modules);
    void on_tabMain_currentChanged();
    void on_actionDruckvorschau_triggered();
    void on_actionDrucken_triggered();
    void on_actionSudGebraut_triggered();
    void on_actionSudAbgefuellt_triggered();
    void on_actionSudVerbraucht_triggered();
    void on_actionHefeZugabeZuruecksetzen_triggered();
    void on_actionWeitereZutaten_triggered();
    void on_actionEingabefelderEntsperren_changed();
    
private:
    void saveSettings();
    void restoreView();
    void checkLoadedSud();

private:
    Ui::MainWindow2 *ui;
};

#endif // MAINWINDOW2_H
