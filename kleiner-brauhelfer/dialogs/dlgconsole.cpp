#include "dlgconsole.h"
#include "ui_dlgconsole.h"
#include "settings.h"
#include <QFontDatabase>

extern Settings* gSettings;

DlgConsole* DlgConsole::Dialog = nullptr;

DlgConsole::DlgConsole(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgConsole)
{
    ui->setupUi(this);
    ui->textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->sbLevel->setValue(gSettings->logLevel());
}

DlgConsole::~DlgConsole()
{
    delete ui;
}

void DlgConsole::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

void DlgConsole::on_btnClear_clicked()
{
    ui->textEdit->clear();
}

void DlgConsole::append(const QString &text)
{
    ui->textEdit->appendPlainText(text);
}

void DlgConsole::on_sbLevel_valueChanged(int level)
{
    gSettings->setLogLevel(level);
}
