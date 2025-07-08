#ifndef TABGAERVERLAUF_H
#define TABGAERVERLAUF_H

#include <QWidget>
#include <QItemSelection>
#include "settings.h"

class SudObject;

namespace Ui {
class TabGaerverlauf;
}

class TabGaerverlauf : public QWidget
{
    Q_OBJECT

public:
    explicit TabGaerverlauf(QWidget *parent = nullptr);
    virtual ~TabGaerverlauf() Q_DECL_OVERRIDE;
    void setup(SudObject *sud);
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);
    void checkEnabled();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    QDateTime toDateTime(QString string) const;
    double toDouble(QString string, bool *ok = nullptr) const;
    void pasteFromClipboardSchnellgaerverlauf(const QString& str);
    void pasteFromClipboardHauptgaerverlauf(const QString& str);
    void pasteFromClipboardNachgaerverlauf(const QString& str);

private slots:
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void dataChangedSchnellgaerverlauf(const QModelIndex& index);
    void dataChangedHauptgaerverlauf(const QModelIndex& index);
    void dataChangedNachgaerverlauf(const QModelIndex& index);
    void updateDiagramm();
    void updateValues();
    void updateWeitereZutaten();
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int index);
    void on_btnAddSchnellgaerMessung_clicked();
    void on_btnDelSchnellgaerMessung_clicked();
    void on_btnImportSchnellgaerMessung_clicked();
    void on_btnAddHauptgaerMessung_clicked();
    void on_btnDelHauptgaerMessung_clicked();
    void on_btnImportHauptgaerMessung_clicked();
    void on_btnGaerungEwzZugeben_clicked();
    void on_btnGaerungEwzEntnehmen_clicked();
    void on_btnAddNachgaerMessung_clicked();
    void on_btnDelNachgaerMessung_clicked();
    void on_btnImportNachgaerMessung_clicked();

private:
    Ui::TabGaerverlauf *ui;
    SudObject *mSud;
    QByteArray mDefaultSplitterStateSchnellgaerung;
    QByteArray mDefaultSplitterStateHauptgaerung;
    QByteArray mDefaultSplitterStateNachgaerung;
};

#endif // TABGAERVERLAUF_H
