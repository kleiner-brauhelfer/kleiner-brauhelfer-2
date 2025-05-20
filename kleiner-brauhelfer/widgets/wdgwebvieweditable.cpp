#include "wdgwebvieweditable.h"
#include "ui_wdgwebvieweditable.h"
#include <QDir>
#include <QMessageBox>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#endif
#include <QJsonDocument>
#include <QFileDialog>
#include <QDesktopServices>
#include "settings.h"

extern Settings* gSettings;

double WdgWebViewEditable::gZoomFactor = 1.0;

WdgWebViewEditable::WdgWebViewEditable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWebViewEditable),
    mTempCssFile(QDir::tempPath() + "/" + QCoreApplication::applicationName() + QStringLiteral(".XXXXXX.css"))
{
    ui->setupUi(this);
    ui->webview->setLinksExternal(true);
    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTemplate->setTabStopDistance(QFontMetrics(ui->tbTemplate->font()).horizontalAdvance(QStringLiteral("  ")));
   #else
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
   #endif
  #endif
    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTags->setTabStopDistance(QFontMetrics(ui->tbTags->font()).horizontalAdvance(QStringLiteral("  ")));
   #else
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTags->font()).width(' '));
   #endif
  #endif
    ui->btnSaveTemplate->setError(true);
    mTimerWebViewUpdate.setSingleShot(true);
    connect(&mTimerWebViewUpdate, &QTimer::timeout, this, &WdgWebViewEditable::updateWebView, Qt::QueuedConnection);
    updateEditMode();
    ui->splitterEditmode->setHandleWidth(0);
    ui->splitterEditmode->setSizes({1, 0});
    gZoomFactor = gSettings->valueInGroup("General", "WebViewZoomFactor", 1.0).toDouble();
}

WdgWebViewEditable::~WdgWebViewEditable()
{
    delete ui;
    gSettings->setValueInGroup("General", "WebViewZoomFactor", gZoomFactor);
}

void WdgWebViewEditable::clear()
{
    ui->webview->setHtml(QStringLiteral(""));
}

void WdgWebViewEditable::setHtmlFile(const QString& file)
{
    QString fileComplete;
    QString lang = gSettings->language();
    if (lang == QStringLiteral("de"))
    {
        fileComplete = file + ".html";
    }
    else
    {
        fileComplete = file + "_" + lang + ".html";
        if (!QFile::exists(gSettings->dataDir(1) + fileComplete))
            fileComplete = file + ".html";
    }
    htmlName = file;
    ui->cbTemplateAuswahl->setItemText(0, fileComplete);
    ui->webview->setTemplateFile(gSettings->dataDir(1) + fileComplete);
}

void WdgWebViewEditable::setPrintable(bool isPrintable)
{
    ui->btnPdf->setVisible(isPrintable);
    ui->btnPrint->setVisible(isPrintable);
}

#ifdef QT_PRINTSUPPORT_LIB
void WdgWebViewEditable::onPrinterPaintRequested(QPrinter *printer)
{
#if defined(QT_WEBENGINECORE_LIB) && (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    QWidget* parent = qobject_cast<QWidget*>(sender());
    if(parent)
        parent->setEnabled(false);
    QEventLoop loop;
  #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    ui->webview->print(printer);
    connect(ui->webview, &QWebEngineView::printFinished, &loop, &QEventLoop::quit);
  #else
    ui->webview->page()->print(printer, [&](bool) { loop.quit(); });
  #endif
    loop.exec();
    if(parent)
        parent->setEnabled(true);
#else
  Q_UNUSED(printer)
#endif
}
#endif

void WdgWebViewEditable::printPreview()
{
  #if defined(QT_PRINTSUPPORT_LIB) && defined(QT_WEBENGINECORE_LIB)
    QVariant style;
    if (gSettings->theme() == Qt::ColorScheme::Dark)
    {
        style = mTemplateTags[QStringLiteral("Style")];
        mTemplateTags[QStringLiteral("Style")] = "style_hell.css";
        ui->webview->setUpdatesEnabled(false);

        QEventLoop loop;
        connect(ui->webview, &QWebEngineView::loadFinished, &loop, &QEventLoop::quit);
        ui->webview->renderTemplate(mTemplateTags);
        loop.exec();
    }

    QPrinter* printer = gSettings->createPrinter();
    QPrintPreviewDialog dlg(printer, ui->webview);
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &WdgWebViewEditable::onPrinterPaintRequested);
    dlg.exec();
    gSettings->savePrinter(printer);
    delete printer;

    if (gSettings->theme() == Qt::ColorScheme::Dark)
    {
        mTemplateTags[QStringLiteral("Style")] = style;
        ui->webview->renderTemplate(mTemplateTags);
        ui->webview->setUpdatesEnabled(true);
    }
  #endif
}

void WdgWebViewEditable::setPdfName(const QString& name)
{
    pdfName = name;
}

