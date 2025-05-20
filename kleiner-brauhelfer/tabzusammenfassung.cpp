#include "tabzusammenfassung.h"
#include "ui_tabzusammenfassung.h"
#include <QDirIterator>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabZusammenfassung::TabZusammenfassung(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabZusammenfassung)
{
    ui->setupUi(this);
    updateAuswahl();

    gSettings->beginGroup("TabZusammenfassung");
    ui->cbAuswahl->setCurrentText(gSettings->value("auswahl", "spickzettel").toString());
    gSettings->endGroup();

    connect(bh, &Brauhelfer::modified, this, &TabZusammenfassung::updateWebView, Qt::QueuedConnection);
    connect(bh, &Brauhelfer::discarded, this, &TabZusammenfassung::updateWebView, Qt::QueuedConnection);
    connect(bh->sud(), &SudObject::loadedChanged, this, &TabZusammenfassung::updateWebView, Qt::QueuedConnection);
}

TabZusammenfassung::~TabZusammenfassung()
{
    delete ui;
}

void TabZusammenfassung::saveSettings()
{
    gSettings->beginGroup("TabZusammenfassung");
    gSettings->setValue("auswahl", ui->cbAuswahl->currentText());
    gSettings->endGroup();
}

void TabZusammenfassung::onTabActivated()
{
    updateWebView();
}

void TabZusammenfassung::modulesChanged(Settings::Modules modules)
{
    Q_UNUSED(modules)
    updateWebView();
}

void TabZusammenfassung::updateAuswahl()
{
    QStringList lst;
    QDirIterator it(gSettings->dataDir(1), QStringList() << QStringLiteral("*.html"));
    while (it.hasNext())
    {
        it.next();
        QString filename = it.fileName();
        filename.chop(5);
        int lastUnderscore = filename.lastIndexOf('_');
        if (filename.length() - lastUnderscore == 3)
            filename = filename.left(lastUnderscore);
        if (!lst.contains(filename) && filename != QStringLiteral("sudinfo"))
            lst.append(filename);
    }
    ui->cbAuswahl->clear();
    ui->cbAuswahl->addItems(lst);
}

void TabZusammenfassung::updateWebView()
{
    if (!isTabActive())
        return;
    ui->webview->setHtmlFile(ui->cbAuswahl->currentText());
    ui->webview->setPdfName(bh->sud()->getSudname());
    TemplateTags::render(ui->webview, bh->sud()->row());
}

bool TabZusammenfassung::isPrintable() const
{
    return true;
}

void TabZusammenfassung::printPreview()
{
    ui->webview->printPreview();
}

void TabZusammenfassung::toPdf()
{
    ui->webview->printToPdf();
}

void TabZusammenfassung::on_cbAuswahl_currentTextChanged(const QString &txt)
{
    Q_UNUSED(txt)
    updateWebView();
}
