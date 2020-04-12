#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "importexport.h"
#include "model/proxymodelsudcolored.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/linklabeldelegate.h"
#include "model/ratingdelegate.h"
#include "model/spinboxdelegate.h"
#include "dialogs/dlgsudteilen.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabSudAuswahl::TabSudAuswahl(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabSudAuswahl)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 0);

    ui->webview->setHtmlFile("sudinfo.html");

    SqlTableModel *model = bh->modelSud();
    model->setHeaderData(ModelSud::ColID, Qt::Horizontal, tr("Sud ID"));
    model->setHeaderData(ModelSud::ColSudnummer, Qt::Horizontal, tr("Sudnummer"));
    model->setHeaderData(ModelSud::ColSudname, Qt::Horizontal, tr("Sudname"));
    model->setHeaderData(ModelSud::ColBraudatum, Qt::Horizontal, tr("Braudatum"));
    model->setHeaderData(ModelSud::ColAbfuelldatum, Qt::Horizontal, tr("Abfülldatum"));
    model->setHeaderData(ModelSud::ColErstellt, Qt::Horizontal, tr("Erstellt"));
    model->setHeaderData(ModelSud::ColGespeichert, Qt::Horizontal, tr("Gespeichert"));
    model->setHeaderData(ModelSud::ColWoche, Qt::Horizontal, tr("Woche"));
    model->setHeaderData(ModelSud::ColBewertungMittel, Qt::Horizontal, tr("Bewertung"));
    model->setHeaderData(ModelSud::ColMenge, Qt::Horizontal, tr("Menge [l]"));
    model->setHeaderData(ModelSud::ColSW, Qt::Horizontal, tr("SW [°P]"));
    model->setHeaderData(ModelSud::ColIBU, Qt::Horizontal, tr("Bittere [IBU]"));
    model->setHeaderData(ModelSud::Colerg_AbgefuellteBiermenge, Qt::Horizontal, tr("Menge [l]"));
    model->setHeaderData(ModelSud::Colerg_Sudhausausbeute, Qt::Horizontal, tr("SHA [%]"));
    model->setHeaderData(ModelSud::ColSWIst, Qt::Horizontal, tr("SW [°P]"));
    model->setHeaderData(ModelSud::ColSREIst, Qt::Horizontal, tr("Restextrakt [°P]"));
    model->setHeaderData(ModelSud::Colerg_S_Gesamt, Qt::Horizontal, tr("Schüttung [kg]"));
    model->setHeaderData(ModelSud::Colerg_Preis, Qt::Horizontal, tr("Kosten [%1/l]").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelSud::Colerg_Alkohol, Qt::Horizontal, tr("Alk. [%]"));
    model->setHeaderData(ModelSud::ColsEVG, Qt::Horizontal, tr("sEVG [%]"));
    model->setHeaderData(ModelSud::ColtEVG, Qt::Horizontal, tr("tEVG [%]"));
    model->setHeaderData(ModelSud::Colerg_EffektiveAusbeute, Qt::Horizontal, tr("Eff. SHA [%]"));
    model->setHeaderData(ModelSud::ColVerdampfungszifferIst, Qt::Horizontal, tr("Verdampfungsziffer [%]"));
    model->setHeaderData(ModelSud::ColAusbeuteIgnorieren, Qt::Horizontal, tr("Für Durchschnitt Ignorieren"));

    QTableView *table = ui->tableSudauswahl;
    QHeaderView *header = table->horizontalHeader();
    ProxyModelSudColored *proxyModel = new ProxyModelSudColored(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    mSpalten.append({ModelSud::ColSudname, true, false, 200, nullptr});
    mSpalten.append({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    mSpalten.append({ModelSud::ColBraudatum, true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({ModelSud::ColAbfuelldatum, false, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({ModelSud::ColErstellt, true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({ModelSud::ColGespeichert, true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({ModelSud::ColWoche, true, true, 80, nullptr});
    mSpalten.append({ModelSud::ColBewertungMittel, true, true, 80, new RatingDelegate(table)});
    mSpalten.append({ModelSud::ColMenge, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    mSpalten.append({ModelSud::ColSW, false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    mSpalten.append({ModelSud::ColIBU, false, true, 80, new SpinBoxDelegate(table)});

    int visualIndex = 0;
    for (const AuswahlSpalten& spalte : mSpalten)
    {
        table->setColumnHidden(spalte.col, !spalte.visible);
        if (spalte.itemDelegate)
            table->setItemDelegateForColumn(spalte.col, spalte.itemDelegate);
        header->resizeSection(spalte.col, spalte.width);
        header->moveSection(header->visualIndex(spalte.col), visualIndex++);
    }
    header->setSectionResizeMode(header->logicalIndex(0), QHeaderView::Stretch);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableSudauswahl_customContextMenuRequested(const QPoint&)));

    gSettings->beginGroup("TabSudAuswahl");

    mDefaultTableState = header->saveState();
    header->restoreState(gSettings->value("tableSudAuswahlState").toByteArray());

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
    ui->cbDatumAlle->setChecked(gSettings->value("ZeitraumAlle", true).toBool());

    gSettings->endGroup();

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()), Qt::QueuedConnection);
    connect(proxyModel, SIGNAL(layoutChanged()), this, SLOT(filterChanged()));
    connect(proxyModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(filterChanged()));
    connect(proxyModel, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(filterChanged()));
    connect(ui->tableSudauswahl->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(selectionChanged()));

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
    ui->tableSudauswahl->horizontalHeader()->restoreState(mDefaultTableState);
    ui->splitter->restoreState(mDefaultSplitterState);
}

QAbstractItemModel* TabSudAuswahl::model() const
{
    return ui->tableSudauswahl->model();
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
    QWidget::keyPressEvent(event);
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

void TabSudAuswahl::on_tableSudauswahl_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int sudId = model->data(index.row(), ModelSud::ColID).toInt();
    clicked(sudId);
}

void TabSudAuswahl::spalteAnzeigen(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableSudauswahl->setColumnHidden(action->data().toInt(), !checked);
}

void TabSudAuswahl::on_tableSudauswahl_customContextMenuRequested(const QPoint &pos)
{
    QAction *action;
    QMenu menu(this);
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());

    QModelIndex index = ui->tableSudauswahl->indexAt(pos);

    if (model->data(index.row(), ModelSud::ColMerklistenID).toBool())
    {
        action = new QAction(tr("Sud vergessen"), &menu);
        connect(action, SIGNAL(triggered()), this, SLOT(on_btnVergessen_clicked()));
        menu.addAction(action);
    }
    else
    {
        action = new QAction(tr("Sud merken"), &menu);
        connect(action, SIGNAL(triggered()), this, SLOT(on_btnMerken_clicked()));
        menu.addAction(action);
    }

    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(index.row(), ModelSud::ColStatus).toInt());
    if (status >= Brauhelfer::SudStatus::Abgefuellt)
    {
        if (status == Brauhelfer::SudStatus::Verbraucht)
        {
            action = new QAction(tr("Sud nicht verbraucht"), &menu);
            connect(action, SIGNAL(triggered()), this, SLOT(onNichtVerbraucht_clicked()));
            menu.addAction(action);
        }
        else
        {
            action = new QAction(tr("Sud verbraucht"), &menu);
            connect(action, SIGNAL(triggered()), this, SLOT(onVerbraucht_clicked()));
            menu.addAction(action);
        }
    }

    menu.addSeparator();

    for (const AuswahlSpalten& spalte : mSpalten)
    {
        if (spalte.canHide)
        {
            action = new QAction(model->headerData(spalte.col, Qt::Horizontal).toString(), &menu);
            action->setCheckable(true);
            action->setChecked(!ui->tableSudauswahl->isColumnHidden(spalte.col));
            action->setData(spalte.col);
            connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
            menu.addAction(action);
        }
    }

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
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterMinimumDate(QDateTime(date.addDays(-1)));
    ui->tbDatumBis->setMinimumDate(date);
}

void TabSudAuswahl::on_tbDatumBis_dateChanged(const QDate &date)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterMaximumDate(QDateTime(date.addDays(1)));
    ui->tbDatumVon->setMaximumDate(date);
}

void TabSudAuswahl::on_cbDatumAlle_stateChanged(int state)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    if (state)
    {
        model->setFilterDateColumn(-1);
    }
    else
    {
        model->setFilterMinimumDate(ui->tbDatumVon->dateTime().addDays(-1));
        model->setFilterMaximumDate(ui->tbDatumBis->dateTime().addDays(1));
        model->setFilterDateColumn(ModelSud::ColBraudatum);
    }
    ui->tbDatumVon->setEnabled(!state);
    ui->tbDatumBis->setEnabled(!state);
}

void TabSudAuswahl::on_btnMerken_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), ModelSud::ColMerklistenID);
        if (!model->data(indexMerkliste).toBool())
            model->setData(indexMerkliste, true);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::on_btnVergessen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), ModelSud::ColMerklistenID);
        if (model->data(indexMerkliste).toBool())
            model->setData(indexMerkliste, false);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::on_btnAlleVergessen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (int row = 0; row < model->rowCount(); ++row)
    {
        QModelIndex index = model->index(row, ModelSud::ColMerklistenID);
        if (model->data(index).toBool())
            model->setData(index, false);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::onVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), ModelSud::ColStatus);
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(indexStatus).toInt());
        if (status == Brauhelfer::SudStatus::Abgefuellt)
            model->setData(indexStatus, static_cast<int>(Brauhelfer::SudStatus::Verbraucht));
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::onNichtVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), ModelSud::ColStatus);
        Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(model->data(indexStatus).toInt());
        if (status == Brauhelfer::SudStatus::Verbraucht)
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
    ui->cbDatumAlle->setChecked(true);
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
    ui->cbDatumAlle->setChecked(true);
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

