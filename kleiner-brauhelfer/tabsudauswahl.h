#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>
#include "settings.h"

namespace Ui {
class TabSudAuswahl;
}

class TabSudAuswahl : public QWidget
{
    Q_OBJECT

public:
    explicit TabSudAuswahl(QWidget *parent = nullptr);
    virtual ~TabSudAuswahl() Q_DECL_OVERRIDE;
    void saveSettings();
    void restoreView();
    void modulesChanged(Settings::Modules modules);

signals:
    void clicked(int sudId);

private slots:
    void databaseModified();
    void filterChanged();
    void selectionChanged();
    void updateWebView();
    void generateTemplateTags(QVariantMap& tags);
    void on_table_doubleClicked(const QModelIndex &index);
    void on_table_customContextMenuRequested(const QPoint &pos);
    void on_tbFilter_textChanged(const QString &pattern);
    void on_btnMerken_clicked();
    void on_btnVergessen_clicked();
    void onMerkliste_clicked(bool value);
    void onVerbraucht_clicked(bool value);
    void on_btnAnlegen_clicked();
    void on_btnKopieren_clicked();
    void on_btnLoeschen_clicked();
    void on_btnImportieren_clicked();
    void on_btnExportieren_clicked();
    void on_btnTeilen_clicked();
    void on_btnLaden_clicked();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void sudAnlegen();
    void sudKopieren();
    void sudTeilen();
    void sudLoeschen();
    void rezeptImportieren();
    void rezeptExportieren();

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultSplitterState;
};

#endif // TABSUDAUSWAHL_H
