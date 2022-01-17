#include "dlgbrauuebersicht.h"
#include "ui_dlgbrauuebersicht.h"
#include <QKeyEvent>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodelsud.h"
#include "model/textdelegate.h"
#include "model/datedelegate.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

DlgBrauUebersicht* DlgBrauUebersicht::Dialog = nullptr;

class ProxyModelBrauuebersicht : public ProxyModelSud
{
public:
    ProxyModelBrauuebersicht(QObject* parent = nullptr) :
        ProxyModelSud(parent)
    {
    }

    using ProxyModelSud::data;

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole)
        {
            int col = index.column();
            if (col == mColAuswahl3)
                return gSettings->DiagramLinie3Light;
            if (col == mColAuswahl2)
                return gSettings->DiagramLinie2Light;
            if (col == mColAuswahl1)
                return gSettings->DiagramLinie1Light;
        }
        return ProxyModelSud::data(index, role);
    }

    int mColAuswahl1;
    int mColAuswahl2;
    int mColAuswahl3;
};

DlgBrauUebersicht::DlgBrauUebersicht(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgBrauUebersicht)
{
    ui->setupUi(this);

    ui->splitter->setSizes({INT_MAX, INT_MAX});

    ui->diagram->colorL1 = gSettings->DiagramLinie1;
    ui->diagram->colorL2 = gSettings->DiagramLinie2;
    ui->diagram->colorL3 = gSettings->DiagramLinie3;

    mAuswahlListe.append({-1, 0, tr("<keine>"), "", 0, 0});
    mAuswahlListe.append({ModelSud::Colerg_AbgefuellteBiermenge, 1, tr("Abgefüllte Biermenge"), tr("l"), 0, 0});
    mAuswahlListe.append({ModelSud::Colerg_S_Gesamt, 2, tr("Schüttung"), tr("kg"), 0, 0});
    mAuswahlListe.append({ModelSud::ColSWIst, 1, tr("Stammwürze"), tr("°P"), 0, 0});
    mAuswahlListe.append({ModelSud::Colerg_Sudhausausbeute, 0, tr("Sudhausausbeute"), tr("%"), 0, 90});
    mAuswahlListe.append({ModelSud::Colerg_EffektiveAusbeute, 0, tr("Effektive Sudhausausbeute"), tr("%"), 0, 90});
    mAuswahlListe.append({ModelSud::ColVerdampfungsrateIst, 1, tr("Verdampfungsrate"), tr("l/h"), 0, 10});
    mAuswahlListe.append({ModelSud::Colerg_Alkohol, 1, tr("Alkohol"), tr("%"), 0, 0});
    mAuswahlListe.append({ModelSud::ColSREIst, 1, tr("Scheinbarer Restextrakt"), tr("°P"), 0, 0});
    mAuswahlListe.append({ModelSud::ColsEVG, 0, tr("Scheinbarer Endvergärungsgrad"), tr("%"), 0, 90});
    mAuswahlListe.append({ModelSud::ColtEVG, 0, tr("Tatsächlicher Endvergärungsgrad"), tr("%"), 0, 90});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        mAuswahlListe.append({ModelSud::Colerg_Preis, 2, tr("Kosten"), tr("%1/l").arg(QLocale().currencySymbol()), 0, 0});
}

DlgBrauUebersicht::~DlgBrauUebersicht()
{
    delete ui;
}

void DlgBrauUebersicht::saveSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableState", ui->tableView->horizontalHeader()->saveState());
    gSettings->setValue("Auswahl1", ui->cbAuswahlL1->currentIndex());
    gSettings->setValue("Auswahl2", ui->cbAuswahlL2->currentIndex());
    gSettings->setValue("Auswahl3", ui->cbAuswahlL3->currentIndex());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void DlgBrauUebersicht::loadSettings()
{
    gSettings->beginGroup(staticMetaObject.className());
    ui->tableView->restoreState(gSettings->value("tableState").toByteArray());
    ui->cbAuswahlL1->setCurrentIndex(gSettings->value("Auswahl1", 0).toInt());
    ui->cbAuswahlL2->setCurrentIndex(gSettings->value("Auswahl2", 0).toInt());
    ui->cbAuswahlL3->setCurrentIndex(gSettings->value("Auswahl3", 0).toInt());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    gSettings->endGroup();
}

void DlgBrauUebersicht::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->remove("tableState");
    gSettings->remove("splitterState");
    gSettings->endGroup();
}

void DlgBrauUebersicht::setModel(QAbstractItemModel* model)
{
    TableView *table = ui->tableView;
    ProxyModelBrauuebersicht *proxyModel = new ProxyModelBrauuebersicht(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterStatus(ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    proxyModel->sort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    table->setModel(proxyModel);
    table->appendCol({ModelSud::ColSudname, true, false, 300, nullptr});
    table->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    table->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->appendCol({ModelSud::ColBraudatum, true, false, 100, new DateDelegate(false, false, table)});
    ui->cbAuswahlL2->addItem(mAuswahlListe[0].label);
    ui->cbAuswahlL3->addItem(mAuswahlListe[0].label);
    for (int i = 1; i < mAuswahlListe.count(); ++i)
    {
        table->appendCol({mAuswahlListe[i].col, true, true, 80, new DoubleSpinBoxDelegate(mAuswahlListe[i].precision, table)});
        ui->cbAuswahlL1->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL2->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL3->addItem(mAuswahlListe[i].label);
    }
    table->build();
    table->setDefaultContextMenu();

    gSettings->beginGroup(staticMetaObject.className());

    table->restoreState(gSettings->value("tableState").toByteArray());

    ui->cbAuswahlL1->setCurrentIndex(gSettings->value("Auswahl1", 0).toInt());
    ui->cbAuswahlL2->setCurrentIndex(gSettings->value("Auswahl2", 0).toInt());
    ui->cbAuswahlL3->setCurrentIndex(gSettings->value("Auswahl3", 0).toInt());

    gSettings->endGroup();

    connect(model, SIGNAL(layoutChanged()), this, SLOT(updateDiagram()));
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(updateDiagram()));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(updateDiagram()));
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(modelDataChanged(QModelIndex)));
    connect(table->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(table_selectionChanged(QItemSelection)));
    connect(ui->diagram, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    updateDiagram();
}