void TabSudAuswahl::rezeptImportieren()
{
    gSettings->beginGroup("General");
    QString filter;
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    QString filePath = QFileDialog::getOpenFileName(this, tr("Rezept Import"),
                                     path, "MaischeMalzundMehr (*.json);;BeerXML (*.xml)", &filter);
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        try
        {
            int sudRow = -1;
            bool ok = false;
            if (filter == "MaischeMalzundMehr (*.json)")
                ok = ImportExport::importMaischeMalzundMehr(filePath, &sudRow);
            else if (filter == "BeerXML (*.xml)")
                ok = ImportExport::importBeerXml(filePath, &sudRow);
            if (ok)
            {
                QMessageBox::information(this, tr("Rezept Import"), tr("Das Rezept wurde erfolgreich importiert."));
                if (!ui->cbRezept->isChecked())
                {
                    ui->cbRezept->setChecked(true);
                    setFilterStatus();
                }
                ui->cbMerkliste->setChecked(false);
                ui->cbDatumAlle->setChecked(true);
                ui->tbFilter->clear();
                filterChanged();
                ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
                sudRow = model->mapRowFromSource(sudRow);
                ui->tableSudauswahl->setCurrentIndex(model->index(sudRow, ModelSud::ColSudname));
                ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
            }
            else
                QMessageBox::warning(this, tr("Rezept Import"), tr("Das Rezept konnte nicht importiert werden."));
        }
        catch (const std::exception& ex)
        {
            QMessageBox::warning(this, tr("Fehler beim Importieren"), ex.what());
        }
        catch (...)
        {
            QMessageBox::warning(this, tr("Fehler beim Importieren"), QObject::tr("Unbekannter Fehler."));
        }

    }
    gSettings->endGroup();
}

