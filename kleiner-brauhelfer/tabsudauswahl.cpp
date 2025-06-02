#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeData>
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
    TabAbstract(parent),
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

void TabSudAuswahl::onTabActivated()
{
    updateWebView();
}

void TabSudAuswahl::databaseModified()
{
    if (!isTabActive())
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
    ui->btnVergessen->setEnabled(selected);
    ui->btnKopieren->setEnabled(selected);
    ui->btnLoeschen->setEnabled(selected);
    ui->btnExportieren->setEnabled(selected);
    ui->btnLaden->setEnabled(selected);
}

void TabSudAuswahl::keyPressEvent(QKeyEvent *event)
{
    TabAbstract::keyPressEvent(event);
    if (ui->table->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnAnlegen_clicked();
            break;
        case Qt::Key::Key_Return:
            on_btnLaden_clicked();
            break;
        }
    }
}

void TabSudAuswahl::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void TabSudAuswahl::dropEvent(QDropEvent *event)
{
    for (const QUrl& url : event->mimeData()->urls())
    {
        rezeptImportieren(url.toLocalFile());
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
    QAction *action;
    QMenu menu(this);
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());

    QModelIndex index = ui->table->indexAt(pos);

    action = new QAction(tr("Sud merken"), &menu);
    action->setCheckable(true);
    action->setChecked(model->data(index.row(), ModelSud::ColMerklistenID).toBool());
    connect(action, &QAction::triggered, this, &TabSudAuswahl::onMerkliste_clicked);
    menu.addAction(action);

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(index.row(), ModelSud::ColStatus).toInt());
    if (status >= Brauhelfer::SudStatus::Abgefuellt)
    {
        action = new QAction(tr("Sud ausgetrunken"), &menu);
        action->setCheckable(true);
        action->setChecked(status == Brauhelfer::SudStatus::Verbraucht);
        connect(action, &QAction::triggered, this, &TabSudAuswahl::onVerbraucht_clicked);
        menu.addAction(action);
    }

    menu.addSeparator();
    ui->table->buildContextMenu(menu);

    menu.exec(ui->table->viewport()->mapToGlobal(pos));
}

void TabSudAuswahl::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelSud*>(ui->table->model())->setFilterText(pattern);
}

void TabSudAuswahl::onMerkliste_clicked(bool value)
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

void TabSudAuswahl::onVerbraucht_clicked(bool value)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), ModelSud::ColStatus);
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(indexStatus).toInt());
        if (status == Brauhelfer::SudStatus::Abgefuellt && value)
            model->setData(indexStatus, static_cast<int>(Brauhelfer::SudStatus::Verbraucht));
        else if (status == Brauhelfer::SudStatus::Verbraucht  && !value)
            model->setData(indexStatus, static_cast<int>(Brauhelfer::SudStatus::Abgefuellt));
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
    model->setFilterKategorie("");
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

void TabSudAuswahl::on_btnAnlegen_clicked()
{
    sudAnlegen();
}

void TabSudAuswahl::sudKopieren(bool loadedSud)
{
    if (loadedSud && !bh->sud()->isLoaded())
        return;

    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    if (!model->filterStatus().testFlag(ProxyModelSud::Rezept))
        model->setFilterStatus(model->filterStatus().setFlag(ProxyModelSud::Rezept));
    model->setFilterMerkliste(false);
    model->setFilterDate(false);
    model->setFilterKategorie("");
    ui->tbFilter->clear();

    int row = -1;
    if (loadedSud)
    {
        QString name = bh->sud()->getSudname() + " " + tr("Kopie");
        row = bh->sudKopieren(bh->sud()->id(), name);
        row = model->mapRowFromSource(row);
    }
    else
    {
        for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
        {
            int sudId = model->data(index.row(), ModelSud::ColID).toInt();
            QString name = model->data(index.row(), ModelSud::ColSudname).toString() + " " + tr("Kopie");
            row = bh->sudKopieren(sudId, name);
            row = model->mapRowFromSource(row);
        }
    }
    if (row >= 0)
    {
        filterChanged();
        ui->table->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->table->scrollTo(ui->table->currentIndex());
        ui->table->edit(ui->table->currentIndex());
    }
}

void TabSudAuswahl::on_btnKopieren_clicked()
{
    sudKopieren();
}

void TabSudAuswahl::sudTeilen(bool loadedSud)
{
    if (loadedSud && !bh->sud()->isLoaded())
        return;

    if (loadedSud)
    {
        DlgSudTeilen dlg(bh->sud()->getSudname(), bh->sud()->getMengeIst(), this);
        if (dlg.exec() == QDialog::Accepted)
            bh->sudTeilen(bh->sud()->id(), dlg.nameTeil1(), dlg.nameTeil2(), dlg.prozent());
    }
    else
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
}

void TabSudAuswahl::sudLoeschen(bool loadedSud)
{
    if (loadedSud && !bh->sud()->isLoaded())
        return;

    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    if (loadedSud)
    {
        int row = model->getRowWithValue(ModelSud::ColID, bh->sud()->id());
        QString name = bh->sud()->getSudname();
        int ret = QMessageBox::question(this, tr("Sud löschen?"),
                                        tr("Soll der Sud \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            if (model->removeRow(row))
                filterChanged();
        }
    }
    else
    {
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
}

void TabSudAuswahl::on_btnLoeschen_clicked()
{
    sudLoeschen();
}

void TabSudAuswahl::rezeptImportieren(const QString& filePath)
{
    DlgImportExport dlg(true, -1, this);
    if (!filePath.isEmpty())
        dlg.oeffnen(filePath);
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
            model->setFilterKategorie("");
            ui->tbFilter->clear();
            filterChanged();
            sudRow = model->mapRowFromSource(sudRow);
            ui->table->setCurrentIndex(model->index(sudRow, ModelSud::ColSudname));
            ui->table->scrollTo(ui->table->currentIndex());
        }
    }
}

void TabSudAuswahl::on_btnImportieren_clicked()
{
    rezeptImportieren();
}

void TabSudAuswahl::rezeptExportieren(bool loadedSud)
{
    if (loadedSud && !bh->sud()->isLoaded())
        return;

    QList<int> list;
    if (loadedSud)
    {
        list.append(bh->sud()->row());
    }
    else
    {
        list.reserve(ui->table->selectionModel()->selectedRows().count());
        ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
        for (const QModelIndex &index : ui->table->selectionModel()->selectedRows())
            list.append(model->mapRowToSource(index.row()));
    }

    for (int row : list)
    {
        DlgImportExport dlg(false, row, this);
        dlg.exec();
    }
}

void TabSudAuswahl::on_btnExportieren_clicked()
{
    rezeptExportieren();
}

void TabSudAuswahl::on_btnTeilen_clicked()
{
    sudTeilen(false);
}

void TabSudAuswahl::on_btnLaden_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    QModelIndexList selection = ui->table->selectionModel()->selectedRows();
    if (selection.count() > 0)
    {
        int sudId = model->data(selection[0].row(), ModelSud::ColID).toInt();
        emit clicked(sudId);
    }
}

bool TabSudAuswahl::isPrintable() const
{
    return true;
}

void TabSudAuswahl::printPreview()
{
    ui->webview->printPreview();
}

void TabSudAuswahl::toPdf()
{
    ui->webview->printToPdf();
}
