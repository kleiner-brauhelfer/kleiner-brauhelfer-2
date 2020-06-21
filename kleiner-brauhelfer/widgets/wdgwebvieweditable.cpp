#include "wdgwebvieweditable.h"
#include "ui_wdgwebvieweditable.h"
#include <QDir>
#include <QMessageBox>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QJsonDocument>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

double WdgWebViewEditable::gZoomFactor = 1.0;

WdgWebViewEditable::WdgWebViewEditable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWebViewEditable),
    mTempCssFile(QDir::tempPath() + "/" + QCoreApplication::applicationName() + QLatin1String(".XXXXXX.css"))
{
    ui->setupUi(this);
    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTemplate->setTabStopDistance(QFontMetrics(ui->tbTemplate->font()).horizontalAdvance("  "));
   #else
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
   #endif
  #endif
    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
   #if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    ui->tbTags->setTabStopDistance(QFontMetrics(ui->tbTags->font()).horizontalAdvance("  "));
   #else
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTags->font()).width(' '));
   #endif
  #endif
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    mTimerWebViewUpdate.setSingleShot(true);
    connect(&mTimerWebViewUpdate, SIGNAL(timeout()), this, SLOT(updateWebView()), Qt::QueuedConnection);
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    gSettings->beginGroup("General");
    gZoomFactor = gSettings->value("WebViewZoomFactor", 1.0).toDouble();
    gSettings->endGroup();
}

WdgWebViewEditable::~WdgWebViewEditable()
{
    delete ui;
    gSettings->beginGroup("General");
    gSettings->setValue("WebViewZoomFactor", gZoomFactor);
    gSettings->endGroup();
}

void WdgWebViewEditable::clear()
{
    ui->webview->setHtml("");
}

void WdgWebViewEditable::setHtmlFile(const QString& file)
{
    ui->cbTemplateAuswahl->setItemText(0, file);
    ui->webview->setTemplateFile(gSettings->dataDir(1) + file);
}

void WdgWebViewEditable::printDocument(QPrinter *printer)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    bool success = false;
    QEventLoop loop;
    ui->webview->page()->print(printer, [&](bool _success) { success = _success; loop.quit(); });
    loop.exec();
    if (success)
    {
        gSettings->beginGroup("General");
        gSettings->setValue("DefaultPrinter", printer->printerName());
        QRectF rect(printer->margins().left, printer->margins().top, printer->margins().right, printer->margins().bottom);
        gSettings->setValue("PrintMargins", rect);
        gSettings->endGroup();
    }
#else
  Q_UNUSED(printer)
#endif
}

void WdgWebViewEditable::printPreview()
{
    QVariant style;
    if (gSettings->theme() == Settings::Dark)
    {
        style = mTemplateTags["Style"];
        mTemplateTags["Style"] = "style_hell.css";
        ui->webview->setUpdatesEnabled(false);

        QEventLoop loop;
        connect(ui->webview, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));
        ui->webview->renderTemplate(mTemplateTags);
        loop.exec();
    }

    gSettings->beginGroup("General");

    QPrinterInfo printerInfo = QPrinterInfo::printerInfo(gSettings->value("DefaultPrinter").toString());
    QPrinter printer(printerInfo, QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setColorMode(QPrinter::Color);
    QRectF rect = gSettings->value("PrintMargins", QRectF(5, 10, 5, 15)).toRectF();
    printer.setPageMargins(rect.left(), rect.top(), rect.width(), rect.height(), QPrinter::Millimeter);

    gSettings->endGroup();

    QPrintPreviewDialog dlg(&printer, ui->webview->page()->view());
    connect(&dlg, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
    dlg.exec();

    if (gSettings->theme() == Settings::Dark)
    {
        mTemplateTags["Style"] = style;
        ui->webview->renderTemplate(mTemplateTags);
        ui->webview->setUpdatesEnabled(true);
    }
}

void WdgWebViewEditable::printToPdf(const QString& filePath, const QMarginsF &margins)
{
    if (gSettings->theme() == Settings::Dark)
    {
        QVariant style = mTemplateTags["Style"];
        mTemplateTags["Style"] = "style_hell.css";
        ui->webview->setUpdatesEnabled(false);

        QEventLoop loop;
        connect(ui->webview, SIGNAL(loadFinished(bool)), &loop, SLOT(quit()));
        ui->webview->renderTemplate(mTemplateTags);
        loop.exec();

        ui->webview->printToPdf(filePath, margins);

        mTemplateTags["Style"] = style;
        ui->webview->renderTemplate(mTemplateTags);
        ui->webview->setUpdatesEnabled(true);
    }
    else
    {
        ui->webview->printToPdf(filePath, margins);
    }
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
    if (checkSaveTemplate())
    {
        ui->cbEditMode->setChecked(true);
        return;
    }

    ui->tbTemplate->setVisible(checked);
    ui->tbTags->setVisible(checked);
    ui->lblFilePath->setVisible(checked);
    ui->btnRestoreTemplate->setVisible(checked);
    ui->cbTemplateAuswahl->setVisible(checked);
    ui->btnSaveTemplate->setVisible(false);
    ui->splitterEditmode->setHandleWidth(checked ? 5 : 0);
    ui->splitterEditmode->setSizes({1, checked ? 1 : 0});

    if (checked)
    {
        QFile file(gSettings->dataDir(1) + ui->cbTemplateAuswahl->currentText());
        ui->btnSaveTemplate->setProperty("file", file.fileName());
        ui->lblFilePath->setText(file.fileName());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            file.close();
        }
    }

    updateTags();
    updateWebView();
}

void WdgWebViewEditable::on_cbTemplateAuswahl_currentIndexChanged(const QString &fileName)
{
    Q_UNUSED(fileName)
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
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
        on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    }
}

void WdgWebViewEditable::updateWebView()
{
    if (ui->webview->zoomFactor() != gZoomFactor)
    {
        ui->webview->setZoomFactor(gZoomFactor);
        ui->sliderZoom->setValue(gZoomFactor * 100);
    }
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
            mTemplateTags["Style"] = mTempCssFile.fileName();
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

void WdgWebViewEditable::on_sliderZoom_valueChanged(int value)
{
    if (ui->sliderZoom->hasFocus())
    {
        gZoomFactor = value / 100.0;
        ui->webview->setZoomFactor(gZoomFactor);
    }
}
