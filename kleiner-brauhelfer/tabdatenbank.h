#ifndef TABDATENBANK_H
#define TABDATENBANK_H

#include "tababstract.h"

namespace Ui {
class TabDatenbank;
}

class TabDatenbank : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabDatenbank(QWidget *parent = nullptr);
    virtual ~TabDatenbank() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView(bool full) Q_DECL_OVERRIDE;

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
