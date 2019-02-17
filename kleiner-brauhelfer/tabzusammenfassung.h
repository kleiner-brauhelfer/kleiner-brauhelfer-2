#ifndef TABZUSAMMENFASSUNG_H
#define TABZUSAMMENFASSUNG_H

#include <QWidget>

namespace Ui {
class TabZusammenfassung;
}

class TabZusammenfassung : public QWidget
{
    Q_OBJECT

public:
    explicit TabZusammenfassung(QWidget *parent = nullptr);
    ~TabZusammenfassung();
    void saveSettings();
    void restoreView();

private:
    Ui::TabZusammenfassung *ui;
};

#endif // TABZUSAMMENFASSUNG_H
