#ifndef WDGWEBVIEWEDITABLE_H
#define WDGWEBVIEWEDITABLE_H

#include <QWidget>
#include <QTemporaryFile>
#include <QTimer>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinter>
#endif
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
    void setPrintable(bool isPrintable);
    void printPreview();
    void printToPdf();
    void setPdfName(const QString& name);

public slots:
    void updateWebView();
    void updateTags();
    void updateEditMode();

private slots:
  #ifdef QT_PRINTSUPPORT_LIB
    void onPrinterPaintRequested(QPrinter *printer);
  #endif
    void on_cbEditMode_clicked(bool checked);
    void on_cbTemplateAuswahl_currentTextChanged(const QString &fileName);
    void on_btnSaveTemplate_clicked();
    void on_btnRestoreTemplate_clicked();
    void on_tbTemplate_textChanged();
    void on_sliderZoom_valueChanged(int value);
    void on_sliderZoom_sliderReleased();
    void on_btnPrint_clicked();
    void on_btnPdf_clicked();

private:
    bool checkSaveTemplate();

public:
    QVariantMap mTemplateTags;

private:
    Ui::WdgWebViewEditable *ui;
    HtmlHighLighter *mHtmlHightLighter;
    QTemporaryFile mTempCssFile;
    QTimer mTimerWebViewUpdate;
    QString htmlName;
    QString pdfName;

private:
    static double gZoomFactor;
};

#endif // WDGWEBVIEWEDITABLE_H
