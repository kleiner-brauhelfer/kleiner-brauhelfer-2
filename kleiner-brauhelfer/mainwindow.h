#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settings.h"

class DlgRohstoffe;
class DlgBrauUebersicht;
class DlgAusruestung;
class DlgEinstellungen;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* getInstance();
    static void installTranslators();
    static QStringList HopfenTypname;
    static QStringList HefeTypname;
    static QStringList HefeTypFlTrName;
    static QStringList ZusatzTypname;
    static QStringList Einheiten;
    static QList<QPair<QString, int> > AnlageTypname;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QAction* getAction(const QString& name) const;

private slots:
    void focusChanged(QWidget *old, QWidget *now);
    void themeChanged(Qt::ColorScheme theme);
    void databaseModified();
    void updateTabs(Settings::Modules modules);
    void updateValues();
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void saveDatabase();
    void discardDatabase();
    void loadSud(int sudId);
    void checkMessageFinished();
    void modulesChanged(Settings::Modules modules);
    void restoreView();
    void checkForUpdate(bool force);
    void on_tabMain_currentChanged();
    void on_actionSudGebraut_triggered(bool checked);
    void on_actionSudAbgefuellt_triggered(bool checked);
    void on_actionSudAusgetrunken_triggered(bool checked);
    void on_actionEingabefelderEntsperren_triggered(bool checked);

private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void restart(int retCode = 1000);
    void saveSettings();
    void setupLabels();
    void setupActions();
    void checkLoadedSud();

private:
    Ui::MainWindow *ui;
    QByteArray mDefaultState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // MAINWINDOW_H
