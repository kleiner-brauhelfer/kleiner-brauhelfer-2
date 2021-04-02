#include "dlguebernahmerezept.h"
#include "ui_dlguebernahmerezept.h"
#include "model/spinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

class ProxyModelZusatz : public ProxyModel
{
public:
    explicit ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt zeitpunkt,
                              Brauhelfer::ZusatzTyp typ, bool negate, QObject *parent = nullptr) :
        ProxyModel(parent),
        mZeitpunkt(static_cast<int>(zeitpunkt)),
        mTyp(static_cast<int>(typ)),
        mNegateTyp(negate)
    {
    }
    explicit ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt zeitpunkt,
                               QObject *parent = nullptr) :
        ProxyModel(parent),
        mZeitpunkt(static_cast<int>(zeitpunkt)),
        mTyp(-1),
        mNegateTyp(false)
    {
    }
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const Q_DECL_OVERRIDE
    {
        bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
        if (accept)
        {
            QModelIndex idx = sourceModel()->index(source_row, ModelWeitereZutatenGaben::ColZeitpunkt, source_parent);
            if (idx.isValid())
            {
                accept = sourceModel()->data(idx).toInt() == mZeitpunkt;
                if (accept && mTyp != -1)
                {
                    idx = sourceModel()->index(source_row, ModelWeitereZutatenGaben::ColTyp, source_parent);
                    if (idx.isValid())
                    {
                        if (mNegateTyp)
                            accept = sourceModel()->data(idx).toInt() != mTyp;
                        else
                            accept = sourceModel()->data(idx).toInt() == mTyp;
                    }
                }
            }
        }
        return accept;
    }
private:
    const int mZeitpunkt;
    const int mTyp;
    const bool mNegateTyp;
};

DlgUebernahmeRezept::DlgUebernahmeRezept(Art art, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgUebernahmeRezept),
    mArt(art),
    mSudId(-1)
{
    ui->setupUi(this);

    ProxyModel* model = nullptr;
    switch (mArt)
    {
    case Malz:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelMalzschuettung());
        model->setFilterKeyColumn(ModelMalzschuettung::ColSudID);
        ui->tableViewItem->appendCol({ModelMalzschuettung::ColName, true, false, -1, nullptr});
        break;
    case Hopfen:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelHopfengaben());
        model->setFilterKeyColumn(ModelHopfengaben::ColSudID);
        ui->tableViewItem->appendCol({ModelHopfengaben::ColName, true, false, -1, nullptr});
        break;
    case Hefe:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelHefegaben());
        model->setFilterKeyColumn(ModelHefegaben::ColSudID);
        ui->tableViewItem->appendCol({ModelHefegaben::ColName, true, false, -1, nullptr});
        break;
    case WZutaten:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->appendCol({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case Maischplan:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelRasten());
        model->setFilterKeyColumn(ModelRasten::ColSudID);
        ui->tableViewItem->appendCol({ModelRasten::ColName, true, false, -1, nullptr});
        break;
    case Wasseraufbereitung:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelWasseraufbereitung());
        model->setFilterKeyColumn(ModelWasseraufbereitung::ColSudID);
        ui->tableViewItem->appendCol({ModelWasseraufbereitung::ColName, true, false, -1, nullptr});
        break;
    case WZutatenMaischen:
        model = new ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt::Maischen, this);
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->appendCol({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case WZutatenKochen:
        model = new ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt::Kochen, this);
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->appendCol({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case WZutatenGaerung:
        model = new ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt::Gaerung, Brauhelfer::ZusatzTyp::Hopfen, true, this);
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->appendCol({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case HopfenGaerung:
        model = new ProxyModelZusatz(Brauhelfer::ZusatzZeitpunkt::Gaerung, Brauhelfer::ZusatzTyp::Hopfen, false, this);
        model->setSourceModel(bh->modelWeitereZutatenGaben());
        model->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
        ui->tableViewItem->appendCol({ModelWeitereZutatenGaben::ColName, true, false, -1, nullptr});
        break;
    case Tags:
        model = new ProxyModel(this);
        model->setSourceModel(bh->modelTags());
        model->setFilterKeyColumn(ModelTags::ColSudID);
        ui->tableViewItem->appendCol({ModelTags::ColKey, true, false, -1, nullptr});
        ui->tableViewItem->appendCol({ModelTags::ColValue, true, false, -1, nullptr});
        break;
    }
    if (model)
    {
        model->setFilterRegularExpression(QString("^%1$").arg(mSudId));
        ui->tableViewItem->setModel(model);
        ui->tableViewItem->build();
    }

    model = new ProxyModel(this);
    model->setSourceModel(bh->modelSud());
    ui->tableViewSud->setModel(model);
    ui->tableViewSud->appendCol({ModelSud::ColSudname, true, false, -1, nullptr});
    ui->tableViewSud->appendCol({ModelSud::ColSudnummer, true, false, 80, new SpinBoxDelegate(ui->tableViewSud)});
    ui->tableViewSud->build();
    connect(ui->tableViewSud->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(tableViewSud_selectionChanged()));

    adjustSize();
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableViewSud->restoreState(gSettings->value("tableStateSud").toByteArray());
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    gSettings->endGroup();

    ui->tableViewSud->selectRow(0);
}

DlgUebernahmeRezept::~DlgUebernahmeRezept()
{
    gSettings->beginGroup(staticMetaObject.className());
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
        mSudId = static_cast<ProxyModel*>(ui->tableViewSud->model())->data(indexes[0].row(), ModelSud::ColID).toInt();
    else
        mSudId = -1;
    static_cast<ProxyModel*>(ui->tableViewItem->model())->setFilterRegularExpression(QString("^%1$").arg(mSudId));
}

void DlgUebernahmeRezept::on_tableViewSud_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    accept();
}
