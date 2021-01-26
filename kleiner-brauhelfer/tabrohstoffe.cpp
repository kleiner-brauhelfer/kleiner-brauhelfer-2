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

QStringList TabRohstoffe::HopfenTypname = {
    "",
    tr("aroma"),
    tr("bitter"),
    tr("universal")
};

QStringList TabRohstoffe::HefeTypname = {
    "",
    tr("obergärig"),
    tr("untergärig")
};

QStringList TabRohstoffe::HefeTypFlTrName = {
    "",
    tr("trocken"),
    tr("flüssig")
};

QStringList TabRohstoffe::ZusatzTypname = {
    tr("Honig"),
    tr("Zucker"),
    tr("Gewürz"),
    tr("Frucht"),
    tr("Sonstiges"),
    tr("Kraut"),
    tr("Wasseraufbereitung"),
    tr("Klärmittel")
};

QStringList TabRohstoffe::Einheiten = {
    tr("kg"),
    tr("g"),
    tr("mg"),
    tr("Stk."),
    tr("l"),
    tr("ml")
};

QStringList TabRohstoffe::list_tr(const QStringList& list)
{
   QStringList result;
   for (const QString& str : list)
       result.append(tr(str.toStdString().c_str()));
   return result;
}

TabRohstoffe::TabRohstoffe(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabRohstoffe)
{
    QPalette pal;

    HopfenTypname = list_tr(HopfenTypname);
    HefeTypname = list_tr(HefeTypname);
    HefeTypFlTrName = list_tr(HefeTypFlTrName);
    ZusatzTypname = list_tr(ZusatzTypname);
    Einheiten = list_tr(Einheiten);

    ui->setupUi(this);

    pal = ui->tableMalz->palette();
    pal.setColor(QPalette::Button, gSettings->colorMalz);
    ui->tableMalz->setPalette(pal);

    pal = ui->tableHopfen->palette();
    pal.setColor(QPalette::Button, gSettings->colorHopfen);
    ui->tableHopfen->setPalette(pal);

    pal = ui->tableHefe->palette();
    pal.setColor(QPalette::Button, gSettings->colorHefe);
    ui->tableHefe->setPalette(pal);

    pal = ui->tableWeitereZutaten->palette();
    pal.setColor(QPalette::Button, gSettings->colorZusatz);
    ui->tableWeitereZutaten->setPalette(pal);

    pal = ui->tableWasser->palette();
    pal.setColor(QPalette::Button, gSettings->colorWasser);
    ui->tableWasser->setPalette(pal);

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
    model->setHeaderData(ModelMalz::ColEigenschaften, Qt::Horizontal, tr("Eigenschaften"));
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
    table->appendCol({ModelMalz::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->appendCol({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->appendCol({ModelMalz::ColpH, true, true, 100, new PhMalzDelegate(table)});
    table->appendCol({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelMalz::ColBemerkung, true, true, 200, nullptr});
    table->appendCol({ModelMalz::ColEigenschaften, true, true, 200, nullptr});
    table->appendCol({ModelMalz::ColAlternativen, true, true, 200, nullptr});
    table->appendCol({ModelMalz::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->appendCol({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->appendCol({ModelMalz::ColLink, true, true, 100, new LinkLabelDelegate(table)});
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
    table->appendCol({ModelHopfen::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->appendCol({ModelHopfen::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(1, 0.0, std::numeric_limits<double>::max(), 1, false, table)});
    table->appendCol({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, 0.0, 100.0, 0.1, true, table)});
    table->appendCol({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
    table->appendCol({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(HopfenTypname, gSettings->HopfenTypBackgrounds, table)});
    table->appendCol({ModelHopfen::ColBemerkung, true, true, 200, nullptr});
    table->appendCol({ModelHopfen::ColEigenschaften, true, true, 200, nullptr});
    table->appendCol({ModelHopfen::ColAlternativen, true, true, 200, nullptr});
    table->appendCol({ModelHopfen::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->appendCol({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->appendCol({ModelHopfen::ColLink, true, true, 100, new LinkLabelDelegate(table)});
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
    table->appendCol({ModelHefe::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->appendCol({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(0, std::numeric_limits<int>::max(), 1, false, table)});
    table->appendCol({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(HefeTypname, gSettings->HefeTypOgUgBackgrounds, table)});
    table->appendCol({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, table)});
    table->appendCol({ModelHefe::ColWuerzemenge, true, true, 100, new DoubleSpinBoxDelegate(1, 0, std::numeric_limits<double>::max(), 1, false, table)});
    table->appendCol({ModelHefe::ColSedimentation, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColEVG, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColTemperatur, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelHefe::ColBemerkung, true, true, 200, nullptr});
    table->appendCol({ModelHefe::ColEigenschaften, true, true, 200, nullptr});
    table->appendCol({ModelHefe::ColAlternativen, true, true, 200, nullptr});
    table->appendCol({ModelHefe::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->appendCol({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->appendCol({ModelHefe::ColLink, true, true, 100, new LinkLabelDelegate(table)});
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
    table->appendCol({ModelWeitereZutaten::ColName, true, false, 200, new IngredientNameDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColEinheit, true, false, 100, new ComboBoxDelegate(Einheiten, table)});
    table->appendCol({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate(ZusatzTypname, gSettings->WZTypBackgrounds, table)});
    table->appendCol({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(0, 100, 1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(table)});
    table->appendCol({ModelWeitereZutaten::ColBemerkung, true, true, 200, nullptr});
    table->appendCol({ModelWeitereZutaten::ColEigenschaften, true, true, 200, nullptr});
    table->appendCol({ModelWeitereZutaten::ColAlternativen, true, true, 200, nullptr});
    table->appendCol({ModelWeitereZutaten::ColPreis, true, true, 100, new DoubleSpinBoxDelegate(2, 0.0, std::numeric_limits<double>::max(), 0.1, false, table)});
    table->appendCol({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, table)});
    table->appendCol({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, table)});
    table->appendCol({ModelWeitereZutaten::ColLink, true, true, 100, new LinkLabelDelegate(table)});
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
    table->appendCol({ModelWasser::ColName, true, false, -1, nullptr});
    table->appendCol({ModelWasser::ColRestalkalitaet, true, false, 120, new DoubleSpinBoxDelegate(2, table)});
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

void TabRohstoffe::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        bool on = gSettings->module(Settings::ModuleLagerverwaltung);
        ui->tableMalz->setCol(1, on, false);
        ui->tableMalz->setCol(9, on, on);
        ui->tableMalz->setCol(10, on, on);
        ui->tableHopfen->setCol(1, on, false);
        ui->tableHopfen->setCol(9, on, on);
        ui->tableHopfen->setCol(10, on, on);
        ui->tableHefe->setCol(1, on, false);
        ui->tableHefe->setCol(12, on, on);
        ui->tableHefe->setCol(13, on, on);
        ui->tableWeitereZutaten->setCol(1, on, false);
        ui->tableWeitereZutaten->setCol(10, on, on);
        ui->tableWeitereZutaten->setCol(11, on, on);
    }
    if (modules.testFlag(Settings::ModuleWasseraufbereitung))
    {
        bool on = gSettings->module(Settings::ModuleWasseraufbereitung);
        int index = ui->toolBoxRohstoffe->indexOf(ui->tabWasser);
        if (on)
        {
            if (index < 0)
                ui->toolBoxRohstoffe->addItem(ui->tabWasser, tr("Wasser"));
        }
        else
            ui->toolBoxRohstoffe->removeItem(index);
        ui->tabWasser->setVisible(on);
        ui->tableMalz->setCol(3, on, on);
    }
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        bool on = gSettings->module(Settings::ModulePreiskalkulation);
        ui->tableMalz->setCol(8, on, on);
        ui->tableHopfen->setCol(8, on, on);
        ui->tableHefe->setCol(11, on, on);
        ui->tableWeitereZutaten->setCol(9, on, on);
    }
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
    case 4:
        table = ui->tableWasser;
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
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
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
    case 4:
        addEntry(ui->tableWasser, {{ModelWasser::ColName, tr("Neues Profil")}});
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
    case 4:
        table = ui->tableWasser;
        art = DlgRohstoffVorlage::Art::Wasserprofil;
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
    case 4:
        table = ui->tableWasser;
        break;
    default:
        return;
    }
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    SqlTableModel *sourceModel = static_cast<SqlTableModel*>(model->sourceModel());
    for (const QModelIndex& index : table->selectionModel()->selectedRows())
    {
        int row = index.row();
        QMap<int, QVariant> values = sourceModel->copyValues(model->mapRowToSource(row));
        values.insert(model->fieldIndex("Name"), model->data(row, model->fieldIndex("Name")).toString() + " " + tr("Kopie"));
        if (table != ui->tableWasser)
        {
            values.remove(model->fieldIndex("Menge"));
            values.remove(model->fieldIndex("Eingelagert"));
            values.remove(model->fieldIndex("Mindesthaltbar"));
        }
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
    case 4:
        table = ui->tableWasser;
        break;
    default:
        return;
    }
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    QModelIndexList indices = table->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        bool del = true;
        if (table == ui->tableWasser)
        {
            QString name = model->data(index.row(), ModelWasser::ColName).toString();
            int ret = QMessageBox::question(this, tr("Wasserprofil löschen?"),
                                            tr("Soll das Wasserprofil \"%1\" gelöscht werden?").arg(name));
            if (ret != QMessageBox::Yes)
                del = false;
        }
        else
        {
            if (model->data(index.row(), model->fieldIndex("InGebrauch")).toBool())
            {
                QStringList liste = model->data(index.row(), model->fieldIndex("InGebrauchListe")).toStringList();
                QString strListe = "\n\n- " + liste.join("\n- ");
                int ret = QMessageBox::question(this, tr("Rohstoff wird verwendet"),
                                                tr("Dieser Rohstoff wird in einem noch nicht gebrauten Sud verwendet. Soll er trotzdem gelöscht werden?") + strListe);
                if (ret != QMessageBox::Yes)
                    del = false;
            }
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
    ui->buttonNeuVorlageObrama->setEnabled(index != 4);
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
        ProxyModel *proxy = static_cast<ProxyModel*>(ui->tableWasser->model());
        mRowWasser = proxy->mapRowToSource(selected.indexes()[0].row());
        updateWasser();
    }
}

void TabRohstoffe::updateWasser()
{
    ui->lblWasserprofil->setText(dataWasser(ModelWasser::ColName).toString());
    //if (!ui->tbCalciumMg->hasFocus())
        ui->tbCalciumMg->setValue(dataWasser(ModelWasser::ColCalcium).toDouble());
    //if (!ui->tbCalciumMmol->hasFocus())
        ui->tbCalciumMmol->setValue(dataWasser(ModelWasser::ColCalciumMmol).toDouble());
    //if (!ui->tbCalciumHaerte->hasFocus())
        ui->tbCalciumHaerte->setValue(dataWasser(ModelWasser::ColCalciumHaerte).toDouble());
    //if (!ui->tbMagnesiumMg->hasFocus())
        ui->tbMagnesiumMg->setValue(dataWasser(ModelWasser::ColMagnesium).toDouble());
    //if (!ui->tbMagnesiumMmol->hasFocus())
        ui->tbMagnesiumMmol->setValue(dataWasser(ModelWasser::ColMagnesiumMmol).toDouble());
    //if (!ui->tbMagnesiumHaerte->hasFocus())
        ui->tbMagnesiumHaerte->setValue(dataWasser(ModelWasser::ColMagnesiumHaerte).toDouble());
    //if (!ui->tbHydrogencarbonatMg->hasFocus())
        ui->tbHydrogencarbonatMg->setValue(dataWasser(ModelWasser::ColHydrogencarbonat).toDouble());
    //if (!ui->tbHydrogencarbonatMmol->hasFocus())
        ui->tbHydrogencarbonatMmol->setValue(dataWasser(ModelWasser::ColHydrogencarbonatMmol).toDouble());
    //if (!ui->tbHydrogencarbonatHaerte->hasFocus())
        ui->tbHydrogencarbonatHaerte->setValue(dataWasser(ModelWasser::ColCarbonatHaerte).toDouble());
    //if (!ui->tbSulfatMg->hasFocus())
        ui->tbSulfatMg->setValue(dataWasser(ModelWasser::ColSulfat).toDouble());
    //if (!ui->tbSulfatMmol->hasFocus())
        ui->tbSulfatMmol->setValue(dataWasser(ModelWasser::ColSulfatMmol).toDouble());
    //if (!ui->tbChloridMg->hasFocus())
        ui->tbChloridMg->setValue(dataWasser(ModelWasser::ColChlorid).toDouble());
    //if (!ui->tbChloridMmol->hasFocus())
        ui->tbChloridMmol->setValue(dataWasser(ModelWasser::ColChloridMmol).toDouble());
    //if (!ui->tbNatriumMg->hasFocus())
        ui->tbNatriumMg->setValue(dataWasser(ModelWasser::ColNatrium).toDouble());
    //if (!ui->tbNatriumMmol->hasFocus())
        ui->tbNatriumMmol->setValue(dataWasser(ModelWasser::ColNatriumMmol).toDouble());
    //if (!ui->tbRestalkalitaetAdd->hasFocus())
        ui->tbRestalkalitaetAdd->setValue(dataWasser(ModelWasser::ColRestalkalitaetAdd).toDouble());
    ui->tbRestalkalitaet->setValue(dataWasser(ModelWasser::ColRestalkalitaet).toDouble());
    if (!ui->tbBemerkung->hasFocus())
        ui->tbBemerkung->setText(dataWasser(ModelWasser::ColBemerkung).toString());
}

void TabRohstoffe::on_tbCalciumMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColCalcium).toDouble();
    if (prevValue != ui->tbCalciumMg->value())
        setDataWasser(ModelWasser::ColCalcium, ui->tbCalciumMg->value());
}

void TabRohstoffe::on_tbCalciumMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColCalciumMmol).toDouble();
    if (prevValue != ui->tbCalciumMmol->value())
        setDataWasser(ModelWasser::ColCalciumMmol, ui->tbCalciumMmol->value());
}

void TabRohstoffe::on_tbCalciumHaerte_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColCalciumHaerte).toDouble();
    if (prevValue != ui->tbCalciumHaerte->value())
        setDataWasser(ModelWasser::ColCalciumHaerte, ui->tbCalciumHaerte->value());
}

void TabRohstoffe::on_tbMagnesiumMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColMagnesium).toDouble();
    if (prevValue != ui->tbMagnesiumMg->value())
        setDataWasser(ModelWasser::ColMagnesium, ui->tbMagnesiumMg->value());
}

void TabRohstoffe::on_tbMagnesiumMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColMagnesiumMmol).toDouble();
    if (prevValue != ui->tbMagnesiumMmol->value())
        setDataWasser(ModelWasser::ColMagnesiumMmol, ui->tbMagnesiumMmol->value());
}

void TabRohstoffe::on_tbMagnesiumHaerte_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColMagnesiumHaerte).toDouble();
    if (prevValue != ui->tbMagnesiumHaerte->value())
        setDataWasser(ModelWasser::ColMagnesiumHaerte, ui->tbMagnesiumHaerte->value());
}

void TabRohstoffe::on_tbHydrogencarbonatMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColHydrogencarbonat).toDouble();
    if (prevValue != ui->tbHydrogencarbonatMg->value())
        setDataWasser(ModelWasser::ColHydrogencarbonat, ui->tbHydrogencarbonatMg->value());
}

void TabRohstoffe::on_tbHydrogencarbonatMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColHydrogencarbonatMmol).toDouble();
    if (prevValue != ui->tbHydrogencarbonatMmol->value())
        setDataWasser(ModelWasser::ColHydrogencarbonatMmol, ui->tbHydrogencarbonatMmol->value());
}

