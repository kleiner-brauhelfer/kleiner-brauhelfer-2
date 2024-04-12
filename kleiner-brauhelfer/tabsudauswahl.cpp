#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QKeyEvent>
#include "brauhelfer.h"
#include "settings.h"
#include "model/proxymodelsudcolored.h"
#include "model/textdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ratingdelegate.h"
#include "model/spinboxdelegate.h"
#include "dialogs/dlgsudteilen.h"
#include "dialogs/dlgimportexport.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabSudAuswahl::TabSudAuswahl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabSudAuswahl)
{
    ui->setupUi(this);

    restoreView();

    ui->webview->setHtmlFile(QStringLiteral("sudinfo"));

    TableView *table = ui->table;
    ProxyModelSudColored *proxyModel = new ProxyModelSudColored(this);
    proxyModel->setSourceModel(bh->modelSud());
    table->setModel(proxyModel);
    table->appendCol({ModelSud::ColSudname, true, false, -1, new TextDelegate(table)});
    table->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    table->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelSud::ColBraudatum, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColAbfuelldatum, false, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColErstellt, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColGespeichert, true, true, 100, new DateDelegate(false, true, table)});
    table->appendCol({ModelSud::ColWoche, true, true, 80, new TextDelegate(table)});
    table->appendCol({ModelSud::ColBewertungMittel, true, true, 80, new RatingDelegate(table)});
    table->appendCol({ModelSud::ColMenge, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSud::ColSW, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    table->appendCol({ModelSud::ColIBU, false, true, 80, new SpinBoxDelegate(table)});
    table->build();
    table->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table->horizontalHeader(), &QWidget::customContextMenuRequested, this, &TabSudAuswahl::on_table_customContextMenuRequested);
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabSudAuswahl::onSelectionChanged);
    connect(table, &QAbstractItemView::doubleClicked, this, &TabSudAuswahl::sudLaden);

    gSettings->beginGroup(staticMetaObject.className());
    table->restoreState(gSettings->value("tableState").toByteArray());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    ProxyModelSud::FilterStatus filterStatus = static_cast<ProxyModelSud::FilterStatus>(gSettings->value("filterStatus", ProxyModelSud::Alle).toInt());
    proxyModel->setFilterStatus(filterStatus);
    proxyModel->setFilterMerkliste(gSettings->value("filterMerkliste", false).toBool());
    proxyModel->setFilterDate(gSettings->value("ZeitraumAlle", false).toBool());
    QDate minDate = gSettings->value("ZeitraumVon", QDate::currentDate().addYears(-1)).toDate();
    QDate maxDate = gSettings->value("ZeitraumBis", QDate::currentDate()).toDate();
    proxyModel->setFilterMinimumDate(QDateTime(minDate, QTime(1,0,0)));
    proxyModel->setFilterMaximumDate(QDateTime(maxDate, QTime(23,59,59)));
    gSettings->endGroup();

    ui->btnFilter->setModel(proxyModel, true);

    connect(bh, &Brauhelfer::modified, this, &TabSudAuswahl::onDatabaseModified, Qt::QueuedConnection);
    connect(gSettings, &Settings::modulesChanged, this, &TabSudAuswahl::onModulesChanged);
    connect(ui->table->model(), &QAbstractItemModel::layoutChanged, this, &TabSudAuswahl::onFilterChanged);

    ui->table->selectRow(0);
    onFilterChanged();
}

TabSudAuswahl::~TabSudAuswahl()
{
    saveSettings();
    delete ui;
}

void TabSudAuswahl::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabSudAuswahl::saveSettings()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableState", ui->table->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("filterStatus", (int)model->filterStatus());
    gSettings->setValue("filterMerkliste", model->filterMerkliste());
    gSettings->setValue("ZeitraumAlle", model->filterDate());
    gSettings->setValue("ZeitraumVon", model->filterMinimumDate().date());
    gSettings->setValue("ZeitraumBis", model->filterMaximumDate().date());
    gSettings->endGroup();
}

