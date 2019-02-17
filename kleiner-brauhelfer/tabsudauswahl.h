#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include <QWidget>
#include <QAbstractItemModel>
#include <QTemporaryFile>
#include "helper/htmlhighlighter.h"

namespace Ui {
class TabSudAuswahl;
}

class TabSudAuswahl : public QWidget
{
    Q_OBJECT

public:
    explicit TabSudAuswahl(QWidget *parent = nullptr);
    ~TabSudAuswahl();
    void saveSettings();
    void restoreView();
    QAbstractItemModel* model() const;

signals:
    void clicked(int sudId);

private slots:
    void databaseModified();
    void filterChanged();
    void selectionChanged();
    void spalteAnzeigen(bool checked);
    void on_tableSudauswahl_doubleClicked(const QModelIndex &index);
    void on_tableSudauswahl_customContextMenuRequested(const QPoint &pos);
    void on_rbAlle_clicked();
    void on_rbNichtGebraut_clicked();
    void on_rbNichtAbgefuellt_clicked();
    void on_rbNichtVerbraucht_clicked();
    void on_rbAbgefuellt_clicked();
    void on_rbVerbraucht_clicked();
    void on_cbMerkliste_stateChanged(int state);
    void on_tbFilter_textChanged(const QString &pattern);
    void on_tbDatumVon_dateChanged(const QDate &date);
    void on_tbDatumBis_dateChanged(const QDate &date);
    void on_cbDatumAlle_stateChanged(int state);
    void on_btnMerken_clicked();
    void on_btnVergessen_clicked();
    void onVerbraucht_clicked();
    void onNichtVerbraucht_clicked();
    void on_btnAlleVergessen_clicked();
    void on_btnAnlegen_clicked();
    void on_btnKopieren_clicked();
    void on_btnLoeschen_clicked();
    void on_btnImportieren_clicked();
    void on_btnExportieren_clicked();
    void on_btnLaden_clicked();
    void on_btnToPdf_clicked();
    void on_cbEditMode_clicked(bool checked);
    void on_cbTemplateAuswahl_currentIndexChanged(int);
    void on_tbTemplate_textChanged();
    void on_btnSaveTemplate_clicked();
    void on_btnRestoreTemplate_clicked();

private:
    void checkSaveTemplate();
    void erstelleSudInfo();

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultTableState;
    QByteArray mDefaultSplitterState;
    HtmlHighLighter* mHtmlHightLighter;
    QTemporaryFile mTempCssFile;
};

#endif // TABSUDAUSWAHL_H
