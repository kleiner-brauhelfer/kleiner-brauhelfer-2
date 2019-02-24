#ifndef TABZUSAMMENFASSUNG_H
#define TABZUSAMMENFASSUNG_H

#include <QWidget>
#include <QTemporaryFile>
#include "helper/htmlhighlighter.h"

namespace Ui {
class TabZusammenfassung;
}

class TabZusammenfassung : public QWidget
{
    Q_OBJECT

public:
    explicit TabZusammenfassung(QWidget *parent = nullptr);
    ~TabZusammenfassung();
    void saveSettings();
    void restoreView();

private slots:
    void updateAll();
    void on_btnToPdf_clicked();
    void on_cbEditMode_clicked(bool checked);
    void on_cbTemplateAuswahl_currentIndexChanged(int);
    void on_tbTemplate_textChanged();
    void on_btnSaveTemplate_clicked();
    void on_btnRestoreTemplate_clicked();

private:
    void checkSaveTemplate();
    void updateTemplateTags();
    void updateWebView();

private:
    Ui::TabZusammenfassung *ui;
    HtmlHighLighter* mHtmlHightLighter;
    QVariantMap mTemplateTags;
    QTemporaryFile mTempCssFile;
};

#endif // TABZUSAMMENFASSUNG_H
