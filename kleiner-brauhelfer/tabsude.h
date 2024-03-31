#ifndef TABSUDE_H
#define TABSUDE_H

#include <QWidget>

namespace Ui {
class TabSude;
}

class TabSude : public QWidget
{
    Q_OBJECT

public:
    explicit TabSude(QWidget *parent = nullptr);
    ~TabSude();
    void restoreView();

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::TabSude *ui;
};

#endif // TABSUDE_H
