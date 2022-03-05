#include "dlgrohstoffe.h"
#include "ui_dlgrohstoffe.h"
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

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Brauhelfer* bh;
extern Settings* gSettings;

QStringList DlgRohstoffe::HopfenTypname = {
    "",
    tr("aroma"),
    tr("bitter"),
    tr("universal")
};

QStringList DlgRohstoffe::HefeTypname = {
    "",
    tr("obergärig"),
    tr("untergärig")
};

QStringList DlgRohstoffe::HefeTypFlTrName = {
    "",
    tr("trocken"),
    tr("flüssig")
};

QStringList DlgRohstoffe::ZusatzTypname = {
    tr("Honig"),
    tr("Zucker"),
    tr("Gewürz"),
    tr("Frucht"),
    tr("Sonstiges"),
    tr("Kraut"),
    tr("Wasseraufbereitung"),
    tr("Klärmittel")
};

QStringList DlgRohstoffe::Einheiten = {
    tr("kg"),
    tr("g"),
    tr("mg"),
    tr("Stk."),
    tr("l"),
    tr("ml")
};

DlgRohstoffe* DlgRohstoffe::Dialog = nullptr;

QStringList DlgRohstoffe::list_tr(const QStringList& list)
{
   QStringList result;
   for (const QString& str : list)
       result.append(tr(str.toStdString().c_str()));
   return result;
}

DlgRohstoffe::DlgRohstoffe(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRohstoffe)
{
    QPalette pal;

    HopfenTypname = list_tr(HopfenTypname);
    HefeTypname = list_tr(HefeTypname);
    HefeTypFlTrName = list_tr(HefeTypFlTrName);
    ZusatzTypname = list_tr(ZusatzTypname);
    Einheiten = list_tr(Einheiten);

    ui->setupUi(this);

    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionNeu);
    menu->addSeparator();
    menu->addAction(ui->actionNeuVorlage);
    menu->addAction(ui->actionNeuObrama);
    menu->addSeparator();
    menu->addAction(ui->actionNeuKopie);
    ui->buttonAdd->setMenu(menu);
    connect(ui->actionNeu, SIGNAL(triggered()), this, SLOT(buttonAdd_clicked()));
    connect(ui->actionNeuVorlage, SIGNAL(triggered()), this, SLOT(buttonNeuVorlage_clicked()));
    connect(ui->actionNeuObrama, SIGNAL(triggered()), this, SLOT(buttonNeuVorlageObrama_clicked()));
    connect(ui->actionNeuKopie, SIGNAL(triggered()), this, SLOT(buttonCopy_clicked()));

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

    SqlTableModel *model;
    ProxyModelRohstoff *proxyModel;
    TableView *table;

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
}

DlgRohstoffe::~DlgRohstoffe()
{
    delete ui;
}

void DlgRohstoffe::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableMalzState", ui->tableMalz->horizontalHeader()->saveState());
    gSettings->setValue("tableHopfenState", ui->tableHopfen->horizontalHeader()->saveState());
    gSettings->setValue("tableHefeState", ui->tableHefe->horizontalHeader()->saveState());
    gSettings->setValue("tableWeitereZutatenState", ui->tableWeitereZutaten->horizontalHeader()->saveState());
    int filter = 0;
    if (ui->radioButtonVorhanden->isChecked())
        filter = 1;
    else if (ui->radioButtonInGebrauch->isChecked())
        filter = 2;
    gSettings->setValue("filter", filter);
    gSettings->endGroup();
}

void DlgRohstoffe::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableMalz->restoreState(gSettings->value("tableMalzState").toByteArray());
    ui->tableHopfen->restoreState(gSettings->value("tableHopfenState").toByteArray());
    ui->tableHefe->restoreState(gSettings->value("tableHefeState").toByteArray());
    ui->tableWeitereZutaten->restoreState(gSettings->value("tableWeitereZutatenState").toByteArray());
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
}

void DlgRohstoffe::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("tableMalzState");
    gSettings->remove("tableHopfenState");
    gSettings->remove("tableHefeState");
    gSettings->remove("tableWeitereZutatenState");
    gSettings->endGroup();
}

void DlgRohstoffe::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModuleLagerverwaltung))
    {
        bool on = gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung);
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
        bool on = gSettings->isModuleEnabled(Settings::ModuleWasseraufbereitung);
        ui->tableMalz->setCol(3, on, on);
    }
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        bool on = gSettings->isModuleEnabled(Settings::ModulePreiskalkulation);
        ui->tableMalz->setCol(8, on, on);
        ui->tableHopfen->setCol(8, on, on);
        ui->tableHefe->setCol(11, on, on);
        ui->tableWeitereZutaten->setCol(9, on, on);
    }
}

void DlgRohstoffe::keyPressEvent(QKeyEvent* event)
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
            buttonAdd_clicked();
            break;
        }
    }
}

void DlgRohstoffe::on_tableMalz_clicked(const QModelIndex &index)
{
    if (index.column() == ModelMalz::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableHopfen_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHopfen::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableHefe_clicked(const QModelIndex &index)
{
    if (index.column() == ModelHefe::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::on_tableWeitereZutaten_clicked(const QModelIndex &index)
{
    if (index.column() == ModelWeitereZutaten::ColLink && QApplication::keyboardModifiers() != Qt::NoModifier)
        QDesktopServices::openUrl(QUrl(index.data().toString()));
}

void DlgRohstoffe::addEntry(QTableView *table, const QMap<int, QVariant> &values)
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

void DlgRohstoffe::buttonAdd_clicked()
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

void DlgRohstoffe::buttonNeuVorlage_clicked()
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

void DlgRohstoffe::buttonNeuVorlageObrama_clicked()
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

void DlgRohstoffe::buttonCopy_clicked()
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
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
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

void DlgRohstoffe::on_buttonDelete_clicked()
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
    ProxyModel *model = static_cast<ProxyModel*>(table->model());
    QModelIndexList indices = table->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : qAsConst(indices))
    {
        bool del = true;
        if (model->data(index.row(), model->fieldIndex("InGebrauch")).toBool())
        {
            QStringList liste = model->data(index.row(), model->fieldIndex("InGebrauchListe")).toStringList();
            QString strListe = "\n\n- " + liste.join("\n- ");
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

void DlgRohstoffe::on_radioButtonAlle_clicked()
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

void DlgRohstoffe::on_radioButtonVorhanden_clicked()
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

void DlgRohstoffe::on_radioButtonInGebrauch_clicked()
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

void DlgRohstoffe::on_lineEditFilter_textChanged(const QString &pattern)
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

void DlgRohstoffe::on_toolBoxRohstoffe_currentChanged()
{
    updateLabelNumItems();
}

void DlgRohstoffe::updateLabelNumItems()
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
