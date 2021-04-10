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
}

WdgBemerkung::~WdgBemerkung()
{
    delete ui;
}

void WdgBemerkung::setToolTip(const QString &text)
{
    ui->tbRichText->setToolTip(text);
}

void WdgBemerkung::setHtml(const QString& html)
{
    ui->tbRichText->setHtml(html);
}

QString WdgBemerkung::toHtml() const
{
    return DlgRichTextEditor::stripHeader(ui->tbRichText->toHtml());
}

void WdgBemerkung::on_btnEdit_clicked()
{
    DlgRichTextEditor dlg(this);
    dlg.setHtml(toHtml());
    if (dlg.exec() == QDialog::Accepted)
    {
        QString html = dlg.toHtml();
        ui->tbRichText->setHtml(html);
        emit changed(html);
    }
}
