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

    int col;
    SqlTableModel *model;
    ProxyModelRohstoff *proxyModel;
    QTableView *table;
    QHeaderView *header;
    ComboBoxDelegate *comboBox;

    gSettings->beginGroup("TabRohstoffe");

    model = bh->modelMalz();
    table = ui->tableMalz;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = ModelMalz::ColBeschreibung;
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelMalz::ColMenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [kg]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelMalz::ColFarbe;
    model->setHeaderData(col, Qt::Horizontal, tr("Farbe [EBC]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new EbcDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelMalz::ColMaxProzent;
    model->setHeaderData(col, Qt::Horizontal, tr("Max. Anteil [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 100, 1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelMalz::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelMalz::ColAnwendung;
    model->setHeaderData(col, Qt::Horizontal, tr("Anwendung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelMalz::ColPreis;
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = ModelMalz::ColEingelagert;
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = ModelMalz::ColMindesthaltbar;
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = ModelMalz::ColLink;
    model->setHeaderData(col, Qt::Horizontal, tr("Link"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new LinkLabelDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(on_tableMalz_customContextMenuRequested(const QPoint&)));

    mDefaultStateTableMalz = header->saveState();
    header->restoreState(gSettings->value("tableMalzState").toByteArray());

    model = bh->modelHopfen();
    table = ui->tableHopfen;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = ModelHopfen::ColBeschreibung;
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelHopfen::ColMenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Menge [g]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, std::numeric_limits<double>::max(), 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelHopfen::ColAlpha;
    model->setHeaderData(col, Qt::Horizontal, tr("Alpha [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelHopfen::ColPellets;
    model->setHeaderData(col, Qt::Horizontal, tr("Pellets"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelHopfen::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelHopfen::ColEigenschaften;
    model->setHeaderData(col, Qt::Horizontal, tr("Eigenschaften"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelHopfen::ColTyp;
    model->setHeaderData(col, Qt::Horizontal, tr("Typ"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate(HopfenTypname, table);
    comboBox->setColors(gSettings->HopfenTypBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = ModelHopfen::ColPreis;
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/kg]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = ModelHopfen::ColEingelagert;
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = ModelHopfen::ColMindesthaltbar;
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = ModelHopfen::ColLink;
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
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = ModelHefe::ColBeschreibung;
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelHefe::ColMenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Menge"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelHefe::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelHefe::ColEigenschaften;
    model->setHeaderData(col, Qt::Horizontal, tr("Eigenschaften"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelHefe::ColTypOGUG;
    model->setHeaderData(col, Qt::Horizontal, tr("OG/UG"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate(HefeTypname, table);
    comboBox->setColors(gSettings->HefeTypOgUgBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelHefe::ColTypTrFl;
    model->setHeaderData(col, Qt::Horizontal, tr("Trocken/Flüssig"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("trocken"), tr("flüssig")}, table);
    comboBox->setColors(gSettings->HefeTypTrFlBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelHefe::ColVerpackungsmenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Verpackungsmenge"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 6);

    col = ModelHefe::ColWuerzemenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Wuerzemenge [l]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = ModelHefe::ColSED;
    model->setHeaderData(col, Qt::Horizontal, tr("Sedimentation"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate({"", tr("hoch"), tr("mittel"), tr("niedrig")}, table);
    comboBox->setColors(gSettings->HefeSedBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = ModelHefe::ColEVG;
    model->setHeaderData(col, Qt::Horizontal, tr("Vergärungsgrad"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = ModelHefe::ColTemperatur;
    model->setHeaderData(col, Qt::Horizontal, tr("Temperatur"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 10);

    col = ModelHefe::ColPreis;
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 11);

    col = ModelHefe::ColEingelagert;
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 12);

    col = ModelHefe::ColMindesthaltbar;
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 13);

    col = ModelHefe::ColLink;
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
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());
    header = table->horizontalHeader();
    proxyModel = new ProxyModelRohstoff(this);
    proxyModel->setSourceModel(model);
    table->setModel(proxyModel);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = ModelWeitereZutaten::ColBeschreibung;
    model->setHeaderData(col, Qt::Horizontal, tr("Name"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new IngredientNameDelegate(table));
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelWeitereZutaten::ColMenge;
    model->setHeaderData(col, Qt::Horizontal, tr("Menge"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, true, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    col = ModelWeitereZutaten::ColEinheiten;
    model->setHeaderData(col, Qt::Horizontal, tr("Einheit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new ComboBoxDelegate(Einheiten, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 2);

    col = ModelWeitereZutaten::ColTyp;
    model->setHeaderData(col, Qt::Horizontal, tr("Typ"));
    table->setColumnHidden(col, false);
    comboBox = new ComboBoxDelegate(ZusatzTypname, table);
    comboBox->setColors(gSettings->WZTypBackgrounds);
    table->setItemDelegateForColumn(col, comboBox);
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 3);

    col = ModelWeitereZutaten::ColAusbeute;
    model->setHeaderData(col, Qt::Horizontal, tr("Ausbeute [%]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new SpinBoxDelegate(0, 100, 1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 4);

    col = ModelWeitereZutaten::ColEBC;
    model->setHeaderData(col, Qt::Horizontal, tr("Farbe [EBC]"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new EbcDelegate(table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 5);

    col = ModelWeitereZutaten::ColBemerkung;
    model->setHeaderData(col, Qt::Horizontal, tr("Bemerkung"));
    table->setColumnHidden(col, false);
    header->resizeSection(col, 200);
    header->moveSection(header->visualIndex(col), 6);

    col = ModelWeitereZutaten::ColPreis;
    model->setHeaderData(col, Qt::Horizontal, tr("Preis [%1/[kg/l/Stk]]").arg(QLocale().currencySymbol()));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 7);

    col = ModelWeitereZutaten::ColEingelagert;
    model->setHeaderData(col, Qt::Horizontal, tr("Einlagerung"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 8);

    col = ModelWeitereZutaten::ColMindesthaltbar;
    model->setHeaderData(col, Qt::Horizontal, tr("Haltbarkeit"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(true, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 9);

    col = ModelWeitereZutaten::ColLink;
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

    col = ModelWasser::ColName;
    model->setHeaderData(col, Qt::Horizontal, tr("Wasserprofil"));
    table->setColumnHidden(col, false);
    header->setSectionResizeMode(col, QHeaderView::Stretch);
    header->moveSection(header->visualIndex(col), 0);

    col = ModelWasser::ColRestalkalitaet;
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

void TabRohstoffe::restoreView(bool full)
{
    Q_UNUSED(full)
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

void TabRohstoffe::on_tableMalz_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableMalz;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = ModelMalz::ColFarbe;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColMaxProzent;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColBemerkung;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColAnwendung;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColPreis;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColEingelagert;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColMindesthaltbar;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenMalz(bool)));
    menu.addAction(action);

    col = ModelMalz::ColLink;
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
    if (index.column() == ModelHopfen::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHopfen_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableHopfen;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = ModelHopfen::ColAlpha;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColPellets;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColBemerkung;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColEigenschaften;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColTyp;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColPreis;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColEingelagert;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColMindesthaltbar;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHopfen(bool)));
    menu.addAction(action);

    col = ModelHopfen::ColLink;
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
    if (index.column() == ModelHefe::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableHefe_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableHefe;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = ModelHefe::ColBemerkung;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColEigenschaften;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColTypOGUG;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColTypTrFl;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColVerpackungsmenge;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColWuerzemenge;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColSED;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColEVG;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColTemperatur;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColPreis;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColEingelagert;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColMindesthaltbar;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenHefe(bool)));
    menu.addAction(action);

    col = ModelHefe::ColLink;
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
    if (index.column() == ModelWeitereZutaten::ColLink && QApplication::keyboardModifiers() & Qt::ControlModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void TabRohstoffe::on_tableWeitereZutaten_customContextMenuRequested(const QPoint &pos)
{
    int col;
    QAction *action;
    QMenu menu(this);
    QTableView *table = ui->tableWeitereZutaten;
    ProxyModelRohstoff *model = static_cast<ProxyModelRohstoff*>(table->model());

    col = ModelWeitereZutaten::ColTyp;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColAusbeute;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColEBC;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColBemerkung;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColPreis;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColEingelagert;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColMindesthaltbar;
    action = new QAction(model->headerData(col, Qt::Horizontal).toString(), &menu);
    action->setCheckable(true);
    action->setChecked(!table->isColumnHidden(col));
    action->setData(col);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(spalteAnzeigenWeitereZutaten(bool)));
    menu.addAction(action);

    col = ModelWeitereZutaten::ColLink;
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

void TabRohstoffe::addEntry(QTableView *table, const QMap<int, QVariant> &values)
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
        updateLabelNumItems();
    }
}

void TabRohstoffe::on_buttonAdd_clicked()
{
    switch (ui->toolBoxRohstoffe->currentIndex())
    {
    case 0:
        addEntry(ui->tableMalz, {{ModelMalz::ColBeschreibung, tr("Neuer Eintrag")}});
        break;
    case 1:
        addEntry(ui->tableHopfen, {{ModelHopfen::ColBeschreibung, tr("Neuer Eintrag")}});
        break;
    case 2:
        addEntry(ui->tableHefe, {{ModelHefe::ColBeschreibung, tr("Neuer Eintrag")}});
        break;
    case 3:
        addEntry(ui->tableWeitereZutaten, {{ModelWeitereZutaten::ColBeschreibung, tr("Neuer Eintrag")}});
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
        values.insert(model->fieldIndex("Beschreibung"), model->data(row, model->fieldIndex("Beschreibung")).toString() + " " + tr("Kopie"));
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
        if (model->data(index.row(), model->fieldIndex("InGebrauch")).toBool())
            replace(ui->toolBoxRohstoffe->currentIndex(), model->data(index.row(), model->fieldIndex("Beschreibung")).toString());
        if (!model->data(index.row(), model->fieldIndex("InGebrauch")).toBool())
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
        dlg.setModel(bh->modelMalz(), ModelMalz::ColBeschreibung);
        break;
    case 1:
        model = bh->modelHopfengaben();
        model2 = bh->modelWeitereZutatenGaben();
        dlg.setModel(bh->modelHopfen(), ModelHopfen::ColBeschreibung);
        break;
    case 2:
        model = bh->modelHefegaben();
        dlg.setModel(bh->modelHefe(), ModelHefe::ColBeschreibung);
        break;
    case 3:
        model = bh->modelWeitereZutatenGaben();
        dlg.setModel(bh->modelWeitereZutaten(), ModelWeitereZutaten::ColBeschreibung);
        break;
    default:
        return;
    }

    for (int i = 0; i < modelSud.rowCount(); ++i)
    {
        int id = modelSud.data(i, ModelSud::ColID).toInt();
        dlg.setSud(modelSud.data(i, ModelSud::ColSudname).toString());
        for (int j = 0; j < model->rowCount(); ++j)
        {
            if (model->data(j, model->fieldIndex("SudID")).toInt() == id && model->data(j, model->fieldIndex("Name")).toString() == rohstoff)
            {
                if (dlg.exec() == QDialog::Accepted)
                    model->setData(j, model->fieldIndex("Name"), dlg.rohstoff());
            }
        }
        if (model2)
        {
            for (int j = 0; j < model2->rowCount(); ++j)
            {
                if (model2->data(j, model->fieldIndex("SudID")).toInt() == id && model2->data(j, model->fieldIndex("Name")).toString() == rohstoff)
                {
                    if (dlg.exec() == QDialog::Accepted)
                        model2->setData(j, model->fieldIndex("Name"), dlg.rohstoff());
                }
            }
        }
    }
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
    ui->tbRestalkalitaet->setValue(dataWasser(ModelWasser::ColRestalkalitaet).toDouble());
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
