#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodelsud.h"
#include "model/textdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ratingdelegate.h"
#include "model/spinboxdelegate.h"
#include "model/sudnamedelegate.h"
#include "model/sudwochedelegate.h"
#include "dialogs/dlgsudteilen.h"
#include "dialogs/dlgimportexport.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabSudAuswahl::TabSudAuswahl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSudAuswahl)
{
    ui->setupUi(this);

    ui->webview->setHtmlFile(QStringLiteral("sudinfo"));

    TableView *table = ui->table;
    ProxyModelSud *proxyModel = new ProxyModelSud(this);
    proxyModel->setSourceModel(bh->modelSud());
    table->setModel(proxyModel);
    table->appendCol({ModelSud::ColSudname, true, false, -1, new SudNameDelegate(table)});
    table->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    table->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelSud::ColAnlage, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelSud::ColBraudatum, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColAbfuelldatum, false, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColErstellt, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColGespeichert, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColWoche, true, true, 80, new SudWocheDelegate(table)});
    table->appendCol({ModelSud::ColBewertungMittel, true, true, 80, new RatingDelegate(table)});
    table->appendCol({ModelSud::ColMenge, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSud::ColSW, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSud::ColIBU, false, true, 80, new SpinBoxDelegate(table)});
    table->build();

    table->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table->horizontalHeader(), &QWidget::customContextMenuRequested, this, &TabSudAuswahl::on_table_customContextMenuRequested);
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabSudAuswahl::selectionChanged);

    gSettings->beginGroup(staticMetaObject.className());

    table->restoreState(gSettings->value("tableSudAuswahlState").toByteArray());

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setSizes({100, 200});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    proxyModel->loadSettings(gSettings);

    gSettings->endGroup();

    ui->btnFilter->setModel(proxyModel);

    connect(bh, &Brauhelfer::modified, this, &TabSudAuswahl::databaseModified, Qt::QueuedConnection);
    connect(proxyModel, &ProxyModel::layoutChanged, this, &TabSudAuswahl::filterChanged);
    connect(proxyModel, &ProxyModel::rowsInserted, this, &TabSudAuswahl::filterChanged);
    connect(proxyModel, &ProxyModel::rowsRemoved, this, &TabSudAuswahl::filterChanged);

    ui->btnAnlegen->setAction(ui->actionAnlegen);
    ui->btnKopieren->setAction(ui->actionKopieren);
    ui->btnLoeschen->setAction(ui->actionLoeschen);
    ui->btnImportieren->setAction(ui->actionImportieren);
    ui->btnExportieren->setAction(ui->actionExportieren);
    ui->btnTeilen->setAction(ui->actionTeilen);
    ui->btnMerken->setAction(ui->actionMerken);
    ui->btnLaden->setAction(ui->actionLaden);
    connect(ui->actionAnlegen, &QAction::triggered, this, &TabSudAuswahl::sudAnlegen);
    connect(ui->actionKopieren, &QAction::triggered, this, &TabSudAuswahl::sudKopieren);
    connect(ui->actionLoeschen, &QAction::triggered, this, &TabSudAuswahl::sudLoeschen);
    connect(ui->actionImportieren, &QAction::triggered, this, &TabSudAuswahl::sudImportieren);
    connect(ui->actionExportieren, &QAction::triggered, this, &TabSudAuswahl::sudExportieren);
    connect(ui->actionTeilen, &QAction::triggered, this, &TabSudAuswahl::sudTeilen);
    connect(ui->actionMerken, &QAction::toggled, this, &TabSudAuswahl::sudMerken);
    connect(ui->actionLaden, &QAction::triggered, this, &TabSudAuswahl::sudLaden);

    ui->table->selectRow(0);
    filterChanged();
}

TabSudAuswahl::~TabSudAuswahl()
{
    delete ui;
}

void TabSudAuswahl::saveSettings()
{
    ProxyModelSud *proxyModel = static_cast<ProxyModelSud*>(ui->table->model());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableSudAuswahlState", ui->table->horizontalHeader()->saveState());
    proxyModel->saveSetting(gSettings);
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void TabSudAuswahl::restoreView()
{
    ui->table->restoreDefaultState();
    ui->splitter->restoreState(mDefaultSplitterState);
}

void TabSudAuswahl::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleBewertung))
    {
        bool on = gSettings->isModuleEnabled(Settings::ModuleBewertung);
        ui->table->setCol(8, on, on);
    }
    updateWebView();
}

void TabSudAuswahl::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    updateWebView();
}

void TabSudAuswahl::databaseModified()
{
    if (!isVisible())
        return;
    updateWebView();
}

void TabSudAuswahl::filterChanged()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    ProxyModel proxy;
    proxy.setSourceModel(bh->modelSud());
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}

