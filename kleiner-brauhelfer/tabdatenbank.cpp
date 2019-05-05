#include "tabdatenbank.h"
#include "ui_tabdatenbank.h"
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabDatenbank::TabDatenbank(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDatenbank)
{
    ui->setupUi(this);

    ui->tableViewSud->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->comboBox->clear();
    ui->comboBox->addItem(bh->modelAnhang()->tableName());
    ui->comboBox->addItem(bh->modelAusruestung()->tableName());
    ui->comboBox->addItem(bh->modelBewertungen()->tableName());
    ui->comboBox->addItem(bh->modelFlaschenlabel()->tableName());
    ui->comboBox->addItem(bh->modelFlaschenlabelTags()->tableName());
    ui->comboBox->addItem(bh->modelGeraete()->tableName());
    ui->comboBox->addItem(bh->modelHauptgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelHefe()->tableName());
    ui->comboBox->addItem(bh->modelHefegaben()->tableName());
    ui->comboBox->addItem(bh->modelHopfen()->tableName());
    ui->comboBox->addItem(bh->modelHopfengaben()->tableName());
    ui->comboBox->addItem(bh->modelMalz()->tableName());
    ui->comboBox->addItem(bh->modelMalzschuettung()->tableName());
    ui->comboBox->addItem(bh->modelNachgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelRasten()->tableName());
    ui->comboBox->addItem(bh->modelSchnellgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelSud()->tableName());
    ui->comboBox->addItem(bh->modelWasser()->tableName());
    ui->comboBox->addItem(bh->modelWeitereZutaten()->tableName());
    ui->comboBox->addItem(bh->modelWeitereZutatenGaben()->tableName());
    ui->comboBox->setCurrentIndex(16);

    ui->comboBoxSud->clear();
    ui->comboBoxSud->addItem(bh->modelAnhang()->tableName());
    ui->comboBoxSud->addItem(bh->modelBewertungen()->tableName());
    ui->comboBoxSud->addItem(bh->modelFlaschenlabel()->tableName());
    ui->comboBoxSud->addItem(bh->modelFlaschenlabelTags()->tableName());
    ui->comboBoxSud->addItem(bh->modelHauptgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelHefegaben()->tableName());
    ui->comboBoxSud->addItem(bh->modelHopfengaben()->tableName());
    ui->comboBoxSud->addItem(bh->modelMalzschuettung()->tableName());
    ui->comboBoxSud->addItem(bh->modelNachgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelRasten()->tableName());
    ui->comboBoxSud->addItem(bh->modelSchnellgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelWeitereZutatenGaben()->tableName());
    ui->comboBoxSud->setCurrentIndex(7);

    gSettings->beginGroup("TabDatenbank");

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    gSettings->endGroup();

    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));

    updateValues();
}

TabDatenbank::~TabDatenbank()
{
    delete ui;
}

void TabDatenbank::saveSettings()
{
    gSettings->beginGroup("TabDatenbank");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void TabDatenbank::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
}

void TabDatenbank::sudLoaded()
{
    on_comboBoxSud_currentIndexChanged(ui->comboBoxSud->currentText());
}

void TabDatenbank::updateValues()
{
    ui->tbDatenbankPfad->setText(bh->databasePath());
    ui->tbDatenbankVersion->setText(QString::number(bh->databaseVersion()));
}

