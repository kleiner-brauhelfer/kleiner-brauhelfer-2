#include "tabausruestung.h"
#include "ui_tabausruestung.h"
#include <QKeyEvent>
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
    TabAbstract(parent),
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

    col = ModelAusruestung::ColName;
    model->setHeaderData(col, Qt::Horizontal, tr("Anlage"));
    table->setColumnHidden(col, false);
    header->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelAusruestung::ColVermoegen;
    model->setHeaderData(col, Qt::Horizontal, tr("Vermögen [l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelAusruestung::ColAnzahlSude;
    model->setHeaderData(col, Qt::Horizontal, tr("Anzahl Sude"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    table = ui->tableViewGeraete;
    header = table->horizontalHeader();
    model = new ProxyModel(this);
    model->setSourceModel(bh->modelGeraete());
    model->setFilterKeyColumn(ModelGeraete::ColAusruestungAnlagenID);
    table->setModel(model);
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = ModelGeraete::ColBezeichnung;
    table->setColumnHidden(col, false);
    header->setStretchLastSection(true);

    table = ui->tableViewSude;
    header = table->horizontalHeader();
    model = new ProxyModelSudColored(this);
    model->setSourceModel(bh->modelSud());
    model->setFilterKeyColumn(ModelSud::ColAnlage);
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableViewSude_customContextMenuRequested(const QPoint&)));

    col = ModelSud::ColSudname;
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new ReadonlyDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelSud::ColSudnummer;
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelSud::ColBraudatum;
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelSud::Colerg_EffektiveAusbeute;
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelSud::ColVerdampfungsrateIst;
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, table));
    header->resizeSection(col, 150);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelSud::ColAusbeuteIgnorieren;
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
            this, SLOT(anlage_selectionChanged()));

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

void TabAusruestung::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (ui->tableViewAnlagen->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnAnlageLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnNeueAnlage_clicked();
            break;
        }
    }
    else if (ui->tableViewGeraete->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnGeraetLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnNeuesGeraet_clicked();
            break;
        }
    }
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
        row = model->getRowWithValue(ModelAusruestung::ColName, bh->sud()->getAnlage());
    ui->tableViewAnlagen->setCurrentIndex(model->index(row, ModelAusruestung::ColName));
}

void TabAusruestung::anlage_selectionChanged()
{
    QRegExp regExpId;
    QRegExp regExpId2;
    QModelIndexList selectedRows = ui->tableViewAnlagen->selectionModel()->selectedRows();
    if (selectedRows.count() > 0)
    {
        mRow = selectedRows[0].row();
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
        QString anlage = model->data(model->index(mRow, ModelAusruestung::ColName)).toString();
        regExpId = QRegExp(QString("^%1$").arg(anlage), Qt::CaseInsensitive, QRegExp::RegExp);
        regExpId2 = QRegExp(QString("^%1$").arg(model->data(model->index(mRow, ModelAusruestung::ColID)).toInt()), Qt::CaseInsensitive, QRegExp::RegExp);
    }
    else
    {
        regExpId = QRegExp(QString("--dummy--"), Qt::CaseInsensitive, QRegExp::RegExp);
        regExpId2 = QRegExp(QString("--dummy--"), Qt::CaseInsensitive, QRegExp::RegExp);
    }
    static_cast<QSortFilterProxyModel*>(ui->tableViewGeraete->model())->setFilterRegExp(regExpId2);
    static_cast<QSortFilterProxyModel*>(ui->tableViewSude->model())->setFilterRegExp(regExpId);
    ui->sliderAusbeuteSude->setMaximum(9999);
    ui->sliderAusbeuteSude->setValue(9999);
    updateValues();
    updateDurchschnitt();
}

