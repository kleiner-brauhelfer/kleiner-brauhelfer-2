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
    model->setHeaderData(model->fieldIndex("Sudnummer"), Qt::Horizontal, tr("Sudnummer"));
    model->setHeaderData(model->fieldIndex("Sudname"), Qt::Horizontal, tr("Sudname"));
    model->setHeaderData(model->fieldIndex("Braudatum"), Qt::Horizontal, tr("Braudatum"));
    model->setHeaderData(model->fieldIndex("Abfuelldatum"), Qt::Horizontal, tr("Abfülldatum"));
    model->setHeaderData(model->fieldIndex("Erstellt"), Qt::Horizontal, tr("Erstellt"));
    model->setHeaderData(model->fieldIndex("Gespeichert"), Qt::Horizontal, tr("Gespeichert"));
    model->setHeaderData(model->fieldIndex("Woche"), Qt::Horizontal, tr("Woche"));
    model->setHeaderData(model->fieldIndex("BewertungMittel"), Qt::Horizontal, tr("Bewertung"));
    model->setHeaderData(model->fieldIndex("Menge"), Qt::Horizontal, tr("Menge [l]"));
    model->setHeaderData(model->fieldIndex("SW"), Qt::Horizontal, tr("SW [°P]"));
    model->setHeaderData(model->fieldIndex("IBU"), Qt::Horizontal, tr("Bittere [IBU]"));
    model->setHeaderData(model->fieldIndex("erg_AbgefuellteBiermenge"), Qt::Horizontal, tr("Menge [l]"));
    model->setHeaderData(model->fieldIndex("erg_Sudhausausbeute"), Qt::Horizontal, tr("SHA [%]"));
    model->setHeaderData(model->fieldIndex("SWIst"), Qt::Horizontal, tr("SW [°P]"));
    model->setHeaderData(model->fieldIndex("erg_S_Gesamt"), Qt::Horizontal, tr("Schüttung [kg]"));
    model->setHeaderData(model->fieldIndex("erg_Preis"), Qt::Horizontal, tr("Kosten [%1/l]").arg(QLocale().currencySymbol()));
    model->setHeaderData(model->fieldIndex("erg_Alkohol"), Qt::Horizontal, tr("Alk. [%]"));
    model->setHeaderData(model->fieldIndex("sEVG"), Qt::Horizontal, tr("sEVG [%]"));
    model->setHeaderData(model->fieldIndex("tEVG"), Qt::Horizontal, tr("tEVG [%]"));
    model->setHeaderData(model->fieldIndex("erg_EffektiveAusbeute"), Qt::Horizontal, tr("Eff. SHA [%]"));
    model->setHeaderData(model->fieldIndex("Verdampfungsziffer"), Qt::Horizontal, tr("Verdampfungsziffer [%]"));
    model->setHeaderData(model->fieldIndex("AusbeuteIgnorieren"), Qt::Horizontal, tr("Für Durchschnitt Ignorieren"));

    QTableView *table = ui->tableSudauswahl;
    QHeaderView *header = table->horizontalHeader();
    ProxyModelSudColored *proxyModel = new ProxyModelSudColored(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    mSpalten.append({"Sudname", true, false, 200, nullptr});
    mSpalten.append({"Sudnummer", true, true, 80, new SpinBoxDelegate(table)});
    mSpalten.append({"Braudatum", true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({"Abfuelldatum", false, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({"Erstellt", true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({"Gespeichert", true, true, 100, new DateDelegate(false, true, table)});
    mSpalten.append({"Woche", true, true, 80, nullptr});
    mSpalten.append({"BewertungMittel", true, true, 80, new RatingDelegate(table)});
    mSpalten.append({"Menge", false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    mSpalten.append({"SW", false, true, 80, new DoubleSpinBoxDelegate(1, table)});
    mSpalten.append({"IBU", false, true, 80, new SpinBoxDelegate(table)});

    int visualIndex = 0;
    for (const AuswahlSpalten& spalte : mSpalten)
    {
        int col = model->fieldIndex(spalte.fieldName);
        table->setColumnHidden(col, !spalte.visible);
        if (spalte.itemDelegate)
            table->setItemDelegateForColumn(col, spalte.itemDelegate);
        header->resizeSection(col, spalte.width);
        header->moveSection(header->visualIndex(col), visualIndex++);
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

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()));
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
        case Qt::Key::Key_Return:
            on_btnLaden_clicked();
            break;
        }
    }
}

void TabSudAuswahl::on_tableSudauswahl_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int sudId = model->data(index.row(), "ID").toInt();
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

    if (!ui->tableSudauswahl->horizontalHeader()->rect().contains(pos))
    {
        QModelIndex index = ui->tableSudauswahl->indexAt(pos);

        if (model->data(index.row(), "MerklistenID").toBool())
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

        int status = model->data(index.row(), "Status").toInt();
        if (status >= Sud_Status_Abgefuellt)
        {
            if (status == Sud_Status_Verbraucht)
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
    }

    for (const AuswahlSpalten& spalte : mSpalten)
    {
        if (spalte.canHide)
        {
            int col = model->fieldIndex(spalte.fieldName);
            action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
            action->setCheckable(true);
            action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
            action->setData(col);
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
    ui->cbAlle->setChecked(filter == ProxyModelSud::Alle);
}

void TabSudAuswahl::on_cbRezept_clicked()
{
    setFilterStatus();
}

void TabSudAuswahl::on_cbAlle_clicked(bool checked)
{
    if (checked)
    {
        ui->cbRezept->setChecked(true);
        ui->cbGebraut->setChecked(true);
        ui->cbAbgefuellt->setChecked(true);
        ui->cbVerbraucht->setChecked(true);
        setFilterStatus();
    }
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
        model->setFilterDateColumn(bh->modelSud()->fieldIndex("Braudatum"));
    }
    ui->tbDatumVon->setEnabled(!state);
    ui->tbDatumBis->setEnabled(!state);
}

void TabSudAuswahl::on_btnMerken_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int col = model->fieldIndex("MerklistenID");
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), col);
        if (!model->data(indexMerkliste).toBool())
            model->setData(indexMerkliste, true);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::on_btnVergessen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int col = model->fieldIndex("MerklistenID");
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexMerkliste = index.sibling(index.row(), col);
        if (model->data(indexMerkliste).toBool())
            model->setData(indexMerkliste, false);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::on_btnAlleVergessen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int col = model->fieldIndex("MerklistenID");
    for (int row = 0; row < model->rowCount(); ++row)
    {
        QModelIndex index = model->index(row, col);
        if (model->data(index).toBool())
            model->setData(index, false);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::onVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int col = model->fieldIndex("Status");
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), col);
        if (model->data(indexStatus).toInt() == Sud_Status_Abgefuellt)
            model->setData(indexStatus, Sud_Status_Verbraucht);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::onNichtVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int col = model->fieldIndex("Status");
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QModelIndex indexStatus = index.sibling(index.row(), col);
        if (model->data(indexStatus).toInt() == Sud_Status_Verbraucht)
            model->setData(indexStatus, Sud_Status_Abgefuellt);
    }
    ui->tableSudauswahl->setFocus();
}

void TabSudAuswahl::on_btnAnlegen_clicked()
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

    QVariantMap values({{"Sudname", tr("Neuer Sud")}});
    int row = model->append(values);
    if (row >= 0)
    {
        filterChanged();
        ui->tableSudauswahl->setCurrentIndex(model->index(row, model->fieldIndex("Sudname")));
        ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
        ui->tableSudauswahl->edit(ui->tableSudauswahl->currentIndex());
    }
}

