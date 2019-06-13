#include "tabsudauswahl.h"
#include "ui_tabsudauswahl.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
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
    QWidget(parent),
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
    model->setHeaderData(model->fieldIndex("Erstellt"), Qt::Horizontal, tr("Erstellt"));
    model->setHeaderData(model->fieldIndex("Gespeichert"), Qt::Horizontal, tr("Gespeichert"));
    model->setHeaderData(model->fieldIndex("Woche"), Qt::Horizontal, tr("Woche"));
    model->setHeaderData(model->fieldIndex("BewertungMax"), Qt::Horizontal, tr("Bewertung"));
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

    int col;
    QTableView *table = ui->tableSudauswahl;
    QHeaderView *header = table->horizontalHeader();
    ProxyModelSudColored *proxyModel = new ProxyModelSudColored(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Sudname");
    table->setColumnHidden(col, false);
    header->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Sudnummer");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Braudatum");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Erstellt");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, true, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("Gespeichert");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, true, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("Woche");
    table->setColumnHidden(col, false);
    header->moveSection(header->visualIndex(col), 5);

    col = model->fieldIndex("BewertungMax");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new RatingDelegate(table));
    header->moveSection(header->visualIndex(col), 6);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableSudauswahl_customContextMenuRequested(const QPoint&)));

    gSettings->beginGroup("TabSudAuswahl");

    mDefaultTableState = header->saveState();
    header->restoreState(gSettings->value("tableSudAuswahlState").toByteArray());

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ProxyModelSud::FilterStatus filterStatus = static_cast<ProxyModelSud::FilterStatus>(gSettings->value("filterStatus", ProxyModelSud::FilterStatus::Alle).toInt());
    proxyModel->setFilterStatus(filterStatus);
    ui->rbAlle->setChecked(filterStatus == ProxyModelSud::Alle);
    ui->rbNichtGebraut->setChecked(filterStatus == ProxyModelSud::NichtGebraut);
    ui->rbNichtAbgefuellt->setChecked(filterStatus == ProxyModelSud::GebrautNichtAbgefuellt);
    ui->rbNichtVerbraucht->setChecked(filterStatus == ProxyModelSud::NichtVerbraucht);
    ui->rbAbgefuellt->setChecked(filterStatus == ProxyModelSud::Abgefuellt);
    ui->rbVerbraucht->setChecked(filterStatus == ProxyModelSud::Verbraucht);
    proxyModel->setFilterStatus(filterStatus);

    ui->cbMerkliste->setChecked(gSettings->value("filterMerkliste", false).toBool());

    ui->tbDatumVon->setDate(gSettings->value("ZeitraumVon", QDate::currentDate().addYears(-1)).toDate());
    ui->tbDatumBis->setDate(gSettings->value("ZeitraumBis", QDate::currentDate()).toDate());
    ui->cbDatumAlle->setChecked(gSettings->value("ZeitraumAlle", true).toBool());

    gSettings->endGroup();

    connect(bh, SIGNAL(modified()), this, SLOT(databaseModified()));
    connect(proxyModel, SIGNAL(layoutChanged()), this, SLOT(filterChanged()));
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
    gSettings->setValue("filterStatus", static_cast<ProxyModelSud*>(ui->tableSudauswahl->model())->filterStatus());
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

void TabSudAuswahl::databaseModified()
{
    updateTemplateTags();
}

void TabSudAuswahl::filterChanged()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    ui->lblNumSude->setText(QString::number(model->rowCount()) + " / " + QString::number(bh->modelSud()->rowCount()));
}

void TabSudAuswahl::selectionChanged()
{
    bool selected = ui->tableSudauswahl->selectionModel()->selectedRows().count() > 0;
    updateTemplateTags();
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
    int col;
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

    action = new QAction(tr("Sudnummer"), &menu);
    col = model->fieldIndex("Sudnummer");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    action = new QAction(tr("Braudatum"), &menu);
    col = model->fieldIndex("Braudatum");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    action = new QAction(tr("Erstellt"), &menu);
    col = model->fieldIndex("Erstellt");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    action = new QAction(tr("Gespeichert"), &menu);
    col = model->fieldIndex("Gespeichert");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    action = new QAction(tr("Woche"), &menu);
    col = model->fieldIndex("Woche");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    action = new QAction(tr("Bewertung"), &menu);
    col = model->fieldIndex("BewertungMax");
    action->setCheckable(true);
    action->setChecked(!ui->tableSudauswahl->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    menu.exec(ui->tableSudauswahl->viewport()->mapToGlobal(pos));
}

void TabSudAuswahl::on_rbAlle_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::Alle);
}

void TabSudAuswahl::on_rbNichtGebraut_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::NichtGebraut);
}

void TabSudAuswahl::on_rbNichtAbgefuellt_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::GebrautNichtAbgefuellt);
}

void TabSudAuswahl::on_rbNichtVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::NichtVerbraucht);
}

void TabSudAuswahl::on_rbAbgefuellt_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::Abgefuellt);
}

void TabSudAuswahl::on_rbVerbraucht_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableSudauswahl->model());
    model->setFilterStatus(ProxyModelSud::Verbraucht);
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
    if (!ui->rbNichtGebraut->isChecked() && !ui->rbAlle->isChecked())
    {
        ui->rbAlle->setChecked(true);
        model->setFilterStatus(ProxyModelSud::Alle);
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
    if (!ui->rbNichtGebraut->isChecked() && !ui->rbAlle->isChecked())
    {
        ui->rbAlle->setChecked(true);
        model->setFilterStatus(ProxyModelSud::Alle);
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
    // TODO: implement function
    QMessageBox::warning(this, tr("Sud importieren"), tr("Diese Funktion ist noch nicht implementiert."));
    filterChanged();
}

void TabSudAuswahl::on_btnExportieren_clicked()
{
    // TODO: implement function
    for (const QModelIndex &index : ui->tableSudauswahl->selectionModel()->selectedRows())
    {
        QMessageBox::warning(this, tr("Sud exportieren"), tr("Diese Funktion ist noch nicht implementiert."));
    }
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
