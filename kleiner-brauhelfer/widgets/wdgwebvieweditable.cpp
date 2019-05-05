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
    ui->tbTemplate->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
    mHtmlHightLighter = new HtmlHighLighter(ui->tbTemplate->document());
    ui->tbTags->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->tbTags->setTabStopDistance(2 * QFontMetrics(ui->tbTemplate->font()).width(' '));
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
    ui->webview->printToPdf(filePath);
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

void WdgWebViewEditable::updateAll(int sudRow)
{
    mTemplateTags.clear();
    erstelleTagListe(mTemplateTags, sudRow);
    updateTags();
    updateHtml();
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
    QJsonDocument json = QJsonDocument::fromVariant(mTemplateTags);
    ui->tbTags->setPlainText(json.toJson());
}

void WdgWebViewEditable::erstelleTagListe(QVariantMap &ctx, int sudRow)
{
    QLocale locale = QLocale();

    QVariantMap ctxApp;
    ctxApp["Name"] = QCoreApplication::applicationName();
    ctxApp["Version"] = QCoreApplication::applicationVersion();
    ctx["App"] = ctxApp;

    ctx["Style"] = gSettings->theme() == Settings::Dark ? "style_dunkel.css" : "style_hell.css";

    if (sudRow >= 0)
    {
        QVariantMap ctxRezept;
        ctxRezept["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SW").toDouble(), 'f', 1);
        ctxRezept["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "Menge").toDouble(), 'f', 1);
        ctxRezept["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IBU").toDouble());
        ctxRezept["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2").toDouble(), 'f', 1);
        ctxRezept["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble(), 'f', 0);
        ctxRezept["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "erg_Farbe").toDouble())).name();
        ctxRezept["Nachisomerisierung"] = QString::number(bh->modelSud()->data(sudRow, "Nachisomerisierungszeit").toInt());
        ctxRezept["Brauanlage"] = bh->modelSud()->data(sudRow, "Anlage").toString();
        ctxRezept["Name"] = bh->modelSud()->data(sudRow, "Sudname").toString();
        ctxRezept["Nummer"] = QString::number(bh->modelSud()->data(sudRow, "Sudnummer").toDouble());
        ctxRezept["Kommentar"] = bh->modelSud()->data(sudRow, "Kommentar").toString().replace("\n", "<br>");
        ctx["Rezept"] = ctxRezept;

        QVariantMap ctxSud;
        int status = bh->modelSud()->data(sudRow, "Status").toInt();
        ctxSud["Status"] = QString::number(status);
        ctxSud["StatusRezept"] = status == Sud_Status_Rezept ? "1" : "";
        ctxSud["StatusGebraut"] = status == Sud_Status_Gebraut ? "1" : "";
        ctxSud["StatusAbgefuellt"] = status == Sud_Status_Abgefuellt ? "1" : "";
        ctxSud["StatusVerbraucht"] = status == Sud_Status_Verbraucht ? "1" : "";
        ctxSud["SW"] = locale.toString(bh->modelSud()->data(sudRow, "SWIst").toDouble(), 'f', 1);
        ctxSud["Menge"] = locale.toString(bh->modelSud()->data(sudRow, "MengeIst").toDouble(), 'f', 1);
        ctxSud["Bittere"] = QString::number(bh->modelSud()->data(sudRow, "IbuIst").toInt());
        ctxSud["CO2"] = locale.toString(bh->modelSud()->data(sudRow, "CO2Ist").toDouble(), 'f', 1);
        ctxSud["Farbe"] = locale.toString(bh->modelSud()->data(sudRow, "FarbeIst").toDouble(), 'f', 0);
        ctxSud["Braudatum"] = locale.toString(bh->modelSud()->data(sudRow, "Braudatum").toDate(), QLocale::ShortFormat);
        ctxSud["Abfuelldatum"] = locale.toString(bh->modelSud()->data(sudRow, "Abfuelldatum").toDate(), QLocale::ShortFormat);
        ctxSud["FarbeRgb"] = QColor(BierCalc::ebcToColor(bh->modelSud()->data(sudRow, "FarbeIst").toDouble())).name();
        ctxSud["Bewertung"] = QString::number(bh->modelSud()->data(sudRow, "BewertungMax").toInt());
        ctxSud["Alkohol"] = locale.toString(bh->modelSud()->data(sudRow, "erg_Alkohol").toDouble(), 'f', 1);
        ctxSud["tEVG"] = locale.toString(bh->modelSud()->data(sudRow, "tEVG").toDouble(), 'f', 1);
        ctxSud["effSHA"] = locale.toString(bh->modelSud()->data(sudRow, "erg_EffektiveAusbeute").toDouble(), 'f', 1);
        ctx["Sud"] = ctxSud;

        QVariantMap ctxTag;
        int sudId = bh->modelSud()->data(sudRow, "ID").toInt();
        for (int col = 0; col < bh->modelFlaschenlabelTags()->rowCount(); ++col)
        {
            int id = bh->modelFlaschenlabelTags()->data(col, "SudID").toInt();
            if (id == sudId || id < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(col, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(col, "Value").toString();
                if (!t.isEmpty())
                    ctxTag[t] = v;
            }
        }
        ctx["Tag"] = ctxTag;
    }
    else
    {
        QVariantMap ctxTag;
        for (int col = 0; col < bh->modelFlaschenlabelTags()->rowCount(); ++col)
        {
            int id = bh->modelFlaschenlabelTags()->data(col, "SudID").toInt();
            if (id < 0)
            {
                QString t = bh->modelFlaschenlabelTags()->data(col, "TagName").toString();
                QString v = bh->modelFlaschenlabelTags()->data(col, "Value").toString();
                if (!t.isEmpty())
                    ctxTag[t] = v;
            }
        }
        ctx["Tag"] = ctxTag;
    }
}
