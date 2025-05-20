#include "dlghilfe.h"
#include "ui_dlghilfe.h"

DlgHilfe* DlgHilfe::Dialog = nullptr;

DlgHilfe::DlgHilfe(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgHilfe)
{
    ui->setupUi(this);
    ui->webview->setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(ui->webview, &WebView::urlChanged, this, &DlgHilfe::urlChanged);
}

DlgHilfe::~DlgHilfe()
{
    ui->webview->setUrl(QUrl(QStringLiteral("")));
    delete ui;
}

void DlgHilfe::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgHilfe::setHomeUrl(const QUrl &url)
{
    homeUrl = url;
    setUrl(homeUrl);
}

void DlgHilfe::setUrl(const QUrl &url)
{
  #ifdef QT_WEBENGINECORE_LIB
    ui->webview->setUrl(url);
  #else
    Q_UNUSED(url)
    ui->webview->setHtml(tr("Nicht unterstützt."));
  #endif
}

void DlgHilfe::urlChanged(const QUrl &url)
{
    ui->lblUrl->setText(url.toString());
}

void DlgHilfe::on_btnHome_clicked()
{
    setUrl(homeUrl);
}
