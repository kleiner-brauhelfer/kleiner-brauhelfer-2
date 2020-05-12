#include "dlguebernahmerezept.h"
#include "ui_dlguebernahmerezept.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgUebernahmeRezept::DlgUebernahmeRezept(Art art, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgUebernahmeRezept),
    mArt(art),
    mSudId(-1)
{
    ui->setupUi(this);

    ProxyModel* model = new ProxyModel(this);
    switch (mArt)
    {
    case Malz:
        model->setSourceModel(bh->modelMalzschuettung());
        model->setFilterKeyColumn(ModelMalzschuettung::ColSudID);
        ui->tableViewItem->cols.append({ModelMalzschuettung::ColName, true, false, -1, nullptr});
        break;
    case Hopfen:
        model->setSourceModel(bh->modelHopfengaben());
        model->setFilterKeyColumn(ModelHopfengaben::ColSudID);
        ui->tableViewItem->cols.append({ModelHopfengaben::ColName, true, false, -1, nullptr});
        break;
    case Hefe:
        model->setSourceModel(bh->modelHefegaben());
        model->setFilterKeyColumn(ModelHefegaben::ColSudID);
        ui->tableViewItem->cols.append({ModelHefegaben::ColName, true, false, -1, nullptr});
        break;
    case WZutaten:
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->cols.append({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case Maischplan:
        model->setSourceModel(bh->modelRasten());
        model->setFilterKeyColumn(ModelRasten::ColSudID);
        ui->tableViewItem->cols.append({ModelRasten::ColName, true, false, -1, nullptr});
        break;
    }
    model->setFilterRegExp(QString("^%1$").arg(mSudId));
    ui->tableViewItem->setModel(model);
    ui->tableViewItem->build();


    model = new ProxyModel(this);
    model->setSourceModel(bh->modelSud());
    ui->tableViewSud->setModel(model);
    ui->tableViewSud->cols.append({ModelSud::ColSudname, true, false, -1, nullptr});
    ui->tableViewSud->cols.append({ModelSud::ColSudnummer, true, false, 80, new SpinBoxDelegate(ui->tableViewSud)});
    ui->tableViewSud->build();
    connect(ui->tableViewSud->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(tableViewSud_selectionChanged()));

    gSettings->beginGroup("DlgUebernahmeRezept");
    ui->tableViewSud->restoreState(gSettings->value("tableStateSud").toByteArray());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(gSettings->value("size").toSize());
    gSettings->endGroup();

    ui->tableViewSud->selectRow(0);
}

DlgUebernahmeRezept::~DlgUebernahmeRezept()
{
    gSettings->beginGroup("DlgUebernahmeRezept");
    gSettings->setValue("tableStateSud", ui->tableViewSud->horizontalHeader()->saveState());
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
    delete ui;
}

int DlgUebernahmeRezept::sudId() const
{
    return mSudId;
}

void DlgUebernahmeRezept::tableViewSud_selectionChanged()
{
    QModelIndexList indexes = ui->tableViewSud->selectionModel()->selectedRows();
    if (indexes.count() > 0)
        mSudId = bh->modelSud()->data(indexes[0].row(), ModelSud::ColID).toInt();
    else
        mSudId = -1;

    ProxyModel* model = static_cast<ProxyModel*>(ui->tableViewItem->model());
    model->setFilterRegExp(QString("^%1$").arg(mSudId));
}

void DlgUebernahmeRezept::on_tableViewSud_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    accept();
}