void TabSudAuswahl::on_btnImportieren_clicked()
{
    rezeptImportieren();
}

void TabSudAuswahl::rezeptExportieren(bool loadedSud)
{
    if (loadedSud && !bh->sud()->isLoaded())
        return;

    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    if (loadedSud)
    {
        QString filter;
        QString filePath = QFileDialog::getSaveFileName(this, tr("Sud Export"),
                                         path + "/" + bh->sud()->getSudname(), "MaischeMalzundMehr (*.json);;BeerXML (*.xml)", &filter);
        if (!filePath.isEmpty())
        {
            int row = model->getRowWithValue(ModelSud::ColID, bh->sud()->id());
            gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
            try
            {
                bool ok = false;
                if (filter == "MaischeMalzundMehr (*.json)")
                    ok = ImportExport::exportMaischeMalzundMehr(filePath, model->mapRowToSource(row));
                else if (filter == "BeerXML (*.xml)")
                    ok = ImportExport::exportBeerXml(filePath, model->mapRowToSource(row));
                if (ok)
                    QMessageBox::information(this, tr("Sud Export"), tr("Der Sud wurde erfolgreich exportiert."));
                else
                    QMessageBox::warning(this, tr("Sud Export"), tr("Der Sud konnte nicht exportiert werden."));
            }
            catch (const std::exception& ex)
            {
                QMessageBox::warning(this, tr("Fehler beim Exportieren"), ex.what());
            }
            catch (...)
            {
                QMessageBox::warning(this, tr("Fehler beim Exportieren"), QObject::tr("Unbekannter Fehler."));
            }
        }
    }
    else
    {
        for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
        {
            QString sudname = index.sibling(index.row(), ModelSud::ColSudname).data().toString();
            QString filter;
            QString filePath = QFileDialog::getSaveFileName(this, tr("Sud Export"),
                                             path + "/" + sudname, "MaischeMalzundMehr (*.json);;BeerXML (*.xml)", &filter);
            if (!filePath.isEmpty())
            {
                gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
                try
                {
                    bool ok = false;
                    if (filter == "MaischeMalzundMehr (*.json)")
                        ok = ImportExport::exportMaischeMalzundMehr(filePath, model->mapRowToSource(index.row()));
                    else if (filter == "BeerXML (*.xml)")
                        ok = ImportExport::exportBeerXml(filePath, model->mapRowToSource(index.row()));
                    if (ok)
                        QMessageBox::information(this, tr("Sud Export"), tr("Der Sud wurde erfolgreich exportiert."));
                    else
                        QMessageBox::warning(this, tr("Sud Export"), tr("Der Sud konnte nicht exportiert werden."));
                }
                catch (const std::exception& ex)
                {
                    QMessageBox::warning(this, tr("Fehler beim Exportieren"), ex.what());
                }
                catch (...)
                {
                    QMessageBox::warning(this, tr("Fehler beim Exportieren"), QObject::tr("Unbekannter Fehler."));
                }
            }
        }
    }
    gSettings->endGroup();
}

