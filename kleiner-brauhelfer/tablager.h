#ifndef TABLAGER_H
#define TABLAGER_H

#include <QWidget>

namespace Ui {
class TabLager;
}

class TabLager : public QWidget
{
    Q_OBJECT

public:
    explicit TabLager(QWidget *parent = nullptr);
    ~TabLager();

private:
    Ui::TabLager *ui;
};

#endif // TABLAGER_H
