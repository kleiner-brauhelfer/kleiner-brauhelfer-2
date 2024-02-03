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
    static void installTranslators();
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tabWidget_currentChanged(int tab);
    void on_actionShowTabBarLabels_toggled(bool showLabels);
    void focusChanged(QWidget *old, QWidget *now);
    void loadViewSettings();
    void saveViewSettings();
    void restoreView();
    void themeChanged(const QString &theme);
    void languageChanged(const QString &language);
    void databasePathChanged(const QString& path);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