void TabRohstoffe::on_tbHydrogencarbonatHaerte_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColCarbonatHaerte).toDouble();
    if (prevValue != ui->tbHydrogencarbonatHaerte->value())
        setDataWasser(ModelWasser::ColCarbonatHaerte, ui->tbHydrogencarbonatHaerte->value());
}

void TabRohstoffe::on_tbSulfatMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColSulfat).toDouble();
    if (prevValue != ui->tbSulfatMg->value())
        setDataWasser(ModelWasser::ColSulfat, ui->tbSulfatMg->value());
}

void TabRohstoffe::on_tbSulfatMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColSulfatMmol).toDouble();
    if (prevValue != ui->tbSulfatMmol->value())
        setDataWasser(ModelWasser::ColSulfatMmol, ui->tbSulfatMmol->value());
}

void TabRohstoffe::on_tbChloridMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColChlorid).toDouble();
    if (prevValue != ui->tbChloridMg->value())
        setDataWasser(ModelWasser::ColChlorid, ui->tbChloridMg->value());
}

void TabRohstoffe::on_tbChloridMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColChloridMmol).toDouble();
    if (prevValue != ui->tbChloridMmol->value())
        setDataWasser(ModelWasser::ColChloridMmol, ui->tbChloridMmol->value());
}

void TabRohstoffe::on_tbNatriumMg_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColNatrium).toDouble();
    if (prevValue != ui->tbNatriumMg->value())
        setDataWasser(ModelWasser::ColNatrium, ui->tbNatriumMg->value());
}

void TabRohstoffe::on_tbNatriumMmol_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColNatriumMmol).toDouble();
    if (prevValue != ui->tbNatriumMmol->value())
        setDataWasser(ModelWasser::ColNatriumMmol, ui->tbNatriumMmol->value());
}

void TabRohstoffe::on_tbRestalkalitaetAdd_editingFinished()
{
    double prevValue = dataWasser(ModelWasser::ColRestalkalitaetAdd).toDouble();
    if (prevValue != ui->tbRestalkalitaetAdd->value())
        setDataWasser(ModelWasser::ColRestalkalitaetAdd, ui->tbRestalkalitaetAdd->value());
}

void TabRohstoffe::on_tbBemerkung_textChanged()
{
    if (ui->tbBemerkung->hasFocus())
        setDataWasser(ModelWasser::ColBemerkung, ui->tbBemerkung->toPlainText());
}
