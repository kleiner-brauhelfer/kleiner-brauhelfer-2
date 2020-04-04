#include "dlgrohstoffauswahl.h"
#include "proxymodelrohstoff.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/ingredientnamedelegate.h"
#include "model/linklabeldelegate.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

void DlgRohstoffAuswahl::restoreView()
{
    gSettings->beginGroup("DlgRohstoffAuswahl");
    gSettings->setValue("tableStateMalz", QByteArray());
    gSettings->setValue("tableStateHopfen", QByteArray());
    gSettings->setValue("tableStateHefe", QByteArray());
    gSettings->setValue("tableStateWeitereZutaten", QByteArray());
    gSettings->endGroup();
}

DlgRohstoffAuswahl::DlgRohstoffAuswahl(Brauhelfer::RohstoffTyp rohstoff, QWidget *parent) :
	QDialog(parent),
    ui(new Ui::DlgRohstoffAuswahl),
    mRohstoff(rohstoff)
{
    ui->setupUi(this);
    ui->tableView->setFocus();

    gSettings->beginGroup("DlgRohstoffAuswahl");

    int col;
    SqlTableModel *model;
    QHeaderView *header = ui->tableView->horizontalHeader();
    ProxyModel *proxy = new ProxyModelRohstoff(this);
    ComboBoxDelegate *comboBox;

    switch (mRohstoff)
    {
    case Brauhelfer::RohstoffTyp::Malz:
        model = bh->modelMalz();
        proxy->setSourceModel(model);
        ui->tableView->setModel(proxy);
        mNameCol = ModelMalz::ColBeschreibung;

        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);

        col = ModelMalz::ColBeschreibung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 0);

        col = ModelMalz::ColMenge;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 1);

        col = ModelMalz::ColFarbe;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new EbcDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 2);

        col = ModelMalz::ColMaxProzent;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new SpinBoxDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 3);

        col = ModelMalz::ColBemerkung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 4);

        col = ModelMalz::ColAnwendung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 5);

        col = ModelMalz::ColEingelagert;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(false, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 6);

        col = ModelMalz::ColMindesthaltbar;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(true, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 7);

        header->restoreState(gSettings->value("tableStateMalz").toByteArray());

        break;

    case Brauhelfer::RohstoffTyp::Hopfen:
        model = bh->modelHopfen();
        proxy->setSourceModel(model);
        ui->tableView->setModel(proxy);
        mNameCol = ModelHopfen::ColBeschreibung;

        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);

        col = ModelHopfen::ColBeschreibung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 0);

        col = ModelHopfen::ColMenge;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new SpinBoxDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 1);

        col = ModelHopfen::ColAlpha;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(1, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 2);

        col = ModelHopfen::ColBemerkung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 3);

        col = ModelHopfen::ColEigenschaften;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 4);

        col = ModelHopfen::ColTyp;
        ui->tableView->setColumnHidden(col, false);
        comboBox = new ComboBoxDelegate({"", tr("aroma"), tr("bitter"), tr("universal")}, ui->tableView);
        comboBox->setColors(gSettings->HopfenTypBackgrounds);
        ui->tableView->setItemDelegateForColumn(col, comboBox);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 5);

        col = ModelHopfen::ColEingelagert;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(false, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 6);

        col = ModelHopfen::ColMindesthaltbar;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(true, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 7);

        header->restoreState(gSettings->value("tableStateHopfen").toByteArray());

        break;

    case Brauhelfer::RohstoffTyp::Hefe:
        model = bh->modelHefe();
        proxy->setSourceModel(model);
        ui->tableView->setModel(proxy);
        mNameCol = ModelHefe::ColBeschreibung;

        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);

        col = ModelHefe::ColBeschreibung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 0);

        col = ModelHefe::ColMenge;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new SpinBoxDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 1);

        col = ModelHefe::ColBemerkung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 2);

        col = ModelHefe::ColEigenschaften;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 3);

        col = ModelHefe::ColTypOGUG;
        ui->tableView->setColumnHidden(col, false);
        comboBox = new ComboBoxDelegate({"", tr("obergärig"), tr("untergärig")}, ui->tableView);
        comboBox->setColors(gSettings->HefeTypOgUgBackgrounds);
        ui->tableView->setItemDelegateForColumn(col, comboBox);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 4);

        col = ModelHefe::ColTypTrFl;
        ui->tableView->setColumnHidden(col, false);
        comboBox = new ComboBoxDelegate({"", tr("trocken"), tr("flüssig")}, ui->tableView);
        comboBox->setColors(gSettings->HefeTypTrFlBackgrounds);
        ui->tableView->setItemDelegateForColumn(col, comboBox);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 5);

        col = ModelHefe::ColSED;
        ui->tableView->setColumnHidden(col, false);
        comboBox = new ComboBoxDelegate({"", tr("hoch"), tr("mittel"), tr("niedrig")}, ui->tableView);
        comboBox->setColors(gSettings->HefeSedBackgrounds);
        ui->tableView->setItemDelegateForColumn(col, comboBox);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 6);

        col = ModelHefe::ColEVG;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 7);

        col = ModelHefe::ColTemperatur;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 8);

        col = ModelHefe::ColEingelagert;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(false, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 9);

        col = ModelHefe::ColMindesthaltbar;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(true, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 10);

        header->restoreState(gSettings->value("tableStateHefe").toByteArray());

        break;

    case Brauhelfer::RohstoffTyp::Zusatz:
        model = bh->modelWeitereZutaten();
        proxy->setSourceModel(model);
        ui->tableView->setModel(proxy);
        mNameCol = ModelWeitereZutaten::ColBeschreibung;

        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);

        col = ModelWeitereZutaten::ColBeschreibung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 0);

        col = ModelWeitereZutaten::ColMenge;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(2, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 1);

        col = ModelWeitereZutaten::ColEinheiten;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate({tr("kg"), tr("g"), tr("mg"), tr("Stk.")}, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 2);

        col = ModelWeitereZutaten::ColTyp;
        ui->tableView->setColumnHidden(col, false);
        comboBox = new ComboBoxDelegate({tr("Honig"), tr("Zucker"), tr("Gewürz"), tr("Frucht"), tr("Sonstiges")}, ui->tableView);
        comboBox->setColors(gSettings->WZTypBackgrounds);
        ui->tableView->setItemDelegateForColumn(col, comboBox);
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 3);

        col = ModelWeitereZutaten::ColAusbeute;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new SpinBoxDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 4);

        col = ModelWeitereZutaten::ColEBC;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new EbcDelegate(ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 5);

        col = ModelWeitereZutaten::ColBemerkung;
        ui->tableView->setColumnHidden(col, false);
        header->resizeSection(col, 200);
        header->moveSection(header->visualIndex(col), 6);

        col = ModelWeitereZutaten::ColEingelagert;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(false, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 7);

        col = ModelWeitereZutaten::ColMindesthaltbar;
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new DateDelegate(true, false, ui->tableView));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), 8);

        header->restoreState(gSettings->value("tableStateWeitereZutaten").toByteArray());

        break;
    }

    proxy->setFilterKeyColumn(mNameCol);
    proxy->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    resize(gSettings->value("size").toSize());

    gSettings->endGroup();
}

