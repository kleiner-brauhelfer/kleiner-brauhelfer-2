#include "dlgrohstoffauswahl.h"
#include "proxymodelrohstoff.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/ebcdelegate.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"
#include "mainwindow.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

void DlgRohstoffAuswahl::restoreView()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableStateMalz", QByteArray());
    gSettings->setValue("tableStateHopfen", QByteArray());
    gSettings->setValue("tableStateHefe", QByteArray());
    gSettings->setValue("tableStateWeitereZutaten", QByteArray());
    gSettings->endGroup();
}

DlgRohstoffAuswahl::DlgRohstoffAuswahl(Brauhelfer::RohstoffTyp rohstoff, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRohstoffAuswahl),
    mRohstoff(rohstoff)
{
    ui->setupUi(this);
    adjustSize();
    ui->tableView->setFocus();

    gSettings->beginGroup(staticMetaObject.className());

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
        table->appendCol({ModelMalz::ColName, true, false, 100, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            table->appendCol({ModelMalz::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, ui->tableView)});
        table->appendCol({ModelMalz::ColFarbe, true, true, 100, new EbcDelegate(ui->tableView)});
        table->appendCol({ModelMalz::ColMaxProzent, true, true, 100, new SpinBoxDelegate(ui->tableView)});
        table->appendCol({ModelMalz::ColBemerkung, true, true, 200, nullptr});
        table->appendCol({ModelMalz::ColEigenschaften, true, true, 200, nullptr});
        table->appendCol({ModelMalz::ColAlternativen, true, true, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            table->appendCol({ModelMalz::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
            table->appendCol({ModelMalz::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        }
        table->build();
        table->restoreState(gSettings->value("tableStateMalz").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Hopfen:
        proxy->setSourceModel(bh->modelHopfen());
        table->setModel(proxy);
        mNameCol = ModelHopfen::ColName;
        table->appendCol({ModelHopfen::ColName, true, false, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            table->appendCol({ModelHopfen::ColMenge, true, false, 100, new SpinBoxDelegate(ui->tableView)});
        table->appendCol({ModelHopfen::ColAlpha, true, true, 100, new DoubleSpinBoxDelegate(1, ui->tableView)});
        table->appendCol({ModelHopfen::ColPellets, true, true, 100, new CheckBoxDelegate(table)});
        table->appendCol({ModelHopfen::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::HopfenTypname, gSettings->HopfenTypBackgrounds, ui->tableView)});
        table->appendCol({ModelHopfen::ColBemerkung, true, true, 200, nullptr});
        table->appendCol({ModelHopfen::ColEigenschaften, true, true, 200, nullptr});
        table->appendCol({ModelHopfen::ColAlternativen, true, true, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            table->appendCol({ModelHopfen::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
            table->appendCol({ModelHopfen::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        }
        table->build();
        table->restoreState(gSettings->value("tableStateHopfen").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Hefe:
        proxy->setSourceModel(bh->modelHefe());
        table->setModel(proxy);
        mNameCol = ModelHefe::ColName;
        table->appendCol({ModelHefe::ColName, true, false, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            table->appendCol({ModelHefe::ColMenge, true, false, 100, new SpinBoxDelegate(ui->tableView)});
        table->appendCol({ModelHefe::ColTypOGUG, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypname, gSettings->HefeTypOgUgBackgrounds, ui->tableView)});
        table->appendCol({ModelHefe::ColTypTrFl, true, true, 100, new ComboBoxDelegate(MainWindow::HefeTypFlTrName, gSettings->HefeTypTrFlBackgrounds, ui->tableView)});
        table->appendCol({ModelHefe::ColSedimentation, true, true, 100, nullptr});
        table->appendCol({ModelHefe::ColEVG, true, true, 100, nullptr});
        table->appendCol({ModelHefe::ColTemperatur, true, true, 100, nullptr});
        table->appendCol({ModelHefe::ColBemerkung, true, true, 200, nullptr});
        table->appendCol({ModelHefe::ColEigenschaften, true, true, 200, nullptr});
        table->appendCol({ModelHefe::ColAlternativen, true, true, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            table->appendCol({ModelHefe::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
            table->appendCol({ModelHefe::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        }
        table->build();
        table->restoreState(gSettings->value("tableStateHefe").toByteArray());
        break;

    case Brauhelfer::RohstoffTyp::Zusatz:
        proxy->setSourceModel(bh->modelWeitereZutaten());
        table->setModel(proxy);
        mNameCol = ModelWeitereZutaten::ColName;
        table->appendCol({ModelWeitereZutaten::ColName, true, false, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
            table->appendCol({ModelWeitereZutaten::ColMenge, true, false, 100, new DoubleSpinBoxDelegate(2, ui->tableView)});
        table->appendCol({ModelWeitereZutaten::ColEinheit, true, true, 100, new ComboBoxDelegate({"kg", "g", "mg", tr("Stk.")}, ui->tableView)});
        table->appendCol({ModelWeitereZutaten::ColTyp, true, true, 100, new ComboBoxDelegate(MainWindow::ZusatzTypname, gSettings->WZTypBackgrounds, ui->tableView)});
        table->appendCol({ModelWeitereZutaten::ColAusbeute, true, true, 100, new SpinBoxDelegate(ui->tableView)});
        table->appendCol({ModelWeitereZutaten::ColFarbe, true, true, 100, new EbcDelegate(ui->tableView)});
        table->appendCol({ModelWeitereZutaten::ColBemerkung, true, true, 200, nullptr});
        table->appendCol({ModelWeitereZutaten::ColEigenschaften, true, true, 200, nullptr});
        table->appendCol({ModelWeitereZutaten::ColAlternativen, true, true, 200, nullptr});
        if (gSettings->isModuleEnabled(Settings::ModuleLagerverwaltung))
        {
            table->appendCol({ModelWeitereZutaten::ColEingelagert, true, true, 100, new DateDelegate(false, false, ui->tableView)});
            table->appendCol({ModelWeitereZutaten::ColMindesthaltbar, true, true, 100, new DateDelegate(true, false, ui->tableView)});
        }
        table->build();
        table->restoreState(gSettings->value("tableStateWeitereZutaten").toByteArray());
        break;
    }
    table->setDefaultContextMenu();

    proxy->setFilterKeyColumn(mNameCol);

    if (gSettings->value("filter", 0).toInt() == 1)
    {
        ui->radioButtonVorhanden->setChecked(true);
        on_radioButtonVorhanden_clicked();
    }

    gSettings->endGroup();
}

DlgRohstoffAuswahl::~DlgRohstoffAuswahl()
{
    gSettings->beginGroup(staticMetaObject.className());
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
    gSettings->setValue("filter", ui->radioButtonVorhanden->isChecked() ? 1 : 0);
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
    proxy->setFilterString(text);
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
