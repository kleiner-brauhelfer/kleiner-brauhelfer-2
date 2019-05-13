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

private slots:
    void updateAll();
    void on_btnToPdf_clicked();

private:
    Ui::TabZusammenfassung *ui;
};

#endif // TABZUSAMMENFASSUNG_H
