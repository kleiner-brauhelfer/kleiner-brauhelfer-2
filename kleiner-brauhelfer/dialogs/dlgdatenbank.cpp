#include "dlgdatenbank.h"
#include "ui_dlgdatenbank.h"
#include <QFileDialog>
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgDatenbank* DlgDatenbank::Dialog = nullptr;

DlgDatenbank::DlgDatenbank(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgDatenbank)
{
    ui->setupUi(this);

    ui->tableViewSud->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->comboBox->clear();
    ui->comboBox->addItem(bh->modelAnhang()->tableName());
    ui->comboBox->addItem(bh->modelAusruestung()->tableName());
    ui->comboBox->addItem(bh->modelBewertungen()->tableName());
    ui->comboBox->addItem(bh->modelEtiketten()->tableName());
    ui->comboBox->addItem(bh->modelTags()->tableName());
    ui->comboBox->addItem(bh->modelGeraete()->tableName());
    ui->comboBox->addItem(bh->modelHauptgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelHefe()->tableName());
    ui->comboBox->addItem(bh->modelHefegaben()->tableName());
    ui->comboBox->addItem(bh->modelHopfen()->tableName());
    ui->comboBox->addItem(bh->modelHopfengaben()->tableName());
    ui->comboBox->addItem(bh->modelKategorien()->tableName());
    ui->comboBox->addItem(bh->modelMalz()->tableName());
    ui->comboBox->addItem(bh->modelMalzschuettung()->tableName());
    ui->comboBox->addItem(bh->modelNachgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelMaischplan()->tableName());
    ui->comboBox->addItem(bh->modelSchnellgaerverlauf()->tableName());
    ui->comboBox->addItem(bh->modelSud()->tableName());
    ui->comboBox->addItem(bh->modelWasser()->tableName());
    ui->comboBox->addItem(bh->modelWasseraufbereitung()->tableName());
    ui->comboBox->addItem(bh->modelWeitereZutaten()->tableName());
    ui->comboBox->addItem(bh->modelWeitereZutatenGaben()->tableName());
    ui->comboBox->setCurrentIndex(17);

    ui->comboBoxSud->clear();
    ui->comboBoxSud->addItem(bh->modelAnhang()->tableName());
    ui->comboBoxSud->addItem(bh->modelBewertungen()->tableName());
    ui->comboBoxSud->addItem(bh->modelEtiketten()->tableName());
    ui->comboBoxSud->addItem(bh->modelTags()->tableName());
    ui->comboBoxSud->addItem(bh->modelHauptgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelHefegaben()->tableName());
    ui->comboBoxSud->addItem(bh->modelHopfengaben()->tableName());
    ui->comboBoxSud->addItem(bh->modelMalzschuettung()->tableName());
    ui->comboBoxSud->addItem(bh->modelNachgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelMaischplan()->tableName());
    ui->comboBoxSud->addItem(bh->modelSchnellgaerverlauf()->tableName());
    ui->comboBoxSud->addItem(bh->modelWasseraufbereitung()->tableName());
    ui->comboBoxSud->addItem(bh->modelWeitereZutatenGaben()->tableName());
    ui->comboBoxSud->setCurrentIndex(7);

    connect(bh->sud(), &SudObject::loadedChanged, this, &DlgDatenbank::sudLoaded);

    updateValues();
}

DlgDatenbank::~DlgDatenbank()
{
    delete ui;
}

void DlgDatenbank::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void DlgDatenbank::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    gSettings->endGroup();
}

void DlgDatenbank::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("splitterState");
    gSettings->endGroup();
}

void DlgDatenbank::sudLoaded()
{
    on_comboBoxSud_currentTextChanged(ui->comboBoxSud->currentText());
}

void DlgDatenbank::updateValues()
{
    ui->tbDatenbankPfad->setText(bh->databasePath());
    ui->tbDatenbankVersion->setText(QString::number(bh->databaseVersion()));
}

void DlgDatenbank::on_comboBox_currentTextChanged(const QString &table)
{
    SqlTableModel* model = nullptr;
    if (table == bh->modelAnhang()->tableName())
        model = bh->modelAnhang();
    else if (table == bh->modelAusruestung()->tableName())
        model = bh->modelAusruestung();
    else if (table == bh->modelBewertungen()->tableName())
        model = bh->modelBewertungen();
    else if (table == bh->modelEtiketten()->tableName())
        model = bh->modelEtiketten();
    else if (table == bh->modelTags()->tableName())
        model = bh->modelTags();
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
    else if (table == bh->modelKategorien()->tableName())
        model = bh->modelKategorien();
    else if (table == bh->modelMalz()->tableName())
        model = bh->modelMalz();
    else if (table == bh->modelMalzschuettung()->tableName())
        model = bh->modelMalzschuettung();
    else if (table == bh->modelNachgaerverlauf()->tableName())
        model = bh->modelNachgaerverlauf();
    else if (table == bh->modelMaischplan()->tableName())
        model = bh->modelMaischplan();
    else if (table == bh->modelSchnellgaerverlauf()->tableName())
        model = bh->modelSchnellgaerverlauf();
    else if (table == bh->modelSud()->tableName())
        model = bh->modelSud();
    else if (table == bh->modelWasser()->tableName())
        model = bh->modelWasser();
    else if (table == bh->modelWasseraufbereitung()->tableName())
        model = bh->modelWasseraufbereitung();
    else if (table == bh->modelWeitereZutaten()->tableName())
        model = bh->modelWeitereZutaten();
    else if (table == bh->modelWeitereZutatenGaben()->tableName())
        model = bh->modelWeitereZutatenGaben();
    if (model)
    {
        ui->tableView->setModel(model);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, false);
        ui->tableView->setColumnHidden(model->fieldIndex(QStringLiteral("deleted")), true);
        if (model == bh->modelSud())
            connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatenbank::tableView_selectionChanged);
    }
}

void DlgDatenbank::on_comboBoxSud_currentTextChanged(const QString &table)
{
    ProxyModel* model = nullptr;
    if (table == bh->modelAnhang()->tableName())
        model = bh->sud()->modelAnhang();
    else if (table == bh->modelBewertungen()->tableName())
        model = bh->sud()->modelBewertungen();
    else if (table == bh->modelEtiketten()->tableName())
        model = bh->sud()->modelEtiketten();
    else if (table == bh->modelTags()->tableName())
        model = bh->sud()->modelTags();
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
    else if (table == bh->modelMaischplan()->tableName())
        model = bh->sud()->modelMaischplan();
    else if (table == bh->modelSchnellgaerverlauf()->tableName())
        model = bh->sud()->modelSchnellgaerverlauf();
    else if (table == bh->modelWasseraufbereitung()->tableName())
        model = bh->sud()->modelWasseraufbereitung();
    else if (table == bh->modelWeitereZutatenGaben()->tableName())
        model = bh->sud()->modelWeitereZutatenGaben();
    if (model)
    {
        ui->tableViewSud->setModel(model);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableViewSud->setColumnHidden(col, false);
        ui->tableViewSud->setColumnHidden(model->fieldIndex(QStringLiteral("deleted")), true);
    }
}

void DlgDatenbank::tableView_selectionChanged()
{
    if (ui->tableView->model() == bh->modelSud())
    {
        QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
        if (indexes.count() > 0)
        {
            int sudId = bh->modelSud()->data(indexes[0].row(), ModelSud::ColID).toInt();
            bh->sud()->load(sudId);
        }
        else
        {
            bh->sud()->unload();
        }
    }
}