void TabSudAuswahl::restoreView()
{
    ui->table->restoreDefaultState();
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);
    ui->splitter->setSizes({100, 200});
}

void TabSudAuswahl::onModulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleBewertung))
    {
        bool on = gSettings->isModuleEnabled(Settings::ModuleBewertung);
        ui->table->setCol(8, on, on);
    }
    updateWebView();
}

void TabSudAuswahl::onDatabaseModified()
{
    if (!isVisible())
        return;
    updateWebView();
}

void TabSudAuswahl::onFilterChanged()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    ProxyModel proxy;
    proxy.setSourceModel(bh->modelSud());
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}

void TabSudAuswahl::onSelectionChanged()
{
    bool sudSelected = ui->table->selectionModel()->selectedRows().count() > 0;
    emit selectionChanged(sudSelected);
    updateWebView();
}

void TabSudAuswahl::keyPressEvent(QKeyEvent *event)
{
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

void TabSudAuswahl::on_table_customContextMenuRequested(const QPoint &pos)
{
    QAction *action;
    QMenu menu(this);
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());

    QModelIndex index = ui->table->indexAt(pos);

    action = new QAction(tr("Sud merken"), &menu);
    action->setCheckable(true);
    action->setChecked(model->data(index.row(), ModelSud::ColMerklistenID).toBool());
    connect(action, &QAction::triggered, this, &TabSudAuswahl::sudMerken);
    menu.addAction(action);

    menu.addSeparator();
    ui->table->buildContextMenu(menu);

    menu.exec(ui->table->viewport()->mapToGlobal(pos));
}

void TabSudAuswahl::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelSud*>(ui->table->model())->setFilterText(pattern);
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

void TabSudAuswahl::sudAnlegen()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    if (!model->filterStatus().testFlag(ProxyModelSud::Rezept))
        model->setFilterStatus(model->filterStatus().setFlag(ProxyModelSud::Rezept));
    model->setFilterMerkliste(false);
    model->setFilterDate(false);
    ui->tbFilter->clear();
    QMap<int, QVariant> values({{ModelSud::ColSudname, tr("Neuer Sud")}});
    int row = model->append(values);
    if (row >= 0)
    {
        onFilterChanged();
        ui->table->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->table->scrollTo(ui->table->currentIndex());
        ui->table->edit(ui->table->currentIndex());
    }
}

void TabSudAuswahl::sudKopieren()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    if (!model->filterStatus().testFlag(ProxyModelSud::Rezept))
        model->setFilterStatus(model->filterStatus().setFlag(ProxyModelSud::Rezept));
    model->setFilterMerkliste(false);
    model->setFilterDate(false);
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
        onFilterChanged();
        ui->table->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->table->scrollTo(ui->table->currentIndex());
        ui->table->edit(ui->table->currentIndex());
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
                    onFilterChanged();
            }
        }
    }
}

void TabSudAuswahl::sudLaden()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.count() > 0)
    {
        int sudId = model->data(selection[0].row(), ModelSud::ColID).toInt();
        emit clicked(sudId);
        qDebug() << "load" << sudId;
    }
}

void TabSudAuswahl::rezeptImportieren()
{
    DlgImportExport dlg(true, -1, this);
    if (dlg.exec() == QDialog::Accepted)
    {
        int sudRow = dlg.row();
        if (sudRow >= 0)
        {
            ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
            if (!model->filterStatus().testFlag(ProxyModelSud::Rezept))
                model->setFilterStatus(model->filterStatus().setFlag(ProxyModelSud::Rezept));
            model->setFilterMerkliste(false);
            model->setFilterDate(false);
            ui->tbFilter->clear();
            onFilterChanged();
            sudRow = model->mapRowFromSource(sudRow);
            ui->table->setCurrentIndex(model->index(sudRow, ModelSud::ColSudname));
            ui->table->scrollTo(ui->table->currentIndex());
        }
    }
}

void TabSudAuswahl::rezeptExportieren()
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
