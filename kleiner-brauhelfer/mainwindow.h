#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings.h"

class DlgRohstoffe;
class DlgBrauUebersicht;
class DlgAusruestung;
class DlgPrintout;
class DlgEtikett;
class DlgBewertungen;
class DlgEinstellungen;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static QStringList HopfenTypname;
    static QStringList HefeTypname;
    static QStringList HefeTypFlTrName;
    static QStringList ZusatzTypname;
    static QStringList Einheiten;
    static QList<QPair<QString, int> > AnlageTypname;

    static MainWindow* getInstance();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    DlgRohstoffe* showDialogRohstoffe();
    DlgBrauUebersicht* showDialogBrauUebersicht();
    DlgAusruestung* showDialogAusruestung();
    DlgPrintout* showDialogPrintout();
    DlgEtikett* showDialogEtikett();
    DlgBewertungen* showDialogBewertungen();
    DlgEinstellungen* showDialogEinstellungen();

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void themeChanged(Qt::ColorScheme theme);
    void databaseModified();
    void updateTabs(Settings::Modules modules);
    void updateValues();
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void loadSud(int sudId);
    void checkMessageFinished();
    void modulesChanged(Settings::Modules modules);
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
    void on_actionHilfe_triggered();
    void on_actionUeber_triggered();
    void on_actionDatenbank_triggered();
    
private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void restart(int retCode = 1000);
    void saveDatabase();
    void discardDatabase();
    void saveSettings();
    void restoreView();
    void checkForUpdate(bool force);
    void initLabels();
    void initTheme(Qt::ColorScheme theme);
    void checkLoadedSud();

private:
    Ui::MainWindow *ui;
    QByteArray mDefaultState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // MAINWINDOW_H
