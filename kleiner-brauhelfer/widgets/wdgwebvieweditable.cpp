#include "wdgwebvieweditable.h"
#include "ui_wdgwebvieweditable.h"
#include <QDir>
#include <QMessageBox>
#include <QJsonDocument>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgWebViewEditable::WdgWebViewEditable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgWebViewEditable),
    mTempCssFile(QDir::tempPath() + "/" + QCoreApplication::applicationName() + QLatin1String(".XXXXXX.css"))
{
    ui->setupUi(this);
    ui->tbTemplate->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
  #endif
    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  #if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
  #endif
    ui->btnSaveTemplate->setPalette(gSettings->paletteErrorButton);
    on_cbEditMode_clicked(ui->cbEditMode->isChecked());
}

WdgWebViewEditable::~WdgWebViewEditable()
{
    delete ui;
}

void WdgWebViewEditable::setHtmlFile(const QString& file)
{
    ui->cbTemplateAuswahl->setItemText(0, file);
    ui->webview->setTemplateFile(gSettings->dataDir() + file);
}

void WdgWebViewEditable::printToPdf(const QString& filePath)
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

        ui->webview->printToPdf(filePath);

        mTemplateTags["Style"] = style;
        ui->webview->renderTemplate(mTemplateTags);
        ui->webview->setUpdatesEnabled(true);
    }
    else
    {
        ui->webview->printToPdf(filePath);
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
    ui->btnRestoreTemplate->setVisible(checked);
    ui->cbTemplateAuswahl->setVisible(checked);
    ui->btnSaveTemplate->setVisible(false);
    ui->splitterEditmode->setHandleWidth(checked ? 5 : 0);

    if (checked)
    {
        QFile file(gSettings->dataDir() + ui->cbTemplateAuswahl->currentText());
        ui->btnSaveTemplate->setProperty("file", file.fileName());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ui->tbTemplate->setPlainText(file.readAll());
            file.close();
        }
    }

    updateTags();
    updateHtml();
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
        updateHtml();
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
        QFile file(gSettings->dataDir() + ui->cbTemplateAuswahl->currentText());
        QFile file2(":/data/" + ui->cbTemplateAuswahl->currentText());
        file.remove();
        if (file2.copy(file.fileName()))
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        on_cbEditMode_clicked(ui->cbEditMode->isChecked());
    }
}

void WdgWebViewEditable::updateHtml()
{
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
