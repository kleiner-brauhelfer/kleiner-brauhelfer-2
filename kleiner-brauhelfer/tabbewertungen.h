#ifndef TABBEWERTUNGEN_H
#define TABBEWERTUNGEN_H

#include <QWidget>

namespace Ui {
class TabBewertungen;
}

class TabBewertungen : public QWidget
{
    Q_OBJECT

public:
    explicit TabBewertungen(QWidget *parent = nullptr);
    ~TabBewertungen();

private:
    Ui::TabBewertungen *ui;
};

#endif // TABBEWERTUNGEN_H
