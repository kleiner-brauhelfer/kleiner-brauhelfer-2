#ifndef TABGAERVERLAUF_H
#define TABGAERVERLAUF_H

#include <QWidget>
#include <QItemSelection>
#include <QDebug>

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
    void setIspindelAvaiable(bool state);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private slots:
    void sudLoaded();
    void updateDiagramm();
    void updateValues();
    void updateWeitereZutaten();
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int id);
    void on_btnSWSchnellgaerverlauf_clicked();
    void on_btnAddSchnellgaerMessung_clicked();
    void on_btnDelSchnellgaerMessung_clicked();
    void on_btnSWHauptgaerverlauf_clicked();
    void on_btnAddHauptgaerMessung_clicked();
    void on_btnImportHauptgaerIspindel_clicked();
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
