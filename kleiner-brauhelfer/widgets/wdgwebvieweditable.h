#ifndef WDGWEBVIEWEDITABLE_H
#define WDGWEBVIEWEDITABLE_H

#include <QWidget>
#include <QTemporaryFile>
#include <QTimer>
#include <QPrinter>
#include "helper/htmlhighlighter.h"

namespace Ui {
class WdgWebViewEditable;
}

class WdgWebViewEditable : public QWidget
{
    Q_OBJECT

public:
    explicit WdgWebViewEditable(QWidget *parent = nullptr);
    ~WdgWebViewEditable();
    void clear();
    void setHtmlFile(const QString& file);
    void printPreview();
    void printToPdf(const QString& filePath, const QMarginsF& margins = QMarginsF(5, 10, 5, 15));

public slots:
    void updateWebView();
    void updateTags();

private slots:
    void printDocument(QPrinter *printer);
    void on_cbEditMode_clicked(bool checked);
    void on_cbTemplateAuswahl_currentIndexChanged(const QString &fileName);
    void on_btnSaveTemplate_clicked();
    void on_btnRestoreTemplate_clicked();
    void on_tbTemplate_textChanged();
    void on_sliderZoom_valueChanged(int value);
    void on_sliderZoom_sliderReleased();

private:
    bool checkSaveTemplate();

public:
    QVariantMap mTemplateTags;

private:
    Ui::WdgWebViewEditable *ui;
    HtmlHighLighter *mHtmlHightLighter;
    QTemporaryFile mTempCssFile;
    QTimer mTimerWebViewUpdate;

private:
    static double gZoomFactor;
};

#endif // WDGWEBVIEWEDITABLE_H
