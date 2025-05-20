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

    QPalette pal = palette();
    pal.setColor(QPalette::Base, gSettings->NichtGebrautBackground);
    ui->cbRezept->setPalette(pal);
    pal.setColor(QPalette::Base, gSettings->GebrautBackground);
    ui->cbGebraut->setPalette(pal);
    pal.setColor(QPalette::Base, gSettings->AbgefuelltBackground);
    ui->cbAbgefuellt->setPalette(pal);
    pal.setColor(QPalette::Base, gSettings->VerbrauchtBackground);
    ui->cbVerbraucht->setPalette(pal);
    pal.setColor(QPalette::Base, gSettings->MekrlisteBackground);
    ui->cbMerkliste->setPalette(pal);

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

    ProxyModelSud::FilterStatus filterStatus = static_cast<ProxyModelSud::FilterStatus>(gSettings->value("filterStatus", ProxyModelSud::Alle).toInt());
    ui->cbRezept->setChecked(filterStatus & ProxyModelSud::Rezept);
    ui->cbGebraut->setChecked(filterStatus & ProxyModelSud::Gebraut);
    ui->cbAbgefuellt->setChecked(filterStatus & ProxyModelSud::Abgefuellt);
    ui->cbVerbraucht->setChecked(filterStatus & ProxyModelSud::Verbraucht);
    setFilterStatus();

    ui->cbMerkliste->setChecked(gSettings->value("filterMerkliste", false).toBool());

    ui->tbDatumVon->setDate(gSettings->value("ZeitraumVon", QDate::currentDate().addYears(-1)).toDate());
    ui->tbDatumBis->setDate(gSettings->value("ZeitraumBis", QDate::currentDate()).toDate());
    ui->cbDatumAlle->setChecked(gSettings->value("ZeitraumAlle", false).toBool());
    setFilterDate();

    gSettings->endGroup();

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
    gSettings->beginGroup("TabSudAuswahl");
    gSettings->setValue("tableSudAuswahlState", ui->tableSudauswahl->horizontalHeader()->saveState());
    gSettings->setValue("filterStatus", (int)static_cast<ProxyModelSud*>(ui->tableSudauswahl->model())->filterStatus());
    gSettings->setValue("filterMerkliste", ui->cbMerkliste->isChecked());
    gSettings->setValue("ZeitraumVon", ui->tbDatumVon->date());
    gSettings->setValue("ZeitraumBis", ui->tbDatumBis->date());
    gSettings->setValue("ZeitraumAlle", ui->cbDatumAlle->isChecked());
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
    ui->lblNumSude->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
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

void TabSudAuswahl::setFilterStatus()
{
    ProxyModelSud::FilterStatus filter = ProxyModelSud::Keine;
    if (ui->cbRezept->isChecked())
        filter |= ProxyModelSud::Rezept;
    if (ui->cbGebraut->isChecked())
        filter |= ProxyModelSud::Gebraut;
    if (ui->cbAbgefuellt->isChecked())
        filter |= ProxyModelSud::Abgefuellt;
    if (ui->cbVerbraucht->isChecked())
        filter |= ProxyModelSud::Verbraucht;
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(filter);
    if (filter == ProxyModelSud::Alle)
        ui->cbAlle->setCheckState(Qt::Checked);
    else if (filter == ProxyModelSud::Keine)
        ui->cbAlle->setCheckState(Qt::Unchecked);
    else
        ui->cbAlle->setCheckState(Qt::PartiallyChecked);
    selectionChanged();
}

void TabSudAuswahl::setFilterDate()
{
    bool notAll = ui->cbDatumAlle->isChecked();
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    if (notAll)
    {
        model->setFilterMinimumDate(QDateTime(ui->tbDatumVon->date(), QTime(0,0,0)));
        model->setFilterMaximumDate(QDateTime(ui->tbDatumBis->date(), QTime(23,59,59)));
    }
    model->setFilterDate(notAll);
    ui->tbDatumVon->setEnabled(notAll);
    ui->tbDatumBis->setEnabled(notAll);
}

void TabSudAuswahl::on_cbAlle_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    if (model->filterStatus() == ProxyModelSud::Alle)
    {
        ui->cbRezept->setChecked(false);
        ui->cbGebraut->setChecked(false);
        ui->cbAbgefuellt->setChecked(false);
        ui->cbVerbraucht->setChecked(false);
    }
    else
    {
        ui->cbRezept->setChecked(true);
        ui->cbGebraut->setChecked(true);
        ui->cbAbgefuellt->setChecked(true);
        ui->cbVerbraucht->setChecked(true);
    }
    setFilterStatus();
}

void TabSudAuswahl::on_cbRezept_clicked()
{
    setFilterStatus();
}

void TabSudAuswahl::on_cbGebraut_clicked()
{
    setFilterStatus();
}

void TabSudAuswahl::on_cbAbgefuellt_clicked()
{
    setFilterStatus();
}

void TabSudAuswahl::on_cbVerbraucht_clicked()
{
    setFilterStatus();
}

void TabSudAuswahl::on_cbMerkliste_stateChanged(int state)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterMerkliste(state);
}

void TabSudAuswahl::on_tbFilter_textChanged(const QString &pattern)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterText(pattern);
}

void TabSudAuswahl::on_tbDatumVon_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    setFilterDate();
}

void TabSudAuswahl::on_tbDatumBis_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    setFilterDate();
}

void TabSudAuswahl::on_cbDatumAlle_stateChanged(int state)
{
    Q_UNUSED(state);
    setFilterDate();
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
    if (!ui->cbRezept->isChecked())
    {
        ui->cbRezept->setChecked(true);
        setFilterStatus();
    }
    ui->cbMerkliste->setChecked(false);
    ui->cbDatumAlle->setChecked(false);
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

    if (!ui->cbRezept->isChecked())
    {
        ui->cbRezept->setChecked(true);
        setFilterStatus();
    }
    ui->cbMerkliste->setChecked(false);
    ui->cbDatumAlle->setChecked(false);
    ui->tbFilter->clear();

    int row = -1;
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
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
            if (!ui->cbRezept->isChecked())
            {
                ui->cbRezept->setChecked(true);
                setFilterStatus();
            }
            ui->cbMerkliste->setChecked(false);
            ui->cbDatumAlle->setChecked(false);
            ui->tbFilter->clear();
            filterChanged();
            ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
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
