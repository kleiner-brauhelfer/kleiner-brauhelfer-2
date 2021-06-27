#include "dlghilfe.h"
#include "ui_dlghilfe.h"
#include "settings.h"

extern Settings* gSettings;

DlgHilfe* DlgHilfe::Dialog = nullptr;

DlgHilfe::DlgHilfe(const QUrl &url, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgHilfe)
{
    ui->setupUi(this);
    setUrl(url);

    gSettings->beginGroup(staticMetaObject.className());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();

    connect(this, &DlgHilfe::finished, this, []{Dialog->deleteLater();Dialog = nullptr;});
}

DlgHilfe::~DlgHilfe()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
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