void TabAusruestung::on_btnNeueAnlage_clicked()
{
    QMap<int, QVariant> values({{ModelAusruestung::ColName, tr("Neue Brauanlage")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, ModelAusruestung::ColName);
        ui->tableViewAnlagen->setCurrentIndex(index);
        ui->tableViewAnlagen->scrollTo(ui->tableViewAnlagen->currentIndex());
        ui->tableViewAnlagen->edit(ui->tableViewAnlagen->currentIndex());
    }
}

void TabAusruestung::on_btnAnlageLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableViewAnlagen->model());
    QModelIndexList indices = ui->tableViewAnlagen->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        QString name = model->data(index.row(), ModelAusruestung::ColName).toString();
        int ret = QMessageBox::question(this, tr("Brauanlage löschen?"),
                                        tr("Soll die Brauanlage \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
            int row = model->getRowWithValue(ModelAusruestung::ColName, name);
            model->removeRow(row);
        }
    }
}

void TabAusruestung::on_btnNeuesGeraet_clicked()
{
    QModelIndexList selected = ui->tableViewAnlagen->selectionModel()->selectedRows();
    if (selected.count() > 0)
    {
        QMap<int, QVariant> values({{ModelGeraete::ColAusruestungAnlagenID, data(ModelAusruestung::ColID)},
                                    {ModelGeraete::ColBezeichnung, tr("Neues Gerät")}});
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewGeraete->model());
        int row = model->append(values);
        if (row >= 0)
        {
            const QModelIndex index = model->index(row, ModelGeraete::ColBezeichnung);
            ui->tableViewGeraete->setCurrentIndex(index);
            ui->tableViewGeraete->scrollTo(ui->tableViewGeraete->currentIndex());
            ui->tableViewGeraete->edit(ui->tableViewGeraete->currentIndex());
        }
    }
}

void TabAusruestung::on_btnGeraetLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableViewGeraete->model());
    QModelIndexList indices = ui->tableViewGeraete->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        QString name = model->data(index.row(), ModelGeraete::ColBezeichnung).toString();
        int ret = QMessageBox::question(this, tr("Gerät löschen?"),
                                        tr("Soll das Gerät \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            int row = model->getRowWithValue(ModelGeraete::ColBezeichnung, name);
            model->removeRow(row);
        }
    }
}

QVariant TabAusruestung::data(int col) const
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->data(mRow, col);
}

bool TabAusruestung::setData(int col, const QVariant &value)
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->setData(mRow, col, value);
}

