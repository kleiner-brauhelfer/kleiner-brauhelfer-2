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
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/ingredientnamedelegate.h"
#include "model/linklabeldelegate.h"
#include "model/spinboxdelegate.h"
#include "dialogs/dlgrohstoffaustausch.h"
#include "dialogs/dlgrohstoffvorlage.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabRohstoffe::TabRohstoffe(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRohstoffe)
{
    ui->setupUi(this);

    int col;
    SqlTableModel *model;
    ProxyModelRohstoff *proxyModel;
    QTableView *table;
    QHeaderView *header;
    ComboBoxDelegate *comboBox;

    gSettings->beginGroup("TabRohstoffe");

    model = bh->modelMalz();
    table = ui->tableMalz;
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Beschreibung");
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Menge");
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [kg]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Farbe");
    model->setHeaderData(col, Qt::Horizontal, tr("Farbe [EBC]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new EbcDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("MaxProzent");
    model->setHeaderData(col, Qt::Horizontal, tr("Max. Anteil [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 100, 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("Bemerkung");
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("Anwendung");
    model->setHeaderData(col, Qt::Horizontal, tr("Anwendung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 5);

    col = model->fieldIndex("Preis");
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = model->fieldIndex("Eingelagert");
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = model->fieldIndex("Mindesthaltbar");
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = model->fieldIndex("Link");
    model->setHeaderData(col, Qt::Horizontal, tr("Link"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(model->fieldIndex("Link"), new LinkLabelDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableMalz_customContextMenuRequested(const QPoint&)));

    mDefaultStateTableMalz = header->saveState();
    header->restoreState(gSettings->value("tableMalzState").toByteArray());

    model = bh->modelHopfen();
    table = ui->tableHopfen;
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Beschreibung");
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Menge");
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [g]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 9999, 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Alpha");
    model->setHeaderData(col, Qt::Horizontal, tr("Alpha [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Pellets");
    model->setHeaderData(col, Qt::Horizontal, tr("Pellets"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("Bemerkung");
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("Eigenschaften");
    model->setHeaderData(col, Qt::Horizontal, tr("Eigenschaften"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 5);

    col = model->fieldIndex("Typ");
    model->setHeaderData(col, Qt::Horizontal, tr("Typ"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("Aroma"), tr("Bitter"), tr("Universal")}, table);
    comboBox->setColors(gSettings->HopfenTypBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = model->fieldIndex("Preis");
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = model->fieldIndex("Eingelagert");
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = model->fieldIndex("Mindesthaltbar");
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = model->fieldIndex("Link");
    model->setHeaderData(col, Qt::Horizontal, tr("Link"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new LinkLabelDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 10);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableHopfen_customContextMenuRequested(const QPoint&)));

    mDefaultStateTableHopfen = header->saveState();
    header->restoreState(gSettings->value("tableHopfenState").toByteArray());

    model = bh->modelHefe();
    table = ui->tableHefe;
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Beschreibung");
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Menge");
    model->setHeaderData(col, Qt::Horizontal, tr("Menge"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 9999, 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Bemerkung");
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Eigenschaften");
    model->setHeaderData(col, Qt::Horizontal, tr("Eigenschaften"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("TypOGUG");
    model->setHeaderData(col, Qt::Horizontal, tr("OG/UG"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("OG"), tr("UG")}, table);
    comboBox->setColors(gSettings->HefeTypOgUgBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("TypTrFl");
    model->setHeaderData(col, Qt::Horizontal, tr("Trocken/Flüssig"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("Trocken"), tr("Flüssig")}, table);
    comboBox->setColors(gSettings->HefeTypTrFlBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = model->fieldIndex("Verpackungsmenge");
    model->setHeaderData(col, Qt::Horizontal, tr("Verpackungsmenge"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = model->fieldIndex("Wuerzemenge");
    model->setHeaderData(col, Qt::Horizontal, tr("Wuerzemenge [l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 9999, 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = model->fieldIndex("SED");
    model->setHeaderData(col, Qt::Horizontal, tr("Sedimentation"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("Hoch"), tr("Mittel"), tr("Niedrig")}, table);
    comboBox->setColors(gSettings->HefeSedBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = model->fieldIndex("EVG");
    model->setHeaderData(col, Qt::Horizontal, tr("Vergärungsgrad"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = model->fieldIndex("Temperatur");
    model->setHeaderData(col, Qt::Horizontal, tr("Temperatur"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 10);

    col = model->fieldIndex("Preis");
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 11);

    col = model->fieldIndex("Eingelagert");
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 12);

    col = model->fieldIndex("Mindesthaltbar");
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 13);

    col = model->fieldIndex("Link");
    model->setHeaderData(col, Qt::Horizontal, tr("Link"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new LinkLabelDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 14);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableHefe_customContextMenuRequested(const QPoint&)));

    mDefaultStateTableHefe = header->saveState();
    header->restoreState(gSettings->value("tableHefeState").toByteArray());

    model = bh->modelWeitereZutaten();
    table = ui->tableWeitereZutaten;
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Beschreibung");
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Menge");
    model->setHeaderData(col, Qt::Horizontal, tr("Menge"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = model->fieldIndex("Einheiten");
    model->setHeaderData(col, Qt::Horizontal, tr("Einheit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new ComboBoxDelegate({tr("kg"), tr("g"), tr("mg"), tr("Stk.")}, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = model->fieldIndex("Typ");
    model->setHeaderData(col, Qt::Horizontal, tr("Typ"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({tr("Honig"), tr("Zucker"), tr("Gewürz"), tr("Frucht"), tr("Sonstiges")}, table);
    comboBox->setColors(gSettings->WZTypBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = model->fieldIndex("Ausbeute");
    model->setHeaderData(col, Qt::Horizontal, tr("Ausbeute [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 100, 1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = model->fieldIndex("EBC");
    model->setHeaderData(col, Qt::Horizontal, tr("Farbe [EBC]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new EbcDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = model->fieldIndex("Bemerkung");
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 6);

    col = model->fieldIndex("Preis");
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, 9999.9, 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = model->fieldIndex("Eingelagert");
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = model->fieldIndex("Mindesthaltbar");
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = model->fieldIndex("Link");
    model->setHeaderData(col, Qt::Horizontal, tr("Link"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new LinkLabelDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 10);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableWeitereZutaten_customContextMenuRequested(const QPoint&)));

    mDefaultStateTableWeitereZutaten = header->saveState();
    header->restoreState(gSettings->value("tableWeitereZutatenState").toByteArray());

    model = bh->modelWasser();
    ProxyModel *proxyModelWasser = new ProxyModel(this);
    table = ui->tableWasser;
    header = table->horizontalHeader();
    proxyModelWasser->setSourceModel(model);
    table->setModel(proxyModelWasser);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = model->fieldIndex("Name");
    model->setHeaderData(col, Qt::Horizontal, tr("Wasserprofil"));
    table->setColumnHidden(col, false);
    header->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);

    col = model->fieldIndex("Restalkalitaet");
    model->setHeaderData(col, Qt::Horizontal, tr("Restalkalität [°dH]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, table));
    header->resizeSection(col, 120);
    header->moveSection(header->visualIndex(col), 1);

    mDefaultStateTableWasser = header->saveState();
    header->restoreState(gSettings->value("tableWasserState").toByteArray());

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

void TabRohstoffe::restoreView()
{
    ui->tableMalz->horizontalHeader()->restoreState(mDefaultStateTableMalz);
    ui->tableHopfen->horizontalHeader()->restoreState(mDefaultStateTableHopfen);
    ui->tableHefe->horizontalHeader()->restoreState(mDefaultStateTableHefe);
    ui->tableWeitereZutaten->horizontalHeader()->restoreState(mDefaultStateTableWeitereZutaten);
    ui->tableWasser->horizontalHeader()->restoreState(mDefaultStateTableWasser);
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
        }
    }
}

void TabRohstoffe::on_tableMalz_clicked(const QModelIndex &index)
{
    if (index.column() == bh->modelMalz()->fieldIndex("Link") && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableMalz_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableMalz;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = model->fieldIndex("Farbe");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("MaxProzent");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Bemerkung");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Anwendung");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Preis");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eingelagert");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Mindesthaltbar");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Link");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}

void TabRohstoffe::spalteAnzeigenMalz(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableMalz->setColumnHidden(action->data().toInt(), !checked);
}

void TabRohstoffe::on_tableHopfen_clicked(const QModelIndex &index)
{
    if (index.column() == bh->modelHopfen()->fieldIndex("Link") && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHopfen_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableHopfen;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = model->fieldIndex("Alpha");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Pellets");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Bemerkung");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eigenschaften");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Typ");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Preis");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eingelagert");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Mindesthaltbar");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Link");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}

void TabRohstoffe::spalteAnzeigenHopfen(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableHopfen->setColumnHidden(action->data().toInt(), !checked);
}

void TabRohstoffe::on_tableHefe_clicked(const QModelIndex &index)
{
    if (index.column() == bh->modelHefe()->fieldIndex("Link") && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHefe_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableHefe;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = model->fieldIndex("Bemerkung");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eigenschaften");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("TypOGUG");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("TypTrFl");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Verpackungsmenge");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Wuerzemenge");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("SED");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("EVG");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Temperatur");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Preis");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eingelagert");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Mindesthaltbar");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Link");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}

void TabRohstoffe::spalteAnzeigenHefe(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableHefe->setColumnHidden(action->data().toInt(), !checked);
}

void TabRohstoffe::on_tableWeitereZutaten_clicked(const QModelIndex &index)
{
    if (index.column() == bh->modelWeitereZutaten()->fieldIndex("Link") && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableWeitereZutaten_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableWeitereZutaten;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = model->fieldIndex("Typ");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Ausbeute");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("EBC");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Bemerkung");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Preis");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Eingelagert");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Mindesthaltbar");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = model->fieldIndex("Link");
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    menu.exec(table->viewport()->mapToGlobal(pos));
}

void TabRohstoffe::spalteAnzeigenWeitereZutaten(bool checked)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        ui->tableWeitereZutaten->setColumnHidden(action->data().toInt(), !checked);
}

void TabRohstoffe::addEntry(QTableView *table, const QVariantMap &values)
{
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());
    ui->radioButtonAlle->setChecked(true);
    on_radioButtonAlle_clicked();
    ui->lineEditFilter->clear();
    int row = model->append(values);
    if (row >= 0)
    {
        table->setCurrentIndex(model->index(row, model->fieldIndex("Beschreibung")));
        table->scrollTo(table->currentIndex());
        table->edit(table->currentIndex());
    }
}

void TabRohstoffe::on_buttonAdd_clicked()
{
    QVariantMap values({{"Beschreibung", tr("Neuer Eintrag")}});
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        addEntry(ui->tableMalz, values);
        break;
    case 1:
        addEntry(ui->tableHopfen, values);
        break;
    case 2:
        addEntry(ui->tableHefe, values);
        break;
    case 3:
        addEntry(ui->tableWeitereZutaten, values);
        break;
    }
}

void TabRohstoffe::on_buttonNeuVorlage_clicked()
{
    QTableView *table;
    DlgRohstoffVorlage dlg(this);
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        table = ui->tableMalz;
        dlg.ViewMalzauswahl();
        break;
    case 1:
        table = ui->tableHopfen;
        dlg.ViewHopfenauswahl();
        break;
    case 2:
        table = ui->tableHefe;
        dlg.ViewHefeauswahl();
        break;
    case 3:
        table = ui->tableWeitereZutaten;
        dlg.ViewWeitereZutatenauswahl();
        break;
    default:
        return;
    }
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
        QVariantMap values = sourceModel->copyValues(model->mapRowToSource(row));
        values.insert("Beschreibung", model->data(row, "Beschreibung").toString() + " " + tr("Kopie"));
        values.remove("Menge");
        values.remove("Eingelagert");
        values.remove("Mindesthaltbar");
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
        if (model->data(index.row(), "InGebrauch").toBool())
            replace(ui->toolBoxRohstoffe->currentIndex(), model->data(index.row(), "Beschreibung").toString());
        if (!model->data(index.row(), "InGebrauch").toBool())
            model->removeRow(index.row());
    }
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
}

void TabRohstoffe::on_toolBoxRohstoffe_currentChanged(int index)
{
    ui->buttonAdd->setEnabled(index != 4);
    ui->buttonNeuVorlage->setEnabled(index != 4);
    ui->buttonCopy->setEnabled(index != 4);
    ui->buttonDelete->setEnabled(index != 4);
}

void TabRohstoffe::replace(int type, const QString &rohstoff)
{
    DlgRohstoffAustausch dlg(DlgRohstoffAustausch::Loeschen, rohstoff, this);
    ProxyModelSud modelSud;
    modelSud.setSourceModel(bh->modelSud());
    modelSud.setFilterStatus(ProxyModelSud::Rezept | ProxyModelSud::Gebraut);
    SqlTableModel *model = nullptr;
    SqlTableModel *model2 = nullptr;
    switch (type)
    {
    case 0:
        model = bh->modelMalzschuettung();
        dlg.setModel(bh->modelMalz(), bh->modelMalz()->fieldIndex("Beschreibung"));
        break;
    case 1:
        model = bh->modelHopfengaben();
        model2 = bh->modelWeitereZutatenGaben();
        dlg.setModel(bh->modelHopfen(), bh->modelHopfen()->fieldIndex("Beschreibung"));
        break;
    case 2:
        model = bh->modelHefegaben();
        dlg.setModel(bh->modelHefe(), bh->modelHefe()->fieldIndex("Beschreibung"));
        break;
    case 3:
        model = bh->modelWeitereZutatenGaben();
        dlg.setModel(bh->modelWeitereZutaten(), bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
        break;
    default:
        return;
    }

    for (int i = 0; i < modelSud.rowCount(); ++i)
    {
        int id = modelSud.data(i, "ID").toInt();
        dlg.setSud(modelSud.data(i, "Sudname").toString());
        for (int j = 0; j < model->rowCount(); ++j)
        {
            if (model->data(j, "SudID").toInt() == id && model->data(j, "Name").toString() == rohstoff)
            {
                if (dlg.exec() == QDialog::Accepted)
                    model->setData(j, "Name", dlg.rohstoff());
            }
        }
        if (model2)
        {
            for (int j = 0; j < model2->rowCount(); ++j)
            {
                if (model2->data(j, "SudID").toInt() == id && model2->data(j, "Name").toString() == rohstoff)
                {
                    if (dlg.exec() == QDialog::Accepted)
                        model2->setData(j, "Name", dlg.rohstoff());
                }
            }
        }
    }
}

QVariant TabRohstoffe::dataWasser(const QString &fieldName) const
{
    return bh->modelWasser()->data(mRowWasser, fieldName);
}

bool TabRohstoffe::setDataWasser(const QString &fieldName, const QVariant &value)
{
    return bh->modelWasser()->setData(mRowWasser, fieldName, value);
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
    QVariantMap values({{"Name", tr("Neues Profil")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, model->fieldIndex("Name"));
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
        QString name = model->data(index.row(), "Name").toString();
        int ret = QMessageBox::question(this, tr("Wasserprofil löschen?"),
                                        tr("Soll das Wasserprofil \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->tableWasser->model());
            int row = model->getRowWithValue("Name", name);
            if (row >= 0)
                model->removeRow(row);
        }
    }
}

void TabRohstoffe::updateWasser()
{
    if (!ui->tbCalciumMg->hasFocus())
        ui->tbCalciumMg->setValue(dataWasser("Calcium").toDouble());
    if (!ui->tbCalciumMmol->hasFocus())
        ui->tbCalciumMmol->setValue(dataWasser("CalciumMmol").toDouble());
    if (!ui->tbMagnesiumMg->hasFocus())
        ui->tbMagnesiumMg->setValue(dataWasser("Magnesium").toDouble());
    if (!ui->tbMagnesiumMmol->hasFocus())
        ui->tbMagnesiumMmol->setValue(dataWasser("MagnesiumMmol").toDouble());
    if (!ui->tbSaeurekapazitaet->hasFocus())
        ui->tbSaeurekapazitaet->setValue(dataWasser("Saeurekapazitaet").toDouble());
    if (!ui->tbCarbonathaerte->hasFocus())
        ui->tbCarbonathaerte->setValue(dataWasser("Carbonathaerte").toDouble());
    ui->tbCalciumhaerte->setValue(dataWasser("Calciumhaerte").toDouble());
    ui->tbMagnesiumhaerte->setValue(dataWasser("Magnesiumhaerte").toDouble());
    ui->tbRestalkalitaet->setValue(dataWasser("Restalkalitaet").toDouble());
}

void TabRohstoffe::on_tbCalciumMg_valueChanged(double value)
{
    if (ui->tbCalciumMg->hasFocus())
        setDataWasser("Calcium", value);
}

void TabRohstoffe::on_tbCalciumMmol_valueChanged(double value)
{
    if (ui->tbCalciumMmol->hasFocus())
        setDataWasser("CalciumMmol", value);
}

void TabRohstoffe::on_tbMagnesiumMg_valueChanged(double value)
{
    if (ui->tbMagnesiumMg->hasFocus())
        setDataWasser("Magnesium", value);
}

void TabRohstoffe::on_tbMagnesiumMmol_valueChanged(double value)
{
    if (ui->tbMagnesiumMmol->hasFocus())
        setDataWasser("MagnesiumMmol", value);
}

void TabRohstoffe::on_tbSaeurekapazitaet_valueChanged(double value)
{
    if (ui->tbSaeurekapazitaet->hasFocus())
        setDataWasser("Saeurekapazitaet", value);
}

void TabRohstoffe::on_tbCarbonathaerte_valueChanged(double value)
{
    if (ui->tbCarbonathaerte->hasFocus())
        setDataWasser("Carbonathaerte", value);
}
