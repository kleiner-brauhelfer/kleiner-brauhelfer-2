#include "tabausruestung.h"
#include "ui_tabausruestung.h"
#include <QMenu>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodel.h"
#include "model/proxymodelsudcolored.h"
#include "model/readonlydelegate.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/datedelegate.h"
#include "dialogs/dlgverdampfung.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabAusruestung::TabAusruestung(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabAusruestung)
{
    ui->setupUi(this);
    ui->lblCurrency->setText(QLocale().currencySymbol());

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    int col;
    QTableView *table = ui->tableViewAnlagen;
    QHeaderView *header = table->horizontalHeader();
    ProxyModel *model = new ProxyModel(this);
    model->setSourceModel(bh->modelAusruestung());
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Name");
    model->setHeaderData(col, Qt::Horizontal, tr("Anlage"));
    table->setColumnHidden(col, false);
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Vermoegen");
    model->setHeaderData(col, Qt::Horizontal, tr("Vermögen [l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("AnzahlSude");
    model->setHeaderData(col, Qt::Horizontal, tr("Anzahl Sude"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    model = new ProxyModel(this);
    model->setSourceModel(bh->modelGeraete());
    model->setFilterKeyColumn(bh->modelGeraete()->fieldIndex("AusruestungAnlagenID"));
    ui->listViewGeraete->setModel(model);
    ui->listViewGeraete->setModelColumn(bh->modelGeraete()->fieldIndex("Bezeichnung"));

    table = ui->tableViewSude;
    header = table->horizontalHeader();
    model = new ProxyModelSudColored(this);
    model->setSourceModel(bh->modelSud());
    model->setFilterKeyColumn(bh->modelSud()->fieldIndex("AuswahlBrauanlage"));
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Sudname");
    table->setColumnHidden(col, false);
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    table->setItemDelegateForColumn(col, new ReadonlyDelegate(table));
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Sudnummer");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 50);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Braudatum");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("erg_EffektiveAusbeute");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("Verdampfungsziffer");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("AusbeuteIgnorieren");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 5);

    gSettings->beginGroup("TabAusruestung");

    mDefaultTableState = header->saveState();
    header->restoreState(gSettings->value("tableState").toByteArray());

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    mDefaultSplitterLeftState = ui->splitterLeft->saveState();
    ui->splitterLeft->restoreState(gSettings->value("splitterLeftState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->modelSud(), SIGNAL(modified()), this, SLOT(updateDurchschnitt()));
    connect(bh->modelAusruestung(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(ui->tableViewAnlagen->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(anlage_selectionChanged(const QItemSelection&)));

    sudLoaded();
}

TabAusruestung::~TabAusruestung()
{
    delete ui;
}

void TabAusruestung::saveSettings()
{
    gSettings->beginGroup("TabAusruestung");
    gSettings->setValue("tableState", ui->tableViewSude->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterLeftState", ui->splitterLeft->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->endGroup();
}

void TabAusruestung::restoreView()
{
    ui->tableViewSude->horizontalHeader()->restoreState(mDefaultTableState);
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterLeft->restoreState(mDefaultSplitterLeftState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
}

void TabAusruestung::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabAusruestung::sudLoaded()
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    int row = 0;
    if (bh->sud()->isLoaded())
        row = model->getRowWithValue("Name", bh->sud()->getAuswahlBrauanlageName());
    ui->tableViewAnlagen->setCurrentIndex(model->index(row, model->fieldIndex("Name")));
}

void TabAusruestung::anlage_selectionChanged(const QItemSelection &selected)
{
    QRegExp regExpId;
    if (selected.indexes().count() > 0)
    {
        mRow = selected.indexes()[0].row();
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
        QString id = model->data(model->index(mRow, model->fieldIndex("AnlagenID"))).toString();
        regExpId = QRegExp(QString("^%1$").arg(id), Qt::CaseInsensitive, QRegExp::RegExp);
    }
    else
    {
        regExpId = QRegExp(QString("--dummy--"), Qt::CaseInsensitive, QRegExp::RegExp);
    }
    static_cast<QSortFilterProxyModel*>(ui->listViewGeraete->model())->setFilterRegExp(regExpId);
    static_cast<QSortFilterProxyModel*>(ui->tableViewSude->model())->setFilterRegExp(regExpId);
    ui->sliderAusbeuteSude->setMaximum(9999);
    ui->sliderAusbeuteSude->setValue(9999);
    updateValues();
    updateDurchschnitt();
}

void TabAusruestung::on_btnNeueAnlage_clicked()
{
    QVariantMap values({{"Name", tr("Neue Brauanlage")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, model->fieldIndex("Name"));
        ui->tableViewAnlagen->setCurrentIndex(index);
        ui->tableViewAnlagen->scrollTo(ui->tableViewAnlagen->currentIndex());
        ui->tableViewAnlagen->edit(ui->tableViewAnlagen->currentIndex());
    }
}

void TabAusruestung::on_btnAnlageLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableViewAnlagen->model());
    for (const QModelIndex &index : ui->tableViewAnlagen->selectionModel()->selectedRows())
    {
        QString name = model->data(index.row(), "Name").toString();
        int ret = QMessageBox::question(this, tr("Brauanlage löschen?"),
                                        tr("Soll die Brauanlage \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
            int row = model->getRowWithValue("Name", name);
            model->removeRow(row);
        }
    }
}

void TabAusruestung::on_btnNeuesGeraet_clicked()
{
    QModelIndexList selected = ui->tableViewAnlagen->selectionModel()->selectedRows();
    if (selected.count() > 0)
    {
        QVariant id = bh->modelAusruestung()->getValueFromSameRow("Name", data("Name"), "AnlagenID");
        QVariantMap values({{"AusruestungAnlagenID", id}, {"Bezeichnung", tr("Neues Gerät")}});
        ProxyModel *model = static_cast<ProxyModel*>(ui->listViewGeraete->model());
        int row = model->append(values);
        if (row >= 0)
        {
            const QModelIndex index = model->index(row, model->fieldIndex("Bezeichnung"));
            ui->listViewGeraete->setCurrentIndex(index);
            ui->listViewGeraete->scrollTo(ui->listViewGeraete->currentIndex());
            ui->listViewGeraete->edit(ui->listViewGeraete->currentIndex());
        }
    }
}

void TabAusruestung::on_btnGeraetLoeschen_clicked()
{
    for (const QModelIndex &index : ui->listViewGeraete->selectionModel()->selectedIndexes())
    {
        QString name = index.data().toString();
        int ret = QMessageBox::question(this, tr("Gerät löschen?"),
                                        tr("Soll das Gerät \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->listViewGeraete->model());
            int row = model->getRowWithValue("Bezeichnung", name);
            model->removeRow(row);
        }
    }
}

QVariant TabAusruestung::data(const QString &fieldName) const
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->data(mRow, fieldName);
}

bool TabAusruestung::setData(const QString &fieldName, const QVariant &value)
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->setData(mRow, fieldName, value);
}

void TabAusruestung::updateValues()
{
    if (!ui->tbAusbeute->hasFocus())
        ui->tbAusbeute->setValue(data("Sudhausausbeute").toDouble());
    if (!ui->tbVerdampfung->hasFocus())
        ui->tbVerdampfung->setValue(data("Verdampfungsziffer").toDouble());
    if (!ui->tbNachguss->hasFocus())
        ui->tbNachguss->setValue(data("KorrekturWasser").toDouble());
    if (!ui->tbFarbe->hasFocus())
        ui->tbFarbe->setValue(data("KorrekturFarbe").toInt());
    if (!ui->tbKosten->hasFocus())
        ui->tbKosten->setValue(data("Kosten").toDouble());
    if (!ui->tbMaischebottichHoehe->hasFocus())
        ui->tbMaischebottichHoehe->setValue(data("Maischebottich_Hoehe").toDouble());
    if (!ui->tbMaischebottichDurchmesser->hasFocus())
        ui->tbMaischebottichDurchmesser->setValue(data("Maischebottich_Durchmesser").toDouble());
    if (!ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        ui->tbMaischebottichMaxFuellhoehe->setValue(data("Maischebottich_MaxFuellhoehe").toDouble());
    if (!ui->tbSudpfanneHoehe->hasFocus())
        ui->tbSudpfanneHoehe->setValue(data("Sudpfanne_Hoehe").toDouble());
    if (!ui->tbSudpfanneDurchmesser->hasFocus())
        ui->tbSudpfanneDurchmesser->setValue(data("Sudpfanne_Durchmesser").toDouble());
    if (!ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        ui->tbSudpfanneMaxFuellhoehe->setValue(data("Sudpfanne_MaxFuellhoehe").toDouble());
    ui->tbMaischenVolumen->setValue(data("Maischebottich_Volumen").toDouble());
    ui->tbMaischenMaxNutzvolumen->setValue(data("Maischebottich_MaxFuellvolumen").toDouble());
    ui->tbSudpfanneVolumen->setValue(data("Sudpfanne_Volumen").toDouble());
    ui->tbSudpfanneMaxNutzvolumen->setValue(data("Sudpfanne_MaxFuellvolumen").toDouble()); 
}

void TabAusruestung::updateDurchschnitt()
{
    ProxyModelSud model;
    model.setSourceModel(bh->modelSud());
    model.setFilterStatus(ProxyModelSud::Gebraut);
    model.sort(model.fieldIndex("Braudatum"), Qt::DescendingOrder);
    QString anlage = data("Name").toString();
    int colName = model.fieldIndex("AuswahlBrauanlageName");
    int colAusbeute = model.fieldIndex("erg_EffektiveAusbeute");
    int colVerdampfung = model.fieldIndex("Verdampfungsziffer");
    int colAusbeuteIgnorieren = model.fieldIndex("AusbeuteIgnorieren");
    int n = 0;
    int N = 0;
    double ausbeute = 0.0;
    double verdampfung = 0.0;
    for (int i = 0; i < model.rowCount(); ++i)
    {
        if (model.index(i, colName).data().toString() == anlage)
        {
            if (!model.index(i, colAusbeuteIgnorieren).data().toBool())
            {
                if (n < ui->sliderAusbeuteSude->value())
                {
                    ausbeute += model.index(i, colAusbeute).data().toDouble();
                    verdampfung += model.index(i, colVerdampfung).data().toDouble();
                    ++n;
                }
                ++N;
            }
        }
    }
    if (n > 0)
    {
        ausbeute /= n;
        verdampfung /= n;
    }
    ui->sliderAusbeuteSude->setMaximum(N);
    ui->tbAusbeuteMittel->setValue(ausbeute);
    ui->tbVerdampfungMittel->setValue(verdampfung);
    ui->tbAusbeuteSude->setValue(ui->sliderAusbeuteSude->value());
}

void TabAusruestung::on_btnVerdampfungsziffer_clicked()
{
    DlgVerdampfung dlg;
    dlg.setDurchmesser(data("Sudpfanne_Durchmesser").toDouble());
    dlg.setHoehe(data("Sudpfanne_Hoehe").toDouble());
    if (bh->sud()->isLoaded())
    {
        dlg.setKochdauer(bh->sud()->getKochdauerNachBitterhopfung());
        dlg.setMenge1(BierCalc::volumenWasser(20, 100, bh->sud()->getWuerzemengeVorHopfenseihen()));
        dlg.setMenge2(BierCalc::volumenWasser(20, 100, bh->sud()->getWuerzemengeKochende()));
    }
    else
    {
        dlg.setKochdauer(90);
        dlg.setMenge1(20);
        dlg.setMenge2(18);
    }
    if (dlg.exec() == QDialog::Accepted)
    {
        ui->tbVerdampfung->setValue(dlg.getVerdampfungsziffer());
    }
}

void TabAusruestung::on_tbAusbeute_valueChanged(double value)
{
    if (ui->tbAusbeute->hasFocus())
        setData("Sudhausausbeute", value);
}

void TabAusruestung::on_btnAusbeuteMittel_clicked()
{
    setData("Sudhausausbeute", ui->tbAusbeuteMittel->value());
}

void TabAusruestung::on_tbVerdampfung_valueChanged(double value)
{
    if (ui->tbVerdampfung->hasFocus())
        setData("Verdampfungsziffer", value);
}

void TabAusruestung::on_btnVerdampfungMittel_clicked()
{
    setData("Verdampfungsziffer", ui->tbVerdampfungMittel->value());
}

void TabAusruestung::on_sliderAusbeuteSude_valueChanged(int)
{
    updateDurchschnitt();
}

void TabAusruestung::on_tbNachguss_valueChanged(double value)
{
    if (ui->tbNachguss->hasFocus())
        setData("KorrekturWasser", value);
}

void TabAusruestung::on_tbFarbe_valueChanged(int value)
{
    if (ui->tbFarbe->hasFocus())
        setData("KorrekturFarbe", value);
}

void TabAusruestung::on_tbKosten_valueChanged(double value)
{
    if (ui->tbKosten->hasFocus())
        setData("Kosten", value);
}

void TabAusruestung::on_tbMaischebottichHoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichHoehe->hasFocus())
        setData("Maischebottich_Hoehe", value);
}

void TabAusruestung::on_tbMaischebottichDurchmesser_valueChanged(double value)
{
    if (ui->tbMaischebottichDurchmesser->hasFocus())
        setData("Maischebottich_Durchmesser", value);
}

void TabAusruestung::on_tbMaischebottichMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        setData("Maischebottich_MaxFuellhoehe", value);
}

void TabAusruestung::on_tbSudpfanneHoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneHoehe->hasFocus())
        setData("Sudpfanne_Hoehe", value);
}

void TabAusruestung::on_tbSudpfanneDurchmesser_valueChanged(double value)
{
    if (ui->tbSudpfanneDurchmesser->hasFocus())
        setData("Sudpfanne_Durchmesser", value);
}

void TabAusruestung::on_tbSudpfanneMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        setData("Sudpfanne_MaxFuellhoehe", value);
}

void TabAusruestung::spalteAnzeigen(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableViewSude->setColumnHidden(action->data().toInt(), !checked);
}

void TabAusruestung::on_tableViewSude_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableViewSude;
    ProxyModel *model = static_cast<ProxyModel*>(table->model());

    col = model->fieldIndex("Sudnummer");
    action = new QAction(tr("Sudnummer"), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}
