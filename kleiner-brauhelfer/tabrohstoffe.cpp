#include "tabrohstoffe.h"
#include "ui_tabrohstoffe.h"
#include <QMessageBox>
#include <QMenu>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QUrl>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodelrohstoff.h"
#include "proxymodelsud.h"
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

extern Brauhelfer* bh;
extern Settings* gSettings;

QList<QString> TabRohstoffe::HopfenTypname = {
    "",
    tr("aroma"),
    tr("bitter"),
    tr("universal")
};

QList<QString> TabRohstoffe::HefeTypname = {
    "",
    tr("obergärig"),
    tr("untergärig")
};

QList<QString> TabRohstoffe::HefeTypFlTrName = {
    "",
    tr("trocken"),
    tr("flüssig")
};

QList<QString> TabRohstoffe::ZusatzTypname = {
    tr("Honig"),
    tr("Zucker"),
    tr("Gewürz"),
    tr("Frucht"),
    tr("Sonstiges"),
    tr("Kraut"),
    tr("Wasseraufbereitung"),
    tr("Klärmittel")
};

QList<QString> TabRohstoffe::Einheiten = {
    tr("kg"),
    tr("g"),
    tr("mg"),
    tr("Stk."),
    tr("l"),
    tr("ml")
};

TabRohstoffe::TabRohstoffe(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRohstoffe)
{
    ui->setupUi(this);

    SqlTableModel *model;
    ProxyModelRohstoff *proxyModel;
    TableView *table;

    gSettings->beginGroup("TabRohstoffe");

    model = bh->modelMalz();
    model->setHeaderData(ModelMalz::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelMalz::ColMenge, Qt::Horizontal, tr("Menge [kg]"));
    model->setHeaderData(ModelMalz::ColFarbe, Qt::Horizontal, tr("Farbe [EBC]"));
    model->setHeaderData(ModelMalz::ColpH, Qt::Horizontal, tr("pH"));
    model->setHeaderData(ModelMalz::ColMaxProzent, Qt::Horizontal, tr("Max. Anteil [%]"));
    model->setHeaderData(ModelMalz::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelMalz::ColEingenschaften, Qt::Horizontal, tr("Eingenschaften"));
    model->setHeaderData(ModelMalz::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelMalz::ColPreis, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelMalz::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelMalz::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelMalz::ColLink, Qt::Horizontal, tr("Link"));
    table = ui->tableMalz;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    table->cols.append({ModelMalz::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->cols.append({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, true, table)});
    table->cols.append({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->cols.append({ModelMalz::ColpH, true, true, 100, new PhMalzDelegate(table)});
    table->cols.append({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->cols.append({ModelMalz::ColBemerkung, true, true, 200, nullptr});
    table->cols.append({ModelMalz::ColEingenschaften, true, true, 200, nullptr});
    table->cols.append({ModelMalz::ColAlternativen, true, true, 200, nullptr});
    table->cols.append({ModelMalz::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->cols.append({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->cols.append({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->cols.append({ModelMalz::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableMalzState").toByteArray());

    model = bh->modelHopfen();
    model->setHeaderData(ModelHopfen::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelHopfen::ColMenge, Qt::Horizontal, tr("Menge [g]"));
    model->setHeaderData(ModelHopfen::ColAlpha, Qt::Horizontal, tr("Alpha [%]"));
    model->setHeaderData(ModelHopfen::ColPellets, Qt::Horizontal, tr("Pellets"));
    model->setHeaderData(ModelHopfen::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelHopfen::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelHopfen::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelHopfen::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelHopfen::ColPreis, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelHopfen::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelHopfen::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelHopfen::ColLink, Qt::Horizontal, tr("Link"));
    table = ui->tableHopfen;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    table->cols.append({ModelHopfen::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->cols.append({ModelHopfen::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, std::numeric_limits<double>::max(), 1, true, table)});
    table->cols.append({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table)});
    table->cols.append({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
    table->cols.append({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(HopfenTypname, gSettings->HopfenTypBackgrounds, table)});
    table->cols.append({ModelHopfen::ColBemerkung, true, true, 200, nullptr});
    table->cols.append({ModelHopfen::ColEigenschaften, true, true, 200, nullptr});
    table->cols.append({ModelHopfen::ColAlternativen, true, true, 200, nullptr});
    table->cols.append({ModelHopfen::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->cols.append({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->cols.append({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->cols.append({ModelHopfen::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableHopfenState").toByteArray());

    model = bh->modelHefe();
    model->setHeaderData(ModelHefe::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelHefe::ColMenge, Qt::Horizontal, tr("Menge"));
    model->setHeaderData(ModelHefe::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelHefe::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelHefe::ColTypOGUG, Qt::Horizontal, tr("OG/UG"));
    model->setHeaderData(ModelHefe::ColTypTrFl, Qt::Horizontal, tr("Trocken/Flüssig"));
    model->setHeaderData(ModelHefe::ColWuerzemenge, Qt::Horizontal, tr("Würzemenge [l]"));
    model->setHeaderData(ModelHefe::ColSedimentation, Qt::Horizontal, tr("Sedimentation"));
    model->setHeaderData(ModelHefe::ColEVG, Qt::Horizontal, tr("Vergärungsgrad"));
    model->setHeaderData(ModelHefe::ColTemperatur, Qt::Horizontal, tr("Temperatur"));
    model->setHeaderData(ModelHefe::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelHefe::ColPreis, Qt::Horizontal, tr("Preis [%1]").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelHefe::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelHefe::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelHefe::ColLink, Qt::Horizontal, tr("Link"));
    table = ui->tableHefe;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    table->cols.append({ModelHefe::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->cols.append({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, true, table)});
    table->cols.append({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(HefeTypname, gSettings->HefeTypOgUgBackgrounds, table)});
    table->cols.append({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, table)});
    table->cols.append({ModelHefe::ColWuerzemenge, true, true, 100, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, table)});
    table->cols.append({ModelHefe::ColSedimentation, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->cols.append({ModelHefe::ColEVG, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->cols.append({ModelHefe::ColTemperatur, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->cols.append({ModelHefe::ColBemerkung, true, true, 200, nullptr});
    table->cols.append({ModelHefe::ColEigenschaften, true, true, 200, nullptr});
    table->cols.append({ModelHefe::ColAlternativen, true, true, 200, nullptr});
    table->cols.append({ModelHefe::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->cols.append({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->cols.append({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->cols.append({ModelHefe::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableHefeState").toByteArray());

    model = bh->modelWeitereZutaten();
    model->setHeaderData(ModelWeitereZutaten::ColName, Qt::Horizontal, tr("Name"));
    model->setHeaderData(ModelWeitereZutaten::ColMenge, Qt::Horizontal, tr("Menge"));
    model->setHeaderData(ModelWeitereZutaten::ColEinheit, Qt::Horizontal, tr("Einheit"));
    model->setHeaderData(ModelWeitereZutaten::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelWeitereZutaten::ColAusbeute, Qt::Horizontal, tr("Ausbeute [%]"));
    model->setHeaderData(ModelWeitereZutaten::ColFarbe, Qt::Horizontal, tr("Farbe [EBC]"));
    model->setHeaderData(ModelWeitereZutaten::ColBemerkung, Qt::Horizontal, tr("Bemerkung"));
    model->setHeaderData(ModelWeitereZutaten::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
    model->setHeaderData(ModelWeitereZutaten::ColAlternativen, Qt::Horizontal, tr("Alternativen"));
    model->setHeaderData(ModelWeitereZutaten::ColPreis, Qt::Horizontal, tr("Preis [%1/[kg/l/Stk]]").arg(QLocale().currencySymbol()));
    model->setHeaderData(ModelWeitereZutaten::ColEingelagert, Qt::Horizontal, tr("Einlagerung"));
    model->setHeaderData(ModelWeitereZutaten::ColMindesthaltbar, Qt::Horizontal, tr("Haltbarkeit"));
    model->setHeaderData(ModelWeitereZutaten::ColLink, Qt::Horizontal, tr("Link"));
    table = ui->tableWeitereZutaten;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    table->cols.append({ModelWeitereZutaten::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->cols.append({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, true, table)});
    table->cols.append({ModelWeitereZutaten::ColEinheit, true, false, 100, new ComboBoxDelegate(Einheiten, table)});
    table->cols.append({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate(ZusatzTypname, gSettings->WZTypBackgrounds, table)});
    table->cols.append({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->cols.append({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->cols.append({ModelWeitereZutaten::ColBemerkung, true, true, 200, nullptr});
    table->cols.append({ModelWeitereZutaten::ColEigenschaften, true, true, 200, nullptr});
    table->cols.append({ModelWeitereZutaten::ColAlternativen, true, true, 200, nullptr});
    table->cols.append({ModelWeitereZutaten::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->cols.append({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->cols.append({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->cols.append({ModelWeitereZutaten::ColLink, true, true, 100, new LinkLabelDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableWeitereZutatenState").toByteArray());

    model = bh->modelWasser();
    model->setHeaderData(ModelWasser::ColName, Qt::Horizontal, tr("Wasserprofil"));
    model->setHeaderData(ModelWasser::ColRestalkalitaet, Qt::Horizontal, tr("Restalkalität [°dH]"));
    ProxyModel *proxyModelWasser = new ProxyModel(this);
    table = ui->tableWasser;
    proxyModelWasser->setSourceModel(model);
    table->setModel(proxyModelWasser);
    table->cols.append({ModelWasser::ColName, true, false, -1, nullptr});
    table->cols.append({ModelWasser::ColRestalkalitaet, true, false, 120, new DoubleSpinBoxDelegate(2, table)});
    table->build();
    table->restoreState(gSettings->value("tableWasserState").toByteArray());

    int filter = gSettings->value("filter", 0).toInt();
    if (filter == 1)
    {
        ui->radioButtonVorhanden->setChecked(true);
        on_radioButtonVorhanden_clicked();
    }
    else if (filter == 2)
    {
        ui->radioButtonInGebrauch->setChecked(true);
        on_radioButtonInGebrauch_clicked();
    }
    else
    {
        ui->radioButtonAlle->setChecked(true);
        on_radioButtonAlle_clicked();
    }

    gSettings->endGroup();

    connect(ui->tableWasser->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(wasser_selectionChanged(const QItemSelection&)));

    connect(bh->modelWasser(), SIGNAL(modified()), this, SLOT(updateWasser()));

    ui->tableWasser->selectRow(0);
}

TabRohstoffe::~TabRohstoffe()
{
    delete ui;
}

void TabRohstoffe::saveSettings()
{
    gSettings->beginGroup("TabRohstoffe");
    gSettings->setValue("tableMalzState", ui->tableMalz->horizontalHeader()->saveState());
    gSettings->setValue("tableHopfenState", ui->tableHopfen->horizontalHeader()->saveState());
    gSettings->setValue("tableHefeState", ui->tableHefe->horizontalHeader()->saveState());
    gSettings->setValue("tableWeitereZutatenState", ui->tableWeitereZutaten->horizontalHeader()->saveState());
    gSettings->setValue("tableWasserState", ui->tableWasser->horizontalHeader()->saveState());
    int filter = 0;
    if (ui->radioButtonVorhanden->isChecked())
        filter = 1;
    else if (ui->radioButtonInGebrauch->isChecked())
        filter = 2;
    gSettings->setValue("filter", filter);
    gSettings->endGroup();
}

void TabRohstoffe::restoreView(bool full)
{
    Q_UNUSED(full)
    ui->tableMalz->restoreDefaultState();
    ui->tableHopfen->restoreDefaultState();
    ui->tableHefe->restoreDefaultState();
    ui->tableWeitereZutaten->restoreDefaultState();
    ui->tableWasser->restoreDefaultState();
}

void TabRohstoffe::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    if (table->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_buttonDelete_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_buttonAdd_clicked();
            break;
        }
    }
}

void TabRohstoffe::on_tableMalz_clicked(const QModelIndex &index)
{
    if (index.column() == ModelMalz::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHopfen_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHopfen::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHefe_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHefe::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableWeitereZutaten_clicked(const QModelIndex &index)
{
    if (index.column() == ModelWeitereZutaten::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::addEntry(QTableView *table, const QMap<int, QVariant> &values)
{
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());
    ui->radioButtonAlle->setChecked(true);
    on_radioButtonAlle_clicked();
    ui->lineEditFilter->clear();
    int row = model->append(values);
    if (row >= 0)
    {
        table->setCurrentIndex(model->index(row, model->fieldIndex("Name")));
        table->scrollTo(table->currentIndex());
        table->edit(table->currentIndex());
        updateLabelNumItems();
    }
}

void TabRohstoffe::on_buttonAdd_clicked()
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

void TabRohstoffe::on_buttonNeuVorlage_clicked()
{
    QTableView *table;
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        art = DlgRohstoffVorlage::Art::Malz;
        break;
    case 1:
        table = ui->tableHopfen;
        art = DlgRohstoffVorlage::Art::Hopfen;
        break;
    case 2:
        table = ui->tableHefe;
        art = DlgRohstoffVorlage::Art::Hefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        art = DlgRohstoffVorlage::Art::WZutaten;
        break;
    default:
        return;
    }
    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void TabRohstoffe::on_buttonNeuVorlageObrama_clicked()
{
    QTableView *table;
    DlgRohstoffVorlage::Art art;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        art = DlgRohstoffVorlage::Art::MalzOBraMa;
        break;
    case 1:
        table = ui->tableHopfen;
        art = DlgRohstoffVorlage::Art::HopfenOBraMa;
        break;
    case 2:
        table = ui->tableHefe;
        art = DlgRohstoffVorlage::Art::HefeOBraMa;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        art = DlgRohstoffVorlage::Art::WZutatenOBraMa;
        break;
    default:
        return;
    }
    DlgRohstoffVorlage dlg(art, this);
    if (dlg.exec() == QDialog::Accepted)
        addEntry(table, dlg.values());
}

void TabRohstoffe::on_buttonCopy_clicked()
{
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());
    SqlTableModel *sourceModel = static_cast<SqlTableModel*>(model->sourceModel());
    for (const QModelIndex& index : table->selectionModel()->selectedRows())
    {
        int row = index.row();
        QMap<int, QVariant> values = sourceModel->copyValues(model->mapRowToSource(row));
        values.insert(model->fieldIndex("Name"), model->data(row, model->fieldIndex("Name")).toString() + " " + tr("Kopie"));
        values.remove(model->fieldIndex("Menge"));
        values.remove(model->fieldIndex("Eingelagert"));
        values.remove(model->fieldIndex("Mindesthaltbar"));
        addEntry(table, values);
    }
}

void TabRohstoffe::on_buttonDelete_clicked()
{
    QTableView *table;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        break;
    case 1:
        table = ui->tableHopfen;
        break;
    case 2:
        table = ui->tableHefe;
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        break;
    default:
        return;
    }
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());
    QModelIndexList indices = table->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        bool del = true;
        if (model->data(index.row(), model->fieldIndex("InGebrauch")).toBool())
        {
            int ret = QMessageBox::question(this, tr("Rohstoff wird verwendet"),
                                            tr("Dieser Rohstoff wird in einem noch nicht gebrauten Sud verwendet. Soll er trotzdem gelöscht werden?"));
            if (ret != QMessageBox::Yes)
                del = false;
        }
        if (del)
            model->removeRow(index.row());
    }
    updateLabelNumItems();
}

void TabRohstoffe::on_radioButtonAlle_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Alle);
    updateLabelNumItems();
}

void TabRohstoffe::on_radioButtonVorhanden_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
    updateLabelNumItems();
}

void TabRohstoffe::on_radioButtonInGebrauch_clicked()
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilter(ProxyModelRohstoff::Filter::InGebrauch);
    updateLabelNumItems();
}

void TabRohstoffe::on_lineEditFilter_textChanged(const QString &pattern)
{
    ProxyModelRohstoff *proxyModel;
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableMalz->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHopfen->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableHefe->model());
    proxyModel->setFilterString(pattern);
    proxyModel = static_cast<ProxyModelRohstoff*>(ui->tableWeitereZutaten->model());
    proxyModel->setFilterString(pattern);
    updateLabelNumItems();
}

void TabRohstoffe::on_toolBoxRohstoffe_currentChanged(int index)
{
    ui->buttonAdd->setEnabled(index != 4);
    ui->buttonNeuVorlage->setEnabled(index != 4);
    ui->buttonNeuVorlageObrama->setEnabled(index != 4);
    ui->buttonCopy->setEnabled(index != 4);
    ui->buttonDelete->setEnabled(index != 4);
    updateLabelNumItems();
}

void TabRohstoffe::updateLabelNumItems()
{
    QAbstractItemModel* filteredModel;
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        filteredModel = ui->tableMalz->model();
        break;
    case 1:
        filteredModel = ui->tableHopfen->model();
        break;
    case 2:
        filteredModel = ui->tableHefe->model();
        break;
    case 3:
        filteredModel = ui->tableWeitereZutaten->model();
        break;
    default:
        ui->lblNumItems->setText("");
        return;
    }
    QAbstractItemModel* sourceModel = static_cast<ProxyModel*>(filteredModel)->sourceModel();
    ProxyModel sourceModelNoDelete;
    sourceModelNoDelete.setSourceModel(sourceModel);
    ui->lblNumItems->setText(QString::number(filteredModel->rowCount()) + " / " + QString::number(sourceModelNoDelete.rowCount()));
}

QVariant TabRohstoffe::dataWasser(int col) const
{
    return bh->modelWasser()->data(mRowWasser, col);
}

bool TabRohstoffe::setDataWasser(int col, const QVariant &value)
{
    return bh->modelWasser()->setData(mRowWasser, col, value);
}

void TabRohstoffe::wasser_selectionChanged(const QItemSelection &selected)
{
    if (selected.indexes().count() > 0)
    {
        mRowWasser = selected.indexes()[0].row();
        updateWasser();
    }
}

void TabRohstoffe::on_btnNeuesWasserprofil_clicked()
{
    QMap<int, QVariant> values({{ModelWasser::ColName, tr("Neues Profil")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, ModelWasser::ColName);
        ui->tableWasser->setCurrentIndex(index);
        ui->tableWasser->scrollTo(ui->tableWasser->currentIndex());
        ui->tableWasser->edit(ui->tableWasser->currentIndex());
    }
}

void TabRohstoffe::on_btnWasserprofilLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableWasser->model());
    for (const QModelIndex& index : ui->tableWasser->selectionModel()->selectedRows())
    {
        QString name = model->data(index.row(), ModelWasser::ColName).toString();
        int ret = QMessageBox::question(this, tr("Wasserprofil löschen?"),
                                        tr("Soll das Wasserprofil \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
            int row = model->getRowWithValue(ModelWasser::ColName, name);
            if (row >= 0)
                model->removeRow(row);
        }
    }
}

void TabRohstoffe::updateWasser()
{
    if (!ui->tbCalciumMg->hasFocus())
        ui->tbCalciumMg->setValue(dataWasser(ModelWasser::ColCalcium).toDouble());
    if (!ui->tbCalciumMmol->hasFocus())
        ui->tbCalciumMmol->setValue(dataWasser(ModelWasser::ColCalciumMmol).toDouble());
    if (!ui->tbMagnesiumMg->hasFocus())
        ui->tbMagnesiumMg->setValue(dataWasser(ModelWasser::ColMagnesium).toDouble());
    if (!ui->tbMagnesiumMmol->hasFocus())
        ui->tbMagnesiumMmol->setValue(dataWasser(ModelWasser::ColMagnesiumMmol).toDouble());
    if (!ui->tbSaeurekapazitaet->hasFocus())
        ui->tbSaeurekapazitaet->setValue(dataWasser(ModelWasser::ColSaeurekapazitaet).toDouble());
    if (!ui->tbCarbonathaerte->hasFocus())
        ui->tbCarbonathaerte->setValue(dataWasser(ModelWasser::ColCarbonathaerte).toDouble());
    ui->tbCalciumhaerte->setValue(dataWasser(ModelWasser::ColCalciumhaerte).toDouble());
    ui->tbMagnesiumhaerte->setValue(dataWasser(ModelWasser::ColMagnesiumhaerte).toDouble());
    if (!ui->tbRestalkalitaetAdd->hasFocus())
        ui->tbRestalkalitaetAdd->setValue(dataWasser(ModelWasser::ColRestalkalitaetAdd).toDouble());
    ui->tbRestalkalitaet->setValue(dataWasser(ModelWasser::ColRestalkalitaet).toDouble());
    if (!ui->tbBemerkung->hasFocus())
        ui->tbBemerkung->setText(dataWasser(ModelWasser::ColBemerkung).toString());
}

void TabRohstoffe::on_tbCalciumMg_valueChanged(double value)
{
    if (ui->tbCalciumMg->hasFocus())
        setDataWasser(ModelWasser::ColCalcium, value);
}

void TabRohstoffe::on_tbCalciumMmol_valueChanged(double value)
{
    if (ui->tbCalciumMmol->hasFocus())
        setDataWasser(ModelWasser::ColCalciumMmol, value);
}

void TabRohstoffe::on_tbMagnesiumMg_valueChanged(double value)
{
    if (ui->tbMagnesiumMg->hasFocus())
        setDataWasser(ModelWasser::ColMagnesium, value);
}

void TabRohstoffe::on_tbMagnesiumMmol_valueChanged(double value)
{
    if (ui->tbMagnesiumMmol->hasFocus())
        setDataWasser(ModelWasser::ColMagnesiumMmol, value);
}

void TabRohstoffe::on_tbSaeurekapazitaet_valueChanged(double value)
{
    if (ui->tbSaeurekapazitaet->hasFocus())
        setDataWasser(ModelWasser::ColSaeurekapazitaet, value);
}

void TabRohstoffe::on_tbCarbonathaerte_valueChanged(double value)
{
    if (ui->tbCarbonathaerte->hasFocus())
        setDataWasser(ModelWasser::ColCarbonathaerte, value);
}

void TabRohstoffe::on_tbRestalkalitaetAdd_valueChanged(double value)
{
    if (ui->tbRestalkalitaetAdd->hasFocus())
        setDataWasser(ModelWasser::ColRestalkalitaetAdd, value);
}

void TabRohstoffe::on_tbBemerkung_textChanged()
{
    if (ui->tbBemerkung->hasFocus())
        setDataWasser(ModelWasser::ColBemerkung, ui->tbBemerkung->toPlainText());
}
