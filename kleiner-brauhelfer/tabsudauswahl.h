#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include <QWidget>
#include <QAbstractItemModel>

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
    void updateTemplateTags();
    void generateTemplateTags(QVariantMap& tags);
    void on_tableSudauswahl_doubleClicked(const QModelIndex &index);
    void on_tableSudauswahl_customContextMenuRequested(const QPoint &pos);
    void on_cbAlle_clicked(bool checked);
    void on_cbRezept_clicked();
    void on_cbGebraut_clicked();
    void on_cbAbgefuellt_clicked();
    void on_cbVerbraucht_clicked();
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

private:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void setFilterStatus();

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultTableState;
    QByteArray mDefaultSplitterState;
};

#endif // TABSUDAUSWAHL_H
