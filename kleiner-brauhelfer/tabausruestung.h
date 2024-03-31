#ifndef TABAUSRUESTUNG_H
#define TABAUSRUESTUNG_H

#include <QWidget>

namespace Ui {
class TabAusruestung;
}

class TabAusruestung : public QWidget
{
    Q_OBJECT

public:
    explicit TabAusruestung(QWidget *parent = nullptr);
    ~TabAusruestung();
    void restoreView();

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::TabAusruestung *ui;
};

#endif // TABAUSRUESTUNG_H
