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
    QAction* getAction(const QString& name) const;

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
    void saveDatabase();
    void discardDatabase();
    void modulesChanged(Settings::Modules modules);
    void eingabefelderEntsperren();
    
private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void restart(int retCode = 1000);
    void saveSettings();
    void restoreView();
    void checkForUpdate(bool force);
    void initLabels();
    void initTheme(Qt::ColorScheme theme);
    void initActions();
    void checkLoadedSud();

private:
    Ui::MainWindow *ui;
    QByteArray mDefaultState;
    QByteArray mDefaultSplitterHelpState;
};

#endif // MAINWINDOW_H
