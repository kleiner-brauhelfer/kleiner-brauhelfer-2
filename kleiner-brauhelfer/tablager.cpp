#include "tablager.h"
#include "ui_tablager.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QDesktopServices>
#include "mainwindow.h"
#include "proxymodelrohstoff.h"
#include "model/textdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/ingredientnamedelegate.h"
#include "model/linklabeldelegate.h"
#include "model/spinboxdelegate.h"
#include "model/phmalzdelegate.h"
#include "dialogs/dlgrohstoffvorlage.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabLager::TabLager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabLager)
{
    ui->setupUi(this);

    ProxyModelRohstoff *proxyModel;
    TableView *table;

    table = ui->tableMalz;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelMalz());
    table->setModel(proxyModel);

    table = ui->tableHopfen;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelHopfen());
    table->setModel(proxyModel);

    table = ui->tableHefe;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelHefe());
    table->setModel(proxyModel);

    table = ui->tableWeitereZutaten;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(bh->modelWeitereZutaten());
    table->setModel(proxyModel);

    ui->btnFilter->setModel(new ProxyModelRohstoff(this));
    connect(ui->btnFilter->model(), &ProxyModelRohstoff::filterChanged, this, &TabLager::onFilterChanged);

    //modulesChanged(Settings::ModuleAlle);
    //connect(gSettings, &Settings::modulesChanged, this, &DlgRohstoffe::modulesChanged);
    build(); // called in modulesChanged()

    loadSettings();

    on_toolBoxRohstoffe_currentChanged();
}

TabLager::~TabLager()
{
    saveSettings();
    delete ui;
}

void TabLager::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableMalzState", ui->tableMalz->horizontalHeader()->saveState());
    gSettings->setValue("tableHopfenState", ui->tableHopfen->horizontalHeader()->saveState());
    gSettings->setValue("tableHefeState", ui->tableHefe->horizontalHeader()->saveState());
    gSettings->setValue("tableWeitereZutatenState", ui->tableWeitereZutaten->horizontalHeader()->saveState());
    gSettings->endGroup();
}

void TabLager::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableMalz->restoreState(gSettings->value("tableMalzState").toByteArray());
    ui->tableHopfen->restoreState(gSettings->value("tableHopfenState").toByteArray());
    ui->tableHefe->restoreState(gSettings->value("tableHefeState").toByteArray());
    ui->tableWeitereZutaten->restoreState(gSettings->value("tableWeitereZutatenState").toByteArray());
    gSettings->endGroup();
}

void TabLager::restoreView()
{
    ui->tableMalz->restoreDefaultState();
    ui->tableHopfen->restoreDefaultState();
    ui->tableHefe->restoreDefaultState();
    ui->tableWeitereZutaten->restoreDefaultState();
}

/*
void DlgRohstoffe::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleLagerverwaltung) ||
        modules.testFlag(Settings::ModuleWasseraufbereitung) ||
        modules.testFlag(Settings::ModulePreiskalkulation))
    {
        build();
    }
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        ui->radioButtonVorhanden->setVisible(gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung));
        ui->radioButtonAlle->setChecked(true);
        on_radioButtonAlle_clicked();
    }
}
*/

