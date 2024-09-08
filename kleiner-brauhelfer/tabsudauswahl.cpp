#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeData>
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
    TabAbstract(parent),
    ui(new Ui::TabSudAuswahl)
{
    ui->setupUi(this);

    ui->webview->setHtmlFile(QStringLiteral("sudinfo"));

    TableView *table = ui->tableSudauswahl;
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
    connect(table->horizontalHeader(), &QWidget::customContextMenuRequested, this, &TabSudAuswahl::on_tableSudauswahl_customContextMenuRequested);

    gSettings->beginGroup("TabSudAuswahl");

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
    connect(ui->tableSudauswahl->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TabSudAuswahl::selectionChanged);

    ui->tableSudauswahl->selectRow(0);
    filterChanged();
}

TabSudAuswahl::~TabSudAuswahl()
{
    delete ui;
}

void TabSudAuswahl::saveSettings()
{
    ProxyModelSud *proxyModel = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    gSettings->beginGroup("TabSudAuswahl");
    gSettings->setValue("tableSudAuswahlState", ui->tableSudauswahl->horizontalHeader()->saveState());
    proxyModel->saveSetting(gSettings);
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void TabSudAuswahl::restoreView()
{
    ui->tableSudauswahl->restoreDefaultState();
    ui->splitter->restoreState(mDefaultSplitterState);
}

void TabSudAuswahl::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleBewertung))
    {
        bool on = gSettings->isModuleEnabled(Settings::ModuleBewertung);
        ui->tableSudauswahl->setCol(8, on, on);
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
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    ProxyModel proxy;
    proxy.setSourceModel(bh->modelSud());
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}

void TabSudAuswahl::selectionChanged()
{
    bool selected = ui->tableSudauswahl->selectionModel()->selectedRows().count() > 0;
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
    if (ui->tableSudauswahl->hasFocus())
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

void TabSudAuswahl::on_tableSudauswahl_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int sudId = model->data(index.row(), ModelSud::ColID).toInt();
    emit clicked(sudId);
}

void TabSudAuswahl::on_tableSudauswahl_customContextMenuRequested(const QPoint &pos)
{
    QAction *action;
    QMenu menu(this);
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());

    QModelIndex index = ui->tableSudauswahl->indexAt(pos);

    action = new QAction(tr("Sud merken"), &menu);
    action->setCheckable(true);
    action->setChecked(model->data(index.row(), ModelSud::ColMerklistenID).toBool());
    connect(action, &QAction::triggered, this, &TabSudAuswahl::onMerkliste_clicked);
    menu.addAction(action);

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(index.row(), ModelSud::ColStatus).toInt());
    if (status >= Brauhelfer::SudStatus::Abgefuellt)
    {
        action = new QAction(tr("Sud verbraucht"), &menu);
        action->setCheckable(true);
        action->setChecked(status == Brauhelfer::SudStatus::Verbraucht);
        connect(action, &QAction::triggered, this, &TabSudAuswahl::onVerbraucht_clicked);
        menu.addAction(action);
    }

    menu.addSeparator();
    ui->tableSudauswahl->buildContextMenu(menu);

    menu.exec(ui->tableSudauswahl->viewport()->mapToGlobal(pos));
}

void TabSudAuswahl::on_tbFilter_textChanged(const QString &pattern)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterText(pattern);
}

void TabSudAuswahl::on_btnMerken_clicked()
{
    onMerkliste_clicked(true);
}

void TabSudAuswahl::on_btnVergessen_clicked()
{
    onMerkliste_clicked(false);
}

void TabSudAuswahl::onMerkliste_clicked(bool value)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), ModelSud::ColMerklistenID);
        if (model->data(indexMerkliste).toBool() != value)
            model->setData(indexMerkliste, value);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::onVerbraucht_clicked(bool value)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), ModelSud::ColStatus);
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(indexStatus).toInt());
        if (status == Brauhelfer::SudStatus::Abgefuellt && value)
            model->setData(indexStatus, static_cast<int>(Brauhelfer::SudStatus::Verbraucht));
        else if (status == Brauhelfer::SudStatus::Verbraucht  && !value)
            model->setData(indexStatus, static_cast<int>(Brauhelfer::SudStatus::Abgefuellt));
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::sudAnlegen()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
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
        ui->tableSudauswahl->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
        ui->tableSudauswahl->edit(ui->tableSudauswahl->currentIndex());
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

    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
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
        for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
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
        ui->tableSudauswahl->setCurrentIndex(model->index(row, ModelSud::ColSudname));
        ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
        ui->tableSudauswahl->edit(ui->tableSudauswahl->currentIndex());
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
        ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
        for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
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

    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
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
        sudIds.reserve(ui->tableSudauswahl->selectionModel()->selectedRows().count());
        for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
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
            ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
            if (!model->filterStatus().testFlag(ProxyModelSud::Rezept))
                model->setFilterStatus(model->filterStatus().setFlag(ProxyModelSud::Rezept));
            model->setFilterMerkliste(false);
            model->setFilterDate(false);
            model->setFilterKategorie("");
            ui->tbFilter->clear();
            filterChanged();
            sudRow = model->mapRowFromSource(sudRow);
            ui->tableSudauswahl->setCurrentIndex(model->index(sudRow, ModelSud::ColSudname));
            ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
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
        list.reserve(ui->tableSudauswahl->selectionModel()->selectedRows().count());
        ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
        for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
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
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
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
