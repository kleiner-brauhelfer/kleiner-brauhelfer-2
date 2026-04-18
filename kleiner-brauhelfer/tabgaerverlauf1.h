#ifndef TABGAERVERLAUF1_H
#define TABGAERVERLAUF1_H

#include <QWidget>
#include <QItemSelection>
#include "settings.h"

class SudObject;

namespace Ui {
class TabGaerverlauf1;
}

class TabGaerverlauf1 : public QWidget
{
    Q_OBJECT

public:
    explicit TabGaerverlauf1(QWidget *parent = nullptr);
    virtual ~TabGaerverlauf1() Q_DECL_OVERRIDE;
    void setup(SudObject *sud);
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);
    void checkEnabled();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    QDateTime toDateTime(QString string) const;
    double toDouble(QString string, bool *ok = nullptr) const;
    void pasteFromClipboardSchnellgaerverlauf(const QString& str);
    void pasteFromClipboardHauptgaerverlauf(const QString& str);

private slots:
    void sudLoaded();
    void sudDataChanged(const QModelIndex& index);
    void dataChangedSchnellgaerverlauf(const QModelIndex& index);
    void dataChangedHauptgaerverlauf(const QModelIndex& index);
    void updateDiagramm();
    void updateValues();
    void updateWeitereZutaten();
    void table_selectionChangedSchnellgaerverlauf(const QItemSelection &selected);
    void table_selectionChangedHauptgaerverlauf(const QItemSelection &selected);
    void diagram_selectionChangedSchnellgaerverlauf(int index);
    void diagram_selectionChangedHauptgaerverlauf(int index);
    void on_btnAddSchnellgaerMessung_clicked();
    void on_btnDelSchnellgaerMessung_clicked();
    void on_btnImportSchnellgaerMessung_clicked();
    void on_btnAddHauptgaerMessung_clicked();
    void on_btnDelHauptgaerMessung_clicked();
    void on_btnImportHauptgaerMessung_clicked();
    void on_btnGaerungEwzZugeben_clicked();
    void on_btnGaerungEwzEntnehmen_clicked();

private:
    Ui::TabGaerverlauf1 *ui;
    SudObject *mSud;
    QByteArray mDefaultSplitterState;
    QByteArray mDefaultSplitterStateSchnellgaerung;
    QByteArray mDefaultSplitterStateHauptgaerung;
};

#endif // TABGAERVERLAUF1_H
