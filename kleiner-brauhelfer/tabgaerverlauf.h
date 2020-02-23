#ifndef TABGAERVERLAUF_H
#define TABGAERVERLAUF_H

#include "tababstract.h"
#include <QItemSelection>

namespace Ui {
class TabGaerverlauf;
}

class TabGaerverlauf : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabGaerverlauf(QWidget *parent = nullptr);
    virtual ~TabGaerverlauf() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView() Q_DECL_OVERRIDE;
    void checkEnabled();

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    QDateTime fromString(const QString& string);
    void pasteFromClipboardSchnellgaerverlauf();
    void pasteFromClipboardHauptgaerverlauf();
    void pasteFromClipboardNachgaerverlauf();

private slots:
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void onSchnellgaerverlaufRowInserted();
    void onHauptgaerverlaufRowInserted();
    void onNachgaerverlaufRowInserted();
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
