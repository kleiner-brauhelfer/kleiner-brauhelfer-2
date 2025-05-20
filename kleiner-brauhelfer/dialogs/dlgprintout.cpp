#include "dlgprintout.h"
#include "ui_dlgprintout.h"
#include <QDirIterator>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"
#include "model/textdelegate.h"
#include "model/datedelegate.h"
#include "model/sudnamedelegate.h"
#include "model/spinboxdelegate.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgPrintout* DlgPrintout::Dialog = nullptr;

DlgPrintout::DlgPrintout(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DlgPrintout)
{
    ui->setupUi(this);

    TableView *table = ui->table;
    ProxyModelSud *proxyModel = new ProxyModelSud(this);
    proxyModel->setSourceModel(bh->modelSud());
    table->setModel(proxyModel);
    table->appendCol({ModelSud::ColSudname, true, false, -1, new SudNameDelegate(table)});
    table->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    table->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelSud::ColBraudatum, false, true, 100, new DateDelegate(false, true, table)});
    table->build();
    table->setDefaultContextMenu();
    table->setContextMenuFeatures(false, false);

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

    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    ui->splitter->setSizes({300, 300});

    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgPrintout::updateWebView, Qt::QueuedConnection);
    connect(ui->cbAuswahl, &QComboBox::currentTextChanged, this, &DlgPrintout::updateWebView, Qt::QueuedConnection);
    connect(bh, &Brauhelfer::modified, this, &DlgPrintout::updateWebView, Qt::QueuedConnection);
    connect(bh, &Brauhelfer::discarded, this, &DlgPrintout::updateWebView, Qt::QueuedConnection);
    connect(gSettings, &Settings::modulesChanged, this, &DlgPrintout::updateWebView);

    connect(proxyModel, &ProxyModel::layoutChanged, this, &DlgPrintout::onFilterChanged);
    connect(proxyModel, &ProxyModel::rowsInserted, this, &DlgPrintout::onFilterChanged);
    connect(proxyModel, &ProxyModel::rowsRemoved, this, &DlgPrintout::onFilterChanged);

    if (bh->sud()->isLoaded())
        selectSud(bh->sud()->id());
    else
        ui->table->selectRow(0);
}

DlgPrintout::~DlgPrintout()
{
    delete ui;
}

void DlgPrintout::saveSettings()
{
    ProxyModelSud *proxyModel = static_cast<ProxyModelSud*>(ui->table->model());
    gSettings->beginGroup(staticMetaObject.className());
    proxyModel->saveSetting(gSettings);
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("tableState", ui->table->horizontalHeader()->saveState());
    gSettings->setValue("auswahl", ui->cbAuswahl->currentText());
    gSettings->endGroup();
}

void DlgPrintout::loadSettings()
{
    ProxyModelSud *proxyModel = static_cast<ProxyModelSud*>(ui->table->model());
    gSettings->beginGroup(staticMetaObject.className());
    proxyModel->loadSettings(gSettings);
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    ui->table->restoreState(gSettings->value("tableState").toByteArray());
    ui->cbAuswahl->setCurrentText(gSettings->value("auswahl", "spickzettel").toString());
    gSettings->endGroup();
    ui->btnFilter->setModel(proxyModel);
    ui->table->scrollTo(ui->table->currentIndex());
}

void DlgPrintout::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className(), Dialog);
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("splitterState");
    gSettings->endGroup();
}

void DlgPrintout::selectSud(int id)
{
    ProxyModelSud *proxyModel = static_cast<ProxyModelSud*>(ui->table->model());
    int row = proxyModel->getRowWithValue(ModelSud::ColID, id);
    if (row)
        ui->table->selectRow(row);
}

void DlgPrintout::updateWebView()
{
    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.count() == 0)
    {
        ui->webview->clear();
    }
    else if (selection.count() == 1)
    {
        const ProxyModel *proxyModel = static_cast<ProxyModel*>(ui->table->model());
        int sudRow = proxyModel->mapRowToSource(selection[0].row());
        ui->webview->setHtmlFile(ui->cbAuswahl->currentText());
        ui->webview->setPdfName(proxyModel->data(selection[0].row(), ModelSud::ColSudname).toString());
        TemplateTags::render(ui->webview, sudRow);
    }
}

void DlgPrintout::onFilterChanged()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    ProxyModel proxy;
    proxy.setSourceModel(bh->modelSud());
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}

void DlgPrintout::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelSud*>(ui->table->model())->setFilterText(pattern);
}
