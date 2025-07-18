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
    void sudAnlegen();
    void sudKopieren();
    void sudLoeschen();
    void sudImportieren();
    void sudExportieren();
    void sudTeilen();
    void sudMerken(bool value);
    void sudLaden();

private:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultSplitterState;
};

#endif // TABSUDAUSWAHL_H
