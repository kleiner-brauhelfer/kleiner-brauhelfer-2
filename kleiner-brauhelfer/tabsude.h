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

private:
    Ui::TabSude *ui;
};

#endif // TABSUDE_H