void TabAusruestung::updateValues()
{
    ui->lblAnlage->setText(data(ModelAusruestung::ColName).toString());
    if (!ui->tbAusbeute->hasFocus())
        ui->tbAusbeute->setValue(data(ModelAusruestung::ColSudhausausbeute).toDouble());
    if (!ui->tbVerdampfung->hasFocus())
        ui->tbVerdampfung->setValue(data(ModelAusruestung::ColVerdampfungsziffer).toDouble());
    if (!ui->tbNachguss->hasFocus())
        ui->tbNachguss->setValue(data(ModelAusruestung::ColKorrekturWasser).toDouble());
    if (!ui->tbFarbe->hasFocus())
        ui->tbFarbe->setValue(data(ModelAusruestung::ColKorrekturFarbe).toInt());
    if (!ui->tbKosten->hasFocus())
        ui->tbKosten->setValue(data(ModelAusruestung::ColKosten).toDouble());
    if (!ui->tbMaischebottichHoehe->hasFocus())
        ui->tbMaischebottichHoehe->setValue(data(ModelAusruestung::ColMaischebottich_Hoehe).toDouble());
    if (!ui->tbMaischebottichDurchmesser->hasFocus())
        ui->tbMaischebottichDurchmesser->setValue(data(ModelAusruestung::ColMaischebottich_Durchmesser).toDouble());
    if (!ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        ui->tbMaischebottichMaxFuellhoehe->setValue(data(ModelAusruestung::ColMaischebottich_MaxFuellhoehe).toDouble());
    ui->tbMaischebottichMaxFuellhoehe->setMaximum(ui->tbMaischebottichHoehe->value());
    if (!ui->tbSudpfanneHoehe->hasFocus())
        ui->tbSudpfanneHoehe->setValue(data(ModelAusruestung::ColSudpfanne_Hoehe).toDouble());
    if (!ui->tbSudpfanneDurchmesser->hasFocus())
        ui->tbSudpfanneDurchmesser->setValue(data(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble());
    if (!ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        ui->tbSudpfanneMaxFuellhoehe->setValue(data(ModelAusruestung::ColSudpfanne_MaxFuellhoehe).toDouble());
    ui->tbSudpfanneMaxFuellhoehe->setMaximum(ui->tbSudpfanneHoehe->value());
    ui->tbMaischenVolumen->setValue(data(ModelAusruestung::ColMaischebottich_Volumen).toDouble());
    ui->tbMaischenMaxNutzvolumen->setValue(data(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
    ui->tbSudpfanneVolumen->setValue(data(ModelAusruestung::ColSudpfanne_Volumen).toDouble());
    ui->tbSudpfanneMaxNutzvolumen->setValue(data(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
}

void TabAusruestung::updateDurchschnitt()
{
    ProxyModelSud model;
    model.setSourceModel(bh->modelSud());
    model.setFilterStatus(ProxyModelSud::Gebraut | ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    model.sort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    QString anlage = data(ModelAusruestung::ColName).toString();
    int n = 0;
    int N = 0;
    double ausbeute = 0.0;
    double verdampfung = 0.0;
    for (int i = 0; i < model.rowCount(); ++i)
    {
        if (model.index(i, ModelSud::ColAnlage).data().toString() == anlage)
        {
            if (!model.index(i, ModelSud::ColAusbeuteIgnorieren).data().toBool())
            {
                if (n < ui->sliderAusbeuteSude->value())
                {
                    ausbeute += model.index(i, ModelSud::Colerg_EffektiveAusbeute).data().toDouble();
                    verdampfung += model.index(i, ModelSud::ColVerdampfungsrateIst).data().toDouble();
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
    dlg.setDurchmesser(data(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble());
    dlg.setHoehe(data(ModelAusruestung::ColSudpfanne_Hoehe).toDouble());
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
        setData(ModelAusruestung::ColSudhausausbeute, value);
}

void TabAusruestung::on_btnAusbeuteMittel_clicked()
{
    setData(ModelAusruestung::ColSudhausausbeute, ui->tbAusbeuteMittel->value());
}

void TabAusruestung::on_tbVerdampfung_valueChanged(double value)
{
    if (ui->tbVerdampfung->hasFocus())
        setData(ModelAusruestung::ColVerdampfungsziffer, value);
}

void TabAusruestung::on_btnVerdampfungMittel_clicked()
{
    setData(ModelAusruestung::ColVerdampfungsziffer, ui->tbVerdampfungMittel->value());
}

void TabAusruestung::on_sliderAusbeuteSude_valueChanged(int)
{
    updateDurchschnitt();
}

void TabAusruestung::on_tbNachguss_valueChanged(double value)
{
    if (ui->tbNachguss->hasFocus())
        setData(ModelAusruestung::ColKorrekturWasser, value);
}

void TabAusruestung::on_tbFarbe_valueChanged(int value)
{
    if (ui->tbFarbe->hasFocus())
        setData(ModelAusruestung::ColKorrekturFarbe, value);
}

void TabAusruestung::on_tbKosten_valueChanged(double value)
{
    if (ui->tbKosten->hasFocus())
        setData(ModelAusruestung::ColKosten, value);
}

void TabAusruestung::on_tbMaischebottichHoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichHoehe->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_Hoehe, value);
}

void TabAusruestung::on_tbMaischebottichDurchmesser_valueChanged(double value)
{
    if (ui->tbMaischebottichDurchmesser->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_Durchmesser, value);
}

void TabAusruestung::on_tbMaischebottichMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_MaxFuellhoehe, value);
}

void TabAusruestung::on_tbSudpfanneHoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneHoehe->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_Hoehe, value);
}

void TabAusruestung::on_tbSudpfanneDurchmesser_valueChanged(double value)
{
    if (ui->tbSudpfanneDurchmesser->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_Durchmesser, value);
}

void TabAusruestung::on_tbSudpfanneMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_MaxFuellhoehe, value);
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

    col = ModelSud::ColSudnummer;
    action = new QAction(tr("Sudnummer"), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigen(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}
