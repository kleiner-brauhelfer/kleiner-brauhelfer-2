#ifndef TABSUDAUSWAHL_H
#define TABSUDAUSWAHL_H

#include <QWidget>
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
    void modulesChanged(Settings::Modules modules);

signals:
    void clicked(int sudId);
    void selectionChanged(bool sudSelected);

public slots:
    void restoreView();
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
    void on_table_customContextMenuRequested(const QPoint &pos);
    void on_tbFilter_textChanged(const QString &pattern);

private:
    void updateWebView();
    void generateTemplateTags(QVariantMap& tags);
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::TabSudAuswahl *ui;
};

#endif // TABSUDAUSWAHL_H
