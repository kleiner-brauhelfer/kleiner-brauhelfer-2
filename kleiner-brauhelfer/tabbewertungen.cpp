#include "tabbewertungen.h"
#include "ui_tabbewertungen.h"
#include "model/treeproxymodel.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabBewertungen::TabBewertungen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBewertungen)
{
    ui->setupUi(this);

    TreeProxyModel* model = new TreeProxyModel(this);
    model->setGroupping(ModelBewertungen::ColSudID, Qt::DisplayRole);
    model->setSourceModel(bh->modelBewertungen());

    QVector<QVariant> sudIds;
    for (int row = 0; row < bh->modelSud()->rowCount(); row++)
        sudIds.append(bh->modelSud()->data(row, ModelSud::ColID));
    model->setCustomGroups(sudIds);

    QSortFilterProxyModel* modelSort = new QSortFilterProxyModel(this);
    modelSort->setSourceModel(model);

    ui->table->setModel(modelSort);
}

TabBewertungen::~TabBewertungen()
{
    saveSettings();
    delete ui;
}

void TabBewertungen::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabBewertungen::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->endGroup();
}

void TabBewertungen::restoreView()
{
}