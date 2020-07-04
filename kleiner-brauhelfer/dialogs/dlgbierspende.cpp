#include "dlgbierspende.h"
#include "ui_dlgbierspende.h"
#include "settings.h"

extern Settings* gSettings;

DlgBierspende::DlgBierspende(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgBierspende)
{
    ui->setupUi(this);
    adjustSize();
    gSettings->beginGroup("DlgBierspende");
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();
}

DlgBierspende::~DlgBierspende()
{
    gSettings->beginGroup("DlgBierspende");
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}
