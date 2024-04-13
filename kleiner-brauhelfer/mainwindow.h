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
    enum Tab {
        Sudauswahl,
        Sude,
        Lager,
        Anlagen,
        Brauubersicht,
        Bewertungen,
        Einstellungen,
        Ueber
    };

public:
    static MainWindow* getInstance();
    static QStringList HopfenTypname;
    static QStringList HefeTypname;
    static QStringList HefeTypFlTrName;
    static QStringList ZusatzTypname;
    static QStringList Einheiten;
    static QList<QPair<QString, int> > AnlageTypname;

public:
    static void installTranslators();
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tabWidget_currentChanged(int tab);
    void on_actionShowTabBarLabels_toggled(bool showLabels);
    void focusChanged(QWidget *old, QWidget *now);
    void saveSettings();
    void restoreView();
    void themeChanged(const QString &theme);
    void languageChanged(const QString &language);
    void databasePathChanged(const QString& path);
    void checkUpdate(bool force);
    void checkUpdateFinished();
    void databaseModified();
    void saveDatabase();
    void discardDatabase();
    void showDatabase();
    void showDatabaseClean();
    void showHelp();
    void tabSudAuswahlSelectionChanged(bool sudSelected);

private:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void setupActions();
    void setupLabels();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
