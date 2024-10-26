#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include "tababstract.h"
#include "ui_tabsudauswahl.h"
#include <QAbstractItemModel>
#include <QAbstractItemDelegate>
#include <QToolBar>

namespace Ui {
class TabSudAuswahl;
}

class TabSudAuswahl : public TabAbstract
{
    Q_OBJECT
    MAKE_TRANSLATABLE_TAB

public:
    explicit TabSudAuswahl(QWidget *parent = nullptr);
    virtual ~TabSudAuswahl() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void restoreView() Q_DECL_OVERRIDE;
    void modulesChanged(Settings::Modules modules) Q_DECL_OVERRIDE;
    void setupActions(QToolBar* toolbar);

signals:
    void clicked(int sudId);

public slots:
    void sudAnlegen();
    void sudKopieren();
    void sudLoeschen();
    void sudLaden();
    void sudMerken(bool value);
    void sudTeilen();
    void rezeptImportieren();
    void rezeptExportieren();

private slots:
    void onDatabaseModified();
    void onFilterChanged();
    void onSelectionChanged();
    void updateWebView();
    void generateTemplateTags(QVariantMap& tags);
    void on_table_customContextMenuRequested(const QPoint &pos);
    void on_tbFilter_textChanged(const QString &pattern);
    void onVerbraucht_clicked(bool value);

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabSudAuswahl *ui;
    QByteArray mDefaultSplitterState;
};

#endif // TABSUDAUSWAHL_H