void WdgWebViewEditable::printToPdf()
{
  #if defined(QT_PRINTSUPPORT_LIB) && defined(QT_WEBENGINECORE_LIB)
    gSettings->beginGroup("General");
    QString fileName = pdfName.isEmpty() ? htmlName : pdfName;
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                                    path + "/" + fileName + ".pdf",QStringLiteral("PDF (*.pdf)"));
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        QRectF rect = gSettings->value("PrintMargins", QRectF(5, 10, 5, 15)).toRectF();
        QMarginsF margins = QMarginsF(rect.left(), rect.top(), rect.width(), rect.height());
        if (gSettings->theme() == Qt::ColorScheme::Dark)
        {
            QVariant style = mTemplateTags[QStringLiteral("Style")];
            mTemplateTags[QStringLiteral("Style")] = "style_hell.css";
            ui->webview->setUpdatesEnabled(false);

            QEventLoop loop;
            connect(ui->webview, &QWebEngineView::loadFinished, &loop, &QEventLoop::quit);
            ui->webview->renderTemplate(mTemplateTags);
            loop.exec();

            ui->webview->printToPdf(filePath, margins);

            mTemplateTags[QStringLiteral("Style")] = style;
            ui->webview->renderTemplate(mTemplateTags);
            ui->webview->setUpdatesEnabled(true);
        }
        else
        {
            ui->webview->printToPdf(filePath, margins);
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    gSettings->endGroup();
  #endif
}

bool WdgWebViewEditable::checkSaveTemplate()
{
    if (ui->btnSaveTemplate->isVisible())
    {
        int ret = QMessageBox::question(this, tr("Änderungen speichern?"),
                                        tr("Sollen die Änderungen gespeichert werden?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
            on_btnSaveTemplate_clicked();
        else if (ret == QMessageBox::Cancel)
            return true;
    }
    return false;
}

void WdgWebViewEditable::on_cbEditMode_clicked(bool checked)
{
    updateEditMode();
    ui->splitterEditmode->setHandleWidth(checked ? 5 : 0);
    ui->splitterEditmode->setSizes({1, checked ? 1 : 0});
}

void WdgWebViewEditable::on_cbTemplateAuswahl_currentTextChanged(const QString &fileName)
{
    Q_UNUSED(fileName)
    updateEditMode();
}

void WdgWebViewEditable::on_tbTemplate_textChanged()
{
    if (ui->tbTemplate->hasFocus())
    {
        mTimerWebViewUpdate.start(200);
        ui->btnSaveTemplate->setVisible(true);
    }
}

void WdgWebViewEditable::on_btnSaveTemplate_clicked()
{
    QFile file(ui->btnSaveTemplate->property("file").toString());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(ui->tbTemplate->toPlainText().toUtf8());
    file.close();
    ui->btnSaveTemplate->setVisible(false);
}

void WdgWebViewEditable::on_btnRestoreTemplate_clicked()
{
    int ret = QMessageBox::question(this, tr("Template wiederherstellen?"),
                                    tr("Soll das Standardtemplate wiederhergestellt werden?"));
    if (ret == QMessageBox::Yes)
    {
        QFile file(gSettings->dataDir(1) + ui->cbTemplateAuswahl->currentText());
        QFile file2(":/data/Webview/" + ui->cbTemplateAuswahl->currentText());
        file.remove();
        if (file2.copy(file.fileName()))
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        ui->btnSaveTemplate->setVisible(false);
        updateEditMode();
    }
}

void WdgWebViewEditable::updateWebView()
{
  #ifdef QT_WEBENGINECORE_LIB
    if (ui->webview->zoomFactor() != gZoomFactor)
    {
        ui->webview->setZoomFactor(gZoomFactor);
        ui->sliderZoom->setValue(gZoomFactor * 100);
        ui->lblZoom->clear();
    }
  #endif
    if (ui->cbEditMode->isChecked())
    {
        if (ui->cbTemplateAuswahl->currentIndex() == 0)
        {
            ui->webview->renderText(ui->tbTemplate->toPlainText(), mTemplateTags);
        }
        else
        {
            mTempCssFile.open();
            mTempCssFile.write(ui->tbTemplate->toPlainText().toUtf8());
            mTempCssFile.flush();
            mTemplateTags[QStringLiteral("Style")] = mTempCssFile.fileName();
            ui->webview->renderTemplate(mTemplateTags);
        }
    }
    else
    {
        ui->webview->renderTemplate(mTemplateTags);
    }
}

void WdgWebViewEditable::updateTags()
{
    if (ui->tbTags->isVisible())
    {
        QJsonDocument json = QJsonDocument::fromVariant(mTemplateTags);
        ui->tbTags->setPlainText(json.toJson());
    }
}

void WdgWebViewEditable::updateEditMode()
{
    if (checkSaveTemplate())
    {
        ui->cbEditMode->setChecked(true);
        return;
    }

    bool editMode = ui->cbEditMode->isChecked();

    ui->tbTemplate->setVisible(editMode);
    ui->tbTags->setVisible(editMode);
    ui->lblFilePath->setVisible(editMode);
    ui->btnRestoreTemplate->setVisible(editMode);
    ui->cbTemplateAuswahl->setVisible(editMode);
    ui->btnSaveTemplate->setVisible(false);
    if (editMode)
    {
        QFile file(gSettings->dataDir(1) + ui->cbTemplateAuswahl->currentText());
        ui->btnSaveTemplate->setProperty("file", file.fileName());
        ui->lblFilePath->setText("<a href=\"" + file.fileName() + "\">" + file.fileName() + "</a>");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            file.close();
        }
    }

    updateTags();
    updateWebView();
}

void WdgWebViewEditable::on_sliderZoom_valueChanged(int value)
{
    gZoomFactor = value / 100.0;
    ui->lblZoom->setText(QString::number(value)+"%");
  #ifdef QT_WEBENGINECORE_LIB
    ui->webview->setZoomFactor(gZoomFactor);
  #endif
}

void WdgWebViewEditable::on_sliderZoom_sliderReleased()
{
    ui->lblZoom->clear();
}

void WdgWebViewEditable::on_btnPrint_clicked()
{
    printPreview();
}

void WdgWebViewEditable::on_btnPdf_clicked()
{
    printToPdf();
}
