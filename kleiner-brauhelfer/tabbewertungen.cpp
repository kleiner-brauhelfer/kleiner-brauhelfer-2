#include "tabbewertungen.h"
#include "ui_tabbewertungen.h"
#include "model/bewertungtreeproxymodel.h"
#include "model/datetimedelegate.h"
#include "model/ratingdelegate.h"
#include "model/textdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBewertungen::TabBewertungen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBewertungen)
{
    ui->setupUi(this);

    QVector<QVariant> sudNamen;
    for (int row = 0; row < bh->modelSud()->rowCount(); row++)
        sudNamen.append(bh->modelSud()->data(row, ModelSud::ColSudname));

    BewertungTreeProxyModel* model = new BewertungTreeProxyModel(this);
    model->setSourceModel(bh->modelBewertungen());
    model->setGroupping(ModelBewertungen::ColSudName, Qt::DisplayRole);
    model->setCustomGroups(sudNamen);

    QSortFilterProxyModel* modelSort = new QSortFilterProxyModel(this);
    modelSort->setSourceModel(model);

    ui->table->setModel(modelSort);

    for (int c = 0; c < model->columnCount(); ++c)
        ui->table->setColumnHidden(c, true);
    ui->table->setColumnHidden(ModelBewertungen::ColSudName, false);
    ui->table->setColumnHidden(ModelBewertungen::ColDatum, false);
    ui->table->setColumnHidden(ModelBewertungen::ColWoche, false);
    ui->table->setColumnHidden(ModelBewertungen::ColSterne, false);

    ui->table->setItemDelegateForColumn(ModelBewertungen::ColSudName, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, ui->table));
    ui->table->setItemDelegateForColumn(ModelBewertungen::ColDatum, new DateTimeDelegate(false, true, ui->table));
    ui->table->setItemDelegateForColumn(ModelBewertungen::ColWoche, new TextDelegate(true, Qt::AlignHCenter | Qt::AlignVCenter, ui->table));
    ui->table->setItemDelegateForColumn(ModelBewertungen::ColSterne, new RatingDelegate(ui->table));

    QHeaderView *header = ui->table->header();
    header->moveSection(header->visualIndex(ModelBewertungen::ColSudName), 0);
    header->moveSection(header->visualIndex(ModelBewertungen::ColDatum), 1);
    header->moveSection(header->visualIndex(ModelBewertungen::ColWoche), 2);
    header->moveSection(header->visualIndex(ModelBewertungen::ColSterne), 3);

    ui->table->setTreePosition(ModelBewertungen::ColSudName);

    loadSettings();
}

TabBewertungen::~TabBewertungen()
{
    saveSettings();
    delete ui;
}

void TabBewertungen::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->table->header()->restoreState(gSettings->value("tableState").toByteArray());
    gSettings->endGroup();
}

void TabBewertungen::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableState", ui->table->header()->saveState());
    gSettings->endGroup();
}

void TabBewertungen::restoreView()
{
}
