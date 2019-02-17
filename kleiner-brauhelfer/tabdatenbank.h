#ifndef TABDATENBANK_H
#define TABDATENBANK_H

#include <QWidget>

namespace Ui {
class TabDatenbank;
}

class TabDatenbank : public QWidget
{
    Q_OBJECT

public:
    explicit TabDatenbank(QWidget *parent = nullptr);
    ~TabDatenbank();
    void saveSettings();
    void restoreView();

private slots:
    void sudLoaded();
    void updateValues();
    void on_comboBox_currentIndexChanged(const QString &table);
    void on_comboBoxSud_currentIndexChanged(const QString &table);
    void tableView_selectionChanged();

private:
    Ui::TabDatenbank *ui;
    QByteArray mDefaultSplitterState;
};

#endif // TABDATENBANK_H