void TabSudAuswahl::on_btnKopieren_clicked()
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

    int row = -1;
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        int sudId = model->data(index.row(), "ID").toInt();
        QString name = model->data(index.row(), "Sudname").toString() + " " + tr("Kopie");
        row = bh->sudKopieren(sudId, name);
        row = model->mapRowFromSource(row);
    }
    if (row >= 0)
    {
        filterChanged();
        ui->tableSudauswahl->setCurrentIndex(model->index(row, model->fieldIndex("Sudname")));
        ui->tableSudauswahl->scrollTo(ui->tableSudauswahl->currentIndex());
        ui->tableSudauswahl->edit(ui->tableSudauswahl->currentIndex());
    }
}

void TabSudAuswahl::on_btnLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    QList<int> sudIds;
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
        sudIds.append(model->data(index.row(), "ID").toInt());
    for (int sudId : sudIds)
    {
        int row = model->getRowWithValue("ID", sudId);
        if (row >= 0)
        {
            QString name = model->data(row, "Sudname").toString();
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

void TabSudAuswahl::on_btnImportieren_clicked()
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
                ui->tableSudauswahl->setCurrentIndex(model->index(sudRow, model->fieldIndex("Sudname")));
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

void TabSudAuswahl::on_btnExportieren_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    int colSudName = model->fieldIndex("Sudname");
    gSettings->beginGroup("General");
    QString path = gSettings->value("exportPath", QDir::homePath()).toString();
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QString sudname = index.sibling(index.row(), colSudName).data().toString();
                model->data(index.row(), "Sudname").toString() + " " + tr("Kopie");
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
    gSettings->endGroup();
}

void TabSudAuswahl::on_btnLaden_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() > 0)
    {
        int sudId = model->data(selection[0].row(), "ID").toInt();
        clicked(sudId);
    }
}

void TabSudAuswahl::on_btnToPdf_clicked()
{
    QModelIndexList selection = ui->tableSudauswahl->selectionModel()->selectedRows();
    if (selection.count() == 0)
        return;

    gSettings->beginGroup("General");

    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString fileName;
    if (selection.count() == 1)
        fileName = static_cast<ProxyModel*>(ui->tableSudauswahl->model())->data(selection[0].row(), "Sudname").toString() + "_" + tr("Rohstoffe");
    else
        fileName = tr("Rohstoffe");

    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + fileName +  ".pdf", "PDF (*.pdf)");
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        ui->webview->printToPdf(filePath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    gSettings->endGroup();
}
