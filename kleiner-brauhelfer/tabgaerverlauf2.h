#ifndef TABGAERVERLAUF2_H
#define TABGAERVERLAUF2_H

#include <QWidget>
#include <QItemSelection>
#include "settings.h"

class SudObject;

namespace Ui {
class TabGaerverlauf2;
}

class TabGaerverlauf2 : public QWidget
{
    Q_OBJECT

public:
    explicit TabGaerverlauf2(QWidget *parent = nullptr);
    virtual ~TabGaerverlauf2() Q_DECL_OVERRIDE;
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
    void pasteFromClipboardNachgaerverlauf(const QString& str);

private slots:
    void sudLoaded();
    void dataChangedNachgaerverlauf(const QModelIndex& index);
    void updateDiagramm();
    void updateValues();
    void table_selectionChanged(const QItemSelection &selected);
    void diagram_selectionChanged(int index);
    void on_btnAddNachgaerMessung_clicked();
    void on_btnDelNachgaerMessung_clicked();
    void on_btnImportNachgaerMessung_clicked();

private:
    Ui::TabGaerverlauf2 *ui;
    SudObject *mSud;
    QByteArray mDefaultSplitterStateNachgaerung;
};

#endif // TABGAERVERLAUF2_H
