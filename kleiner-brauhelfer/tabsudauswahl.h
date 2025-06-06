#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include "tababstract.h"
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>

namespace Ui {
class TabSudAuswahl;
}

class TabSudAuswahl : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabSudAuswahl(QWidget *parent = nullptr);
    virtual ~TabSudAuswahl() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView() Q_DECL_OVERRIDE;
    void modulesChanged(Settings::Modules modules) Q_DECL_OVERRIDE;
    bool isPrintable() const Q_DECL_OVERRIDE;
    void printPreview() Q_DECL_OVERRIDE;
    void toPdf() Q_DECL_OVERRIDE;
    void sudAnlegen();
    void sudKopieren(bool loadedSud = false);
    void sudTeilen(bool loadedSud = false);
    void sudLoeschen(bool loadedSud = false);
    void rezeptImportieren(const QString& filePath = QString());
    void rezeptExportieren(bool loadedSud = false);

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
    void onTabActivated() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultSplitterState;
};

#endif // TABSUDAUSWAHL_H
