#ifndef TABSUD_H
#define TABSUD_H

#include <QWidget>

namespace Ui {
class TabSud;
}

class TabSud : public QWidget
{
    Q_OBJECT

public:
    explicit TabSud(QWidget *parent = nullptr);
    ~TabSud();

private:
    Ui::TabSud *ui;
};

#endif // TABSUD_H