void TabSudAuswahl::on_btnExportieren_clicked()
{
    rezeptExportieren();
}

void TabSudAuswahl::on_btnLaden_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() > 0)
    {
        int sudId = model->data(selection[0].row(), ModelSud::ColID).toInt();
        clicked(sudId);
    }
}

void TabSudAuswahl::printPreview()
{
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 0)
        return;
    ui->webview->printPreview();
}

void TabSudAuswahl::toPdf()
{
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 0)
        return;

    gSettings->beginGroup("General");

    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString fileName;
    if (selection.count() == 1)
        fileName = static_cast<ProxyModel*>(ui->tableSudauswahl->model())->data(selection[0].row(), ModelSud::ColSudname).toString() + "_" + tr("Rohstoffe");
    else
        fileName = tr("Rohstoffe");

    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + fileName +  ".pdf", "PDF (*.pdf)");
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        QRectF rect = gSettings->value("PrintMargins", QRectF(5, 10, 5, 15)).toRectF();
        QMarginsF margins = QMarginsF(rect.left(), rect.top(), rect.width(), rect.height());
        ui->webview->printToPdf(filePath, margins);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    gSettings->endGroup();
}

void TabSudAuswahl::on_btnToPdf_clicked()
{
    toPdf();
}

void TabSudAuswahl::on_btnPrintPreview_clicked()
{
    printPreview();
}
