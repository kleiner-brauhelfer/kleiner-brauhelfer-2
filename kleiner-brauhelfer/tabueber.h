#ifndef TABUEBER_H
#define TABUEBER_H

#include <QWidget>

namespace Ui {
class TabUeber;
}

class TabUeber : public QWidget
{
    Q_OBJECT

public:
    explicit TabUeber(QWidget *parent = nullptr);
    ~TabUeber();

private:
    Ui::TabUeber *ui;
};

#endif // TABUEBER_H
