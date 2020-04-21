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
#include "tabrohstoffe.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

void DlgRohstoffAuswahl::restoreView(bool full)
{
    Q_UNUSED(full)
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

    ProxyModel *proxy = new ProxyModelRohstoff(this);
    TableView *table = ui->tableView;

    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    table->verticalHeader()->setMinimumSectionSize(table->verticalHeader()->defaultSectionSize());

    switch (mRohstoff)
    {
    case Brauhelfer::RohstoffTyp::Malz:
        proxy->setSourceModel(bh->modelMalz());
        table->setModel(proxy);
        mNameCol = ModelMalz::ColName;
        table->cols.append({ModelMalz::ColName, true, false, 100, nullptr});
        table->cols.append({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, ui->tableView)});
        table->cols.append({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(ui->tableView)});
        table->cols.append({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(ui->tableView)});
        table->cols.append({ModelMalz::ColBemerkung, true, true, 200, nullptr});
        table->cols.append({ModelMalz::ColEingenschaften, true, true, 200, nullptr});
        table->cols.append({ModelMalz::ColAlternativen, true, true, 200, nullptr});
        table->cols.append({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
        table->cols.append({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        table->build();
        table->horizontalHeader()->restoreState(gSettings->value("tableStateMalz").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Hopfen:
        proxy->setSourceModel(bh->modelHopfen());
        table->setModel(proxy);
        mNameCol = ModelHopfen::ColName;
        table->cols.append({ModelHopfen::ColName, true, false, 200, nullptr});
        table->cols.append({ModelHopfen::ColMenge, true, false, 100, new SpinBoxDelegate(ui->tableView)});
        table->cols.append({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, ui->tableView)});
        table->cols.append({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
        table->cols.append({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(TabRohstoffe::HopfenTypname, gSettings->HopfenTypBackgrounds, ui->tableView)});
        table->cols.append({ModelHopfen::ColBemerkung, true, true, 200, nullptr});
        table->cols.append({ModelHopfen::ColEigenschaften, true, true, 200, nullptr});
        table->cols.append({ModelHopfen::ColAlternativen, true, true, 200, nullptr});
        table->cols.append({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
        table->cols.append({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        table->build();
        table->horizontalHeader()->restoreState(gSettings->value("tableStateHopfen").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Hefe:
        proxy->setSourceModel(bh->modelHefe());
        table->setModel(proxy);
        mNameCol = ModelHefe::ColName;
        table->cols.append({ModelHefe::ColName, true, false, 200, nullptr});
        table->cols.append({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(ui->tableView)});
        table->cols.append({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(TabRohstoffe::HefeTypname, gSettings->HefeTypOgUgBackgrounds, ui->tableView)});
        table->cols.append({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(TabRohstoffe::HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, ui->tableView)});
        table->cols.append({ModelHefe::ColSedimentation, true, true, 100, nullptr});
        table->cols.append({ModelHefe::ColEVG, true, true, 100, nullptr});
        table->cols.append({ModelHefe::ColTemperatur, true, true, 100, nullptr});
        table->cols.append({ModelHefe::ColBemerkung, true, true, 200, nullptr});
        table->cols.append({ModelHefe::ColEigenschaften, true, true, 200, nullptr});
        table->cols.append({ModelHefe::ColAlternativen, true, true, 200, nullptr});
        table->cols.append({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
        table->cols.append({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        table->build();
        table->horizontalHeader()->restoreState(gSettings->value("tableStateHefe").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Zusatz:
        proxy->setSourceModel(bh->modelWeitereZutaten());
        table->setModel(proxy);
        mNameCol = ModelWeitereZutaten::ColName;
        table->cols.append({ModelWeitereZutaten::ColName, true, false, 200, nullptr});
        table->cols.append({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColEinheit, true, true, 100, new ComboBoxDelegate({tr("kg"), tr("g"), tr("mg"), tr("Stk.")}, ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate({tr("Honig"), tr("Zucker"), tr("Gewürz"), tr("Frucht"), tr("Sonstiges")}, gSettings->WZTypBackgrounds, ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColBemerkung, true, true, 200, nullptr});
        table->cols.append({ModelWeitereZutaten::ColEigenschaften, true, true, 200, nullptr});
        table->cols.append({ModelWeitereZutaten::ColAlternativen, true, true, 200, nullptr});
        table->cols.append({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
        table->cols.append({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        table->build();
        table->horizontalHeader()->restoreState(gSettings->value("tableStateWeitereZutaten").toByteArray());
        break;
    }
    table->setDefaultContextMenu();

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
