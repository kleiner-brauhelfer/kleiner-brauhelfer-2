#include "wdgbemerkung.h"
#include "ui_wdgbemerkung.h"
#include "dialogs/dlgrichtexteditor.h"
#include "settings.h"

extern Settings* gSettings;

WdgBemerkung::WdgBemerkung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgBemerkung)
{
    ui->setupUi(this);
    ui->tbRichText->document()->setBaseUrl(QUrl::fromLocalFile(gSettings->databaseDir()));
    connect(ui->tbRichText, &BemerkungTextBrowser::doubleClicked, this, &WdgBemerkung::on_btnEdit_clicked);
}

WdgBemerkung::~WdgBemerkung()
{
    delete ui;
}

void WdgBemerkung::setToolTip(const QString &text)
{
    ui->tbRichText->setToolTip(text);
}

void WdgBemerkung::setPlaceholderText(const QString &text)
{
    ui->tbRichText->setPlaceholderText(text);
}

void WdgBemerkung::setHtml(const QString& html)
{
    if (Qt::mightBeRichText(html))
        ui->tbRichText->setHtml(html);
    else
        ui->tbRichText->setPlainText(html);
}

QString WdgBemerkung::toHtml() const
{
    if (ui->tbRichText->toPlainText().isEmpty())
        return QStringLiteral("");
    return ui->tbRichText->toHtml();
}

void WdgBemerkung::on_btnEdit_clicked()
{
    ui->btnEdit->setChecked(true);
    DlgRichTextEditor dlg(this);
    dlg.setHtml(toHtml());
    if (dlg.exec() == QDialog::Accepted)
    {
        QString html = dlg.toHtml();
        ui->tbRichText->setHtml(html);
        emit changed(html);
    }
    ui->btnEdit->setChecked(false);
}