DlgRohstoffAuswahl::~DlgRohstoffAuswahl()
{
    gSettings->beginGroup("DlgRohstoffAuswahl");
    switch (mRohstoff)
    {
    case Brauhelfer::RohstoffTyp::Malz:
        gSettings->setValue("tableStateMalz", ui->tableView->horizontalHeader()->saveState());
        break;
    case Brauhelfer::RohstoffTyp::Hopfen:
        gSettings->setValue("tableStateHopfen", ui->tableView->horizontalHeader()->saveState());
        break;
    case Brauhelfer::RohstoffTyp::Hefe:
        gSettings->setValue("tableStateHefe", ui->tableView->horizontalHeader()->saveState());
        break;
    case Brauhelfer::RohstoffTyp::Zusatz:
        gSettings->setValue("tableStateWeitereZutaten", ui->tableView->horizontalHeader()->saveState());
        break;
    }
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

void DlgRohstoffAuswahl::select(const QString &name)
{
    ProxyModel* proxy = qobject_cast<ProxyModel*>(ui->tableView->model());
    SqlTableModel* model = static_cast<SqlTableModel*>(proxy->sourceModel());
    int row = proxy->mapRowFromSource(model->getRowWithValue(mNameCol, name));
    if (row >= 0)
        ui->tableView->selectRow(row);
}

QString DlgRohstoffAuswahl::name() const
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if (selection.count() > 0)
        return ui->tableView->model()->data(selection[0].sibling(selection[0].row(), mNameCol)).toString();
    return QString();
}

void DlgRohstoffAuswahl::on_radioButtonAlle_clicked()
{
    ProxyModelRohstoff* proxy = static_cast<ProxyModelRohstoff*>(ui->tableView->model());
    proxy->setFilter(ProxyModelRohstoff::Filter::Alle);
}

void DlgRohstoffAuswahl::on_radioButtonVorhanden_clicked()
{
    ProxyModelRohstoff* proxy = static_cast<ProxyModelRohstoff*>(ui->tableView->model());
    proxy->setFilter(ProxyModelRohstoff::Filter::Vorhanden);
}

void DlgRohstoffAuswahl::on_lineEditFilter_textChanged(const QString &text)
{
    ProxyModel* proxy = static_cast<ProxyModel*>(ui->tableView->model());
    proxy->setFilterFixedString(text);
}

void DlgRohstoffAuswahl::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    accept();
}

void DlgRohstoffAuswahl::on_buttonBox_accepted()
{
    if (name().isEmpty())
        reject();
}
