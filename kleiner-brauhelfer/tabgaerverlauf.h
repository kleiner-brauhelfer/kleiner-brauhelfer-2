#ifndef TABGAERVERLAUF_H
#define TABGAERVERLAUF_H

#include <QWidget>

namespace Ui {
class TabGaerverlauf;
}

class TabGaerverlauf : public QWidget
{
    Q_OBJECT

public:
    explicit TabGaerverlauf(QWidget *parent = nullptr);
    ~TabGaerverlauf();
    void saveSettings();
    void restoreView();

private slots:
    void sudLoaded();
    void updateDiagramm();
    void updateEnabled();
    void updateWeitereZutaten();
    void on_btnSWSchnellgaerverlauf_clicked();
    void on_btnAddSchnellgaerMessung_clicked();
    void on_btnDelSchnellgaerMessung_clicked();
    void on_btnSWHauptgaerverlauf_clicked();
    void on_btnAddHauptgaerMessung_clicked();
    void on_btnDelHauptgaerMessung_clicked();
    void on_btnGaerungEwzZugeben_clicked();
    void on_btnGaerungEwzEntnehmen_clicked();
    void on_btnAddNachgaerMessung_clicked();
    void on_btnDelNachgaerMessung_clicked();

private:
    Ui::TabGaerverlauf *ui;
    QByteArray mDefaultSplitterStateSchnellgaerung;
    QByteArray mDefaultSplitterStateHauptgaerung;
    QByteArray mDefaultSplitterStateNachgaerung;
};

#endif // TABGAERVERLAUF_H