void TabLager::build()
{
    TableView *table;

    table = ui->tableMalz;
    table->clearCols();
    table->appendCol({ModelMalz::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    table->appendCol({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung))
    {
        table->appendCol({ModelMalz::ColpH, true, true, 100, new PhMalzDelegate(table)});
    }
    table->appendCol({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelMalz::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelMalz::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelMalz::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
    {
        table->appendCol({ModelMalz::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelMalz::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableHopfen;
    table->clearCols();
    table->appendCol({ModelHopfen::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHopfen::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, std::numeric_limits<double>::max(), 1, false, table)});
    }
    table->appendCol({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table)});
    table->appendCol({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
    table->appendCol({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::HopfenTypname, gSettings->HopfenTypBackgrounds, table)});
    table->appendCol({ModelHopfen::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHopfen::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHopfen::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
    {
        table->appendCol({ModelHopfen::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelHopfen::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableHefe;
    table->clearCols();
    table->appendCol({ModelHefe::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, false, table)});
    }
    table->appendCol({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypname, gSettings->HefeTypOgUgBackgrounds, table)});
    table->appendCol({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, table)});
    table->appendCol({ModelHefe::ColWuerzemenge, true, true, 100, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, table)});
    table->appendCol({ModelHefe::ColSedimentation, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColEVG, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColTemperatur, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHefe::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelHefe::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
    {
        table->appendCol({ModelHefe::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelHefe::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();

    table = ui->tableWeitereZutaten;
    table->clearCols();
    table->appendCol({ModelWeitereZutaten::ColName, true, false, 200, new IngredientNameDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    table->appendCol({ModelWeitereZutaten::ColEinheit, true, false, 100, new ComboBoxDelegate(MainWindow::Einheiten, table)});
    table->appendCol({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::ZusatzTypname, gSettings->WZTypBackgrounds, table)});
    table->appendCol({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColUnvergaerbar, true, true, 100, new CheckBoxDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColBemerkung, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColEigenschaften, true, true, 200, new TextDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColAlternativen, true, true, 200, new TextDelegate(table)});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
    {
        table->appendCol({ModelWeitereZutaten::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    }
    if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
    {
        table->appendCol({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
        table->appendCol({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    }
    table->appendCol({ModelWeitereZutaten::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
}

QTableView* TabLager::currentTable() const
{
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        return ui->tableMalz;
    case 1:
        return ui->tableHopfen;
    case 2:
        return ui->tableHefe;
    case 3:
        return ui->tableWeitereZutaten;
    default:
        return nullptr;
    }
}

void TabLager::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (currentTable()->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            remove();
            break;
        case Qt::Key::Key_Insert:
            add();
            break;
        }
    }
}

void TabLager::on_tableMalz_clicked(const QModelIndex &index)
{
    if (index.column() == ModelMalz::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabLager::on_tableHopfen_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHopfen::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabLager::on_tableHefe_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHefe::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabLager::on_tableWeitereZutaten_clicked(const QModelIndex &index)
{
    if (index.column() == ModelWeitereZutaten::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabLager::addEntry(QTableView *table, const QMap<int, QVariant> &values)
{
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    ui->btnFilter->model()->setFilter(ProxyModelRohstoff::Alle);
    ui->tbFilter->clear();
    int row = model->append(values);
    if (row >= 0)
    {
        table->setCurrentIndex(model->index(row, model->fieldIndex(QStringLiteral("Name"))));
        table->scrollTo(table->currentIndex());
        table->edit(table->currentIndex());
        updateLabelNumItems();
    }
}

void TabLager::add()
{
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        addEntry(ui->tableMalz, {{ModelMalz::ColName, tr("Neuer Eintrag")}});
        break;
    case 1:
        addEntry(ui->tableHopfen, {{ModelHopfen::ColName, tr("Neuer Eintrag")}});
        break;
    case 2:
        addEntry(ui->tableHefe, {{ModelHefe::ColName, tr("Neuer Eintrag")}});
        break;
    case 3:
        addEntry(ui->tableWeitereZutaten, {{ModelWeitereZutaten::ColName, tr("Neuer Eintrag")}});
        break;
    }
}

void TabLager::addVorlage()
{
    QTableView *table = currentTable();
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        art = DlgRohstoffVorlage::Art::Malz;
        break;
    case 1:
        art = DlgRohstoffVorlage::Art::Hopfen;
        break;
    case 2:
        art = DlgRohstoffVorlage::Art::Hefe;
        break;
    case 3:
        art = DlgRohstoffVorlage::Art::WZutaten;
        break;
    default:
        return;
    }
    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void TabLager::addObrama()
{
    QTableView *table = currentTable();
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        art = DlgRohstoffVorlage::Art::MalzOBraMa;
        break;
    case 1:
        art = DlgRohstoffVorlage::Art::HopfenOBraMa;
        break;
    case 2:
        art = DlgRohstoffVorlage::Art::HefeOBraMa;
        break;
    case 3:
        art = DlgRohstoffVorlage::Art::WZutatenOBraMa;
        break;
    default:
        return;
    }

    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void TabLager::copy()
{
    QTableView *table = currentTable();
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    SqlTableModel *sourceModel = static_cast<SqlTableModel*>(model->sourceModel());
    for (const QModelIndex& index : table->selectionModel()->selectedRows())
    {
        int row = index.row();
        QMap<int, QVariant> values = sourceModel->copyValues(model->mapRowToSource(row));
        values.insert(model->fieldIndex(QStringLiteral("Name")), model->data(row, model->fieldIndex(QStringLiteral("Name"))).toString() + " " + tr("Kopie"));
        values.remove(model->fieldIndex(QStringLiteral("Menge")));
        values.remove(model->fieldIndex(QStringLiteral("Eingelagert")));
        values.remove(model->fieldIndex(QStringLiteral("Mindesthaltbar")));
        addEntry(table, values);
    }
}

void TabLager::remove()
{
    QTableView *table = currentTable();
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    QModelIndexList indices = table->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
    {
        bool del = true;
        if (model->data(index.row(), model->fieldIndex(QStringLiteral("InGebrauch"))).toBool())
        {
            QString name = model->data(index.row(), model->fieldIndex(QStringLiteral("Name"))).toString();
            QStringList liste;
            switch (ui->toolBoxRohstoffe->currentIndex())
            {
            case 0:
                liste = bh->modelMalz()->inGebrauchListe(name);
                break;
            case 1:
                liste = bh->modelHopfen()->inGebrauchListe(name);
                break;
            case 2:
                liste = bh->modelHefe()->inGebrauchListe(name);
                break;
            case 3:
                liste = bh->modelWeitereZutaten()->inGebrauchListe(name);
                break;
            }
            QString strListe = "\n\n- " + liste.join(QStringLiteral("\n- "));
            int ret = QMessageBox::question(this, tr("Rohstoff wird verwendet"),
                                            tr("Dieser Rohstoff wird in einem noch nicht gebrauten Sud verwendet. Soll er trotzdem gelöscht werden?") + strListe);
            if (ret != QMessageBox::Yes)
                del = false;
        }
        if (del)
            model->removeRow(index.row());
    }
    updateLabelNumItems();
}

void TabLager::onFilterChanged()
{
    static_cast<ProxyModelRohstoff*>(currentTable()->model())->setFilter(ui->btnFilter->model()->filter());
    updateLabelNumItems();
}

void TabLager::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelRohstoff*>(currentTable()->model())->setFilterString(pattern);
    updateLabelNumItems();
}

void TabLager::on_toolBoxRohstoffe_currentChanged()
{
    ProxyModelRohstoff* model = static_cast<ProxyModelRohstoff*>(currentTable()->model());
    model->setFilterString(ui->tbFilter->text());
    model->setFilter(ui->btnFilter->model()->filter());
    updateLabelNumItems();
}

void TabLager::updateLabelNumItems()
{   
    QAbstractItemModel* model = currentTable()->model();
    QAbstractItemModel* sourceModel = static_cast<ProxyModel*>(model)->sourceModel();
    ProxyModel proxy;
    proxy.setSourceModel(sourceModel);
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}