void TabSudAuswahl::selectionChanged()
{
    bool selected = ui->table->selectionModel()->selectedRows().count() > 0;
    updateWebView();
    ui->btnMerken->setEnabled(selected);
    ui->btnKopieren->setEnabled(selected);
    ui->btnLoeschen->setEnabled(selected);
    ui->btnExportieren->setEnabled(selected);
    ui->btnLaden->setEnabled(selected);
    if (selected)
    {
        ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
        QModelIndex index = ui->table->selectionModel()->selectedRows()[0];
        ui->actionMerken->setChecked(model->data(index.row(), ModelSud::ColMerklistenID).toBool());
    }
}

void TabSudAuswahl::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    if (ui->table->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            sudLoeschen();
            break;
        case Qt::Key::Key_Insert:
            sudAnlegen();
            break;
        case Qt::Key::Key_Return:
            sudLaden();
            break;
        }
    }
}

void TabSudAuswahl::on_table_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    int sudId = model->data(index.row(), ModelSud::ColID).toInt();
    emit clicked(sudId);
}

void TabSudAuswahl::on_table_customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    menu.addAction(ui->actionAnlegen);
    menu.addAction(ui->actionLoeschen);
    menu.addAction(ui->actionKopieren);
    menu.addAction(ui->actionTeilen);
    menu.addSeparator();
    menu.addAction(ui->actionImportieren);
    menu.addAction(ui->actionExportieren);
    menu.addSeparator();
    menu.addAction(ui->actionMerken);
    menu.addSeparator();
    menu.addAction(ui->actionLaden);
    menu.addSeparator();
    ui->table->buildContextMenu(menu);
    menu.exec(ui->table->viewport()->mapToGlobal(pos));
}

void TabSudAuswahl::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelSud*>(ui->table->model())->setFilterText(pattern);
}

void TabSudAuswahl::sudAnlegen()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    model->clearFilter();
    ui->tbFilter->clear();

    QMap<int, QVariant> values({{ModelSud::ColSudname, tr("Neuer Sud")}});
    int row = model->append(values);
    if (row >= 0)
    {
        filterChanged();
        ui->table->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->table->scrollTo(ui->table->currentIndex());
        ui->table->edit(ui->table->currentIndex());
    }
}

void TabSudAuswahl::sudKopieren()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    model->clearFilter();
    ui->tbFilter->clear();

    int row = -1;
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
    {
        int sudId = model->data(index.row(), ModelSud::ColID).toInt();
        QString name = model->data(index.row(), ModelSud::ColSudname).toString() + " " + tr("Kopie");
        row = bh->sudKopieren(sudId, name);
        row = model->mapRowFromSource(row);
    }
    if (row >= 0)
    {
        filterChanged();
        ui->table->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->table->scrollTo(ui->table->currentIndex());
        ui->table->edit(ui->table->currentIndex());
    }
}

void TabSudAuswahl::sudLoeschen()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    QList<int> sudIds;
    sudIds.reserve(ui->table->selectionModel()->selectedRows().count());
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
        sudIds.append(model->data(index.row(), ModelSud::ColID).toInt());
    for (int sudId : sudIds)
    {
        int row = model->getRowWithValue(ModelSud::ColID, sudId);
        if (row >= 0)
        {
            QString name = model->data(row, ModelSud::ColSudname).toString();
            int ret = QMessageBox::question(this, tr("Sud löschen?"),
                                            tr("Soll der Sud \"%1\" gelöscht werden?").arg(name));
            if (ret == QMessageBox::Yes)
            {
                if (model->removeRow(row))
                    filterChanged();
            }
        }
    }
}

void TabSudAuswahl::sudImportieren()
{
    DlgImportExport dlg(true, -1, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudRow = dlg.row();
        if (sudRow >= 0)
        {
            ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
            model->clearFilter();
            ui->tbFilter->clear();
            filterChanged();
            sudRow = model->mapRowFromSource(sudRow);
            ui->table->setCurrentIndex(model->index(sudRow, ModelSud::ColSudname));
            ui->table->scrollTo(ui->table->currentIndex());
        }
    }
}

void TabSudAuswahl::sudExportieren()
{
    QList<int> list;
    list.reserve(ui->table->selectionModel()->selectedRows().count());
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
        list.append(model->mapRowToSource(index.row()));
    for (int row : list)
    {
        DlgImportExport dlg(false, row, this);
        dlg.exec();
    }
}

void TabSudAuswahl::sudTeilen()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
    {
        int sudId = model->data(index.row(), ModelSud::ColID).toInt();
        QString name = model->data(index.row(), ModelSud::ColSudname).toString();
        double menge = model->data(index.row(), ModelSud::ColMengeIst).toDouble();
        DlgSudTeilen dlg(name, menge, this);
        if (dlg.exec() == QDialog::Accepted)
            bh->sudTeilen(sudId, dlg.nameTeil1(), dlg.nameTeil2(), dlg.prozent());
    }
}

void TabSudAuswahl::sudMerken(bool value)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), ModelSud::ColMerklistenID);
        if (model->data(indexMerkliste).toBool() != value)
            model->setData(indexMerkliste, value);
    }
    ui->table->setFocus();
}

void TabSudAuswahl::sudLaden()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.count() > 0)
    {
        int sudId = model->data(selection[0].row(), ModelSud::ColID).toInt();
        emit clicked(sudId);
    }
}