void DlgBrauUebersicht::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (ui->tableView->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Return:
            QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
            if (selection.count() > 0)
            {
                on_tableView_doubleClicked(selection[0]);
            }
            break;
        }
    }
}

void DlgBrauUebersicht::modelDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        ProxyModelBrauuebersicht *proxyModel = static_cast<ProxyModelBrauuebersicht*>(ui->tableView->model());
        proxyModel->invalidate();
        break;
    }
    updateDiagram();
}

void DlgBrauUebersicht::updateDiagram()
{
    ui->diagram->DiagrammLeeren();
    ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->tableView->model());
    if (model->rowCount() > 1)
    {
        int i = ui->cbAuswahlL1->currentIndex() + 1;
        if (i >= 0 && i < mAuswahlListe.size())
        {
            AuswahlType *auswahl1 = &mAuswahlListe[i];
            model->mColAuswahl1 = auswahl1->col;
            ui->diagram->BezeichnungL1 = auswahl1->label + " [" + auswahl1->unit + "]";
            ui->diagram->KurzbezeichnungL1 = auswahl1->unit;
            ui->diagram->L1Precision = auswahl1->precision;
            ui->diagram->L1Min = auswahl1->min;
            ui->diagram->L1Max = auswahl1->max;
        }
        i = ui->cbAuswahlL2->currentIndex();
        if (i >= 0 && i < mAuswahlListe.size())
        {
            AuswahlType *auswahl2 = &mAuswahlListe[i];
            model->mColAuswahl2 = auswahl2->col;
            ui->diagram->BezeichnungL2 = auswahl2->label + " [" + auswahl2->unit + "]";
            ui->diagram->KurzbezeichnungL2 = auswahl2->unit;
            ui->diagram->L2Precision = auswahl2->precision;
            ui->diagram->L2Min = auswahl2->min;
            ui->diagram->L2Max = auswahl2->max;
        }
        i = ui->cbAuswahlL3->currentIndex();
        if (i >= 0 && i < mAuswahlListe.size())
        {
            AuswahlType *auswahl3 = &mAuswahlListe[i];
            model->mColAuswahl3 = auswahl3->col;
            ui->diagram->BezeichnungL3 = auswahl3->label + " [" + auswahl3->unit + "]";
            ui->diagram->KurzbezeichnungL3 = auswahl3->unit;
            ui->diagram->L3Precision = auswahl3->precision;
            ui->diagram->L3Min = auswahl3->min;
            ui->diagram->L3Max = auswahl3->max;
        }
        for (int row = model->rowCount() - 1; row >= 0; --row)
        {
            QDateTime dt = model->index(row, ModelSud::ColBraudatum).data().toDateTime();
            ui->diagram->Ids.append(model->index(row, ModelSud::ColID).data().toInt());
            ui->diagram->L1Daten.append(model->index(row, model->mColAuswahl1).data().toDouble());
            ui->diagram->L1Datum.append(dt);
            if (model->mColAuswahl2 >= 0)
            {
                ui->diagram->L2Daten.append(model->index(row, model->mColAuswahl2).data().toDouble());
                ui->diagram->L2Datum.append(dt);
            }
            if (model->mColAuswahl3 >= 0)
            {
                ui->diagram->L3Daten.append(model->index(row, model->mColAuswahl3).data().toDouble());
                ui->diagram->L3Datum.append(dt);
            }
        }
    }
    ui->diagram->repaint();
}

void DlgBrauUebersicht::on_tableView_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableView->model());
    int sudId = model->data(index.row(), ModelSud::ColID).toInt();
    emit clicked(sudId);
}

void DlgBrauUebersicht::table_selectionChanged(const QItemSelection &selected)
{
    int sudId = -1;
    if (selected.indexes().count() > 0)
    {
        const QModelIndex index = selected.indexes()[0];
        sudId = index.sibling(index.row(), ModelSud::ColID).data().toInt();
    }
    ui->diagram->MarkierePunkt(sudId);
}

void DlgBrauUebersicht::diagram_selectionChanged(int sudId)
{
    for (int row = 0; row < ui->tableView->model()->rowCount(); ++row)
    {
        if (ui->tableView->model()->index(row, ModelSud::ColID).data().toInt() == sudId)
        {
            ui->tableView->selectRow(row);
            break;
        }
    }
}

void DlgBrauUebersicht::on_cbAuswahlL1_currentIndexChanged(int)
{
    if (ui->cbAuswahlL1->hasFocus())
    {
        updateDiagram();
        ui->tableView->setFocus();
    }
}

void DlgBrauUebersicht::on_cbAuswahlL2_currentIndexChanged(int)
{
    if (ui->cbAuswahlL2->hasFocus())
    {
        updateDiagram();
        ui->tableView->setFocus();
    }
}

void DlgBrauUebersicht::on_cbAuswahlL3_currentIndexChanged(int)
{
    if (ui->cbAuswahlL3->hasFocus())
    {
        updateDiagram();
        ui->tableView->setFocus();
    }
}