void TabDatenbank::on_comboBox_currentIndexChanged(const QString &table)
{
    SqlTableModel* model = nullptr;
    if (table == bh->modelAnhang()->tableName())
        model = bh->modelAnhang();
    else if (table == bh->modelAusruestung()->tableName())
        model = bh->modelAusruestung();
    else if (table == bh->modelBewertungen()->tableName())
        model = bh->modelBewertungen();
    else if (table == bh->modelFlaschenlabel()->tableName())
        model = bh->modelFlaschenlabel();
    else if (table == bh->modelFlaschenlabelTags()->tableName())
        model = bh->modelFlaschenlabelTags();
    else if (table == bh->modelGeraete()->tableName())
        model = bh->modelGeraete();
    else if (table == bh->modelHauptgaerverlauf()->tableName())
        model = bh->modelHauptgaerverlauf();
    else if (table == bh->modelHefe()->tableName())
        model = bh->modelHefe();
    else if (table == bh->modelHefegaben()->tableName())
        model = bh->modelHefegaben();
    else if (table == bh->modelHopfen()->tableName())
        model = bh->modelHopfen();
    else if (table == bh->modelHopfengaben()->tableName())
        model = bh->modelHopfengaben();
    else if (table == bh->modelMalz()->tableName())
        model = bh->modelMalz();
    else if (table == bh->modelMalzschuettung()->tableName())
        model = bh->modelMalzschuettung();
    else if (table == bh->modelNachgaerverlauf()->tableName())
        model = bh->modelNachgaerverlauf();
    else if (table == bh->modelRasten()->tableName())
        model = bh->modelRasten();
    else if (table == bh->modelSchnellgaerverlauf()->tableName())
        model = bh->modelSchnellgaerverlauf();
    else if (table == bh->modelSud()->tableName())
        model = bh->modelSud();
    else if (table == bh->modelWasser()->tableName())
        model = bh->modelWasser();
    else if (table == bh->modelWeitereZutaten()->tableName())
        model = bh->modelWeitereZutaten();
    else if (table == bh->modelWeitereZutatenGaben()->tableName())
        model = bh->modelWeitereZutatenGaben();
    if (model)
    {
        ui->tableView->setModel(model);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, false);
        ui->tableView->setColumnHidden(model->fieldIndex("deleted"), true);
        if (model == bh->modelSud())
            connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
                    this, SLOT(tableView_selectionChanged()));
    }
}

void TabDatenbank::on_comboBoxSud_currentIndexChanged(const QString &table)
{
    ProxyModel* model = nullptr;
    if (table == bh->modelAnhang()->tableName())
        model = bh->sud()->modelAnhang();
    else if (table == bh->modelBewertungen()->tableName())
        model = bh->sud()->modelBewertungen();
    else if (table == bh->modelFlaschenlabel()->tableName())
        model = bh->sud()->modelFlaschenlabel();
    else if (table == bh->modelFlaschenlabelTags()->tableName())
        model = bh->sud()->modelFlaschenlabelTags();
    else if (table == bh->modelHauptgaerverlauf()->tableName())
        model = bh->sud()->modelHauptgaerverlauf();
    else if (table == bh->modelHefegaben()->tableName())
        model = bh->sud()->modelHefegaben();
    else if (table == bh->modelHopfengaben()->tableName())
        model = bh->sud()->modelHopfengaben();
    else if (table == bh->modelMalzschuettung()->tableName())
        model = bh->sud()->modelMalzschuettung();
    else if (table == bh->modelNachgaerverlauf()->tableName())
        model = bh->sud()->modelNachgaerverlauf();
    else if (table == bh->modelRasten()->tableName())
        model = bh->sud()->modelRasten();
    else if (table == bh->modelSchnellgaerverlauf()->tableName())
        model = bh->sud()->modelSchnellgaerverlauf();
    else if (table == bh->modelWeitereZutatenGaben()->tableName())
        model = bh->sud()->modelWeitereZutatenGaben();
    if (model)
    {
        ui->tableViewSud->setModel(model);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, false);
        ui->tableViewSud->setColumnHidden(model->fieldIndex("deleted"), true);
    }
}

void TabDatenbank::tableView_selectionChanged()
{
    if (ui->tableView->model() == bh->modelSud())
    {
        QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
        if (indexes.count() > 0)
        {
            int sudId = bh->modelSud()->data(indexes[0].row(), "ID").toInt();
            bh->sud()->load(sudId);
        }
        else
        {
            bh->sud()->unload();
        }
    }
}
