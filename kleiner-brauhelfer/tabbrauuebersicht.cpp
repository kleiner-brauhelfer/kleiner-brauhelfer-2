#include "tabbrauuebersicht.h"
#include "ui_tabbrauuebersicht.h"
#include "proxymodelsud.h"
#include "model/textdelegate.h"
#include "model/datedelegate.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

class ProxyModelBrauuebersicht : public ProxyModelSud
{
public:
    ProxyModelBrauuebersicht(QObject* parent = nullptr) :
        ProxyModelSud(parent),
        colAuswahl1(-1),
        colAuswahl2(-1),
        colAuswahl3(-1)
    {
    }

    using ProxyModelSud::data;

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole)
        {
            int col = index.column();
            if (col == colAuswahl3)
                return gSettings->DiagramLinie3Light;
            if (col == colAuswahl2)
                return gSettings->DiagramLinie2Light;
            if (col == colAuswahl1)
                return gSettings->DiagramLinie1Light;
        }
        return ProxyModelSud::data(index, role);
    }

    int colAuswahl1;
    int colAuswahl2;
    int colAuswahl3;
};

TabBrauuebersicht::TabBrauuebersicht(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBrauuebersicht)
{
    ui->setupUi(this);
    restoreView();

    ProxyModelBrauuebersicht *proxyModel = new ProxyModelBrauuebersicht(this);
    proxyModel->setSourceModel(bh->modelSud());
    proxyModel->setFilterStatus(ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    proxyModel->sort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    ui->table->setModel(proxyModel);

    gSettings->beginGroup(staticMetaObject.className());
    ui->table->restoreState(gSettings->value("tableState").toByteArray());
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());
    proxyModel->setFilterMerkliste(gSettings->value("filterMerkliste", false).toBool());
    proxyModel->setFilterDate(gSettings->value("ZeitraumAlle", false).toBool());
    QDate minDate = gSettings->value("ZeitraumVon", QDate::currentDate().addYears(-1)).toDate();
    QDate maxDate = gSettings->value("ZeitraumBis", QDate::currentDate()).toDate();
    proxyModel->setFilterMinimumDate(QDateTime(minDate, QTime(1,0,0)));
    proxyModel->setFilterMaximumDate(QDateTime(maxDate, QTime(23,59,59)));
    ui->cbAuswahlL1->setCurrentIndex(gSettings->value("Auswahl1", 0).toInt());
    ui->cbAuswahlL2->setCurrentIndex(gSettings->value("Auswahl2", 0).toInt());
    ui->cbAuswahlL3->setCurrentIndex(gSettings->value("Auswahl3", 0).toInt());
    gSettings->endGroup();

    ui->btnFilter->init(proxyModel, false);

    connect(gSettings, &Settings::modulesChanged, this, &TabBrauuebersicht::onModulesChanged);
    connect(ui->table->model(), &QAbstractItemModel::layoutChanged, this, &TabBrauuebersicht::onLayoutChanged);
    connect(ui->table->model(), &QAbstractItemModel::dataChanged, this, &TabBrauuebersicht::onDataChanged);

    connect(ui->table->selectionModel(), &QItemSelectionModel::selectionChanged,this, &TabBrauuebersicht::onTableSelectionChanged);
    connect(ui->diagram, &Chart3::selectionChanged, this, &TabBrauuebersicht::onDiagramSelectionChanged);

    onModulesChanged(Settings::ModuleAlle);
    onLayoutChanged();
}

TabBrauuebersicht::~TabBrauuebersicht()
{
    saveSettings();
    delete ui;
}

void TabBrauuebersicht::saveSettings()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    gSettings->beginGroup(staticMetaObject.className());
    gSettings->setValue("tableState", ui->table->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("filterMerkliste", model->filterMerkliste());
    gSettings->setValue("ZeitraumAlle", model->filterDate());
    gSettings->setValue("ZeitraumVon", model->filterMinimumDate().date());
    gSettings->setValue("ZeitraumBis", model->filterMaximumDate().date());
    gSettings->setValue("Auswahl1", ui->cbAuswahlL1->currentIndex());
    gSettings->setValue("Auswahl2", ui->cbAuswahlL2->currentIndex());
    gSettings->setValue("Auswahl3", ui->cbAuswahlL3->currentIndex());
    gSettings->endGroup();
}

void TabBrauuebersicht::restoreView()
{
    ui->table->restoreDefaultState();
    ui->splitter->setSizes({500, 500});
}

void TabBrauuebersicht::onModulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        build();
    }
}

void TabBrauuebersicht::build()
{
    mAuswahlListe.clear();
    mAuswahlListe.append({-1, 0, tr("<keine>"), ""});
    mAuswahlListe.append({ModelSud::Colerg_AbgefuellteBiermenge, 1, tr("Abgefüllte Biermenge"), "L"});
    mAuswahlListe.append({ModelSud::Colerg_S_Gesamt, 2, tr("Schüttung"), "kg"});
    mAuswahlListe.append({ModelSud::ColSWIst, 1, tr("Stammwürze"), "°P"});
    mAuswahlListe.append({ModelSud::Colerg_Sudhausausbeute, 0, tr("Sudhausausbeute"), "%"});
    mAuswahlListe.append({ModelSud::Colerg_EffektiveAusbeute, 0, tr("Effektive Sudhausausbeute"), "%"});
    mAuswahlListe.append({ModelSud::ColVerdampfungsrateIst, 1, tr("Verdampfungsrate"), "L/h"});
    mAuswahlListe.append({ModelSud::Colerg_Alkohol, 1, tr("Alkohol"), "%"});
    mAuswahlListe.append({ModelSud::ColSREIst, 1, tr("Scheinbarer Restextrakt"), "°P"});
    mAuswahlListe.append({ModelSud::ColsEVG, 0, tr("Scheinbarer Endvergärungsgrad"), "%"});
    mAuswahlListe.append({ModelSud::ColtEVG, 0, tr("Tatsächlicher Endvergärungsgrad"), "%"});
    if (gSettings->isModuleEnabled(Settings::ModulePreiskalkulation))
        mAuswahlListe.append({ModelSud::Colerg_Preis, 2, tr("Kosten"), QString("%1/L").arg(QLocale().currencySymbol())});

    ui->table->clearCols();
    ui->table->appendCol({ModelSud::ColSudname, true, false, 200, new TextDelegate(ui->table)});
    ui->table->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(ui->table)});
    ui->table->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, ui->table)});
    ui->table->appendCol({ModelSud::ColBraudatum, true, false, 100, new DateDelegate(false, false, ui->table)});
    ui->cbAuswahlL1->clear();
    ui->cbAuswahlL2->clear();
    ui->cbAuswahlL3->clear();
    ui->cbAuswahlL2->addItem(mAuswahlListe[0].label);
    ui->cbAuswahlL3->addItem(mAuswahlListe[0].label);
    for (int i = 1; i < mAuswahlListe.count(); ++i)
    {
        ui->table->appendCol({mAuswahlListe[i].col, true, true, 80, new DoubleSpinBoxDelegate(mAuswahlListe[i].precision, ui->table)});
        ui->cbAuswahlL1->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL2->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL3->addItem(mAuswahlListe[i].label);
    }
    ui->table->build();
    ui->table->setDefaultContextMenu();
}

void TabBrauuebersicht::onLayoutChanged()
{
    updateDiagram();
    updateFilterLabel();
}

void TabBrauuebersicht::onDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->table->model());
        model->invalidate();
        break;
    }
    updateDiagram();
}

void TabBrauuebersicht::updateDiagram()
{
    ui->diagram->clear();
    ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->table->model());
    if (model->rowCount() > 1)
    {
        int index = ui->cbAuswahlL1->currentIndex() + 1;
        if (index >= 0 && index < mAuswahlListe.size())
        {
            AuswahlType *auswahl1 = &mAuswahlListe[index];
            model->colAuswahl1 = auswahl1->col;
            QVector<double> x(model->rowCount());
            QVector<double> y(model->rowCount());
            int i = 0;
            for (int row = model->rowCount() - 1; row >= 0; --row)
            {
                QDateTime dt = model->index(row, ModelSud::ColBraudatum).data().toDateTime();
                x[i] = dt.toSecsSinceEpoch();
                y[i] = model->index(row, auswahl1->col).data().toDouble();
                i++;
            }
            ui->diagram->setData1(x, y, auswahl1->label, auswahl1->unit, auswahl1->precision);
        }
        index = ui->cbAuswahlL2->currentIndex();
        if (index >= 0 && index < mAuswahlListe.size())
        {
            AuswahlType *auswahl2 = &mAuswahlListe[index];
            model->colAuswahl2 = auswahl2->col;
            if (auswahl2->col >= 0)
            {
                QVector<double> x(model->rowCount());
                QVector<double> y(model->rowCount());
                int i = 0;
                for (int row = model->rowCount() - 1; row >= 0; --row)
                {
                    QDateTime dt = model->index(row, ModelSud::ColBraudatum).data().toDateTime();
                    x[i] = dt.toSecsSinceEpoch();
                    y[i] = model->index(row, auswahl2->col).data().toDouble();
                    i++;
                }
                ui->diagram->setData2(x, y, auswahl2->label, auswahl2->unit, auswahl2->precision);
            }
        }
        index = ui->cbAuswahlL3->currentIndex();
        if (index >= 0 && index < mAuswahlListe.size())
        {
            AuswahlType *auswahl3 = &mAuswahlListe[index];
            model->colAuswahl3 = auswahl3->col;
            if (auswahl3->col >= 0)
            {
                QVector<double> x(model->rowCount());
                QVector<double> y(model->rowCount());
                int i = 0;
                for (int row = model->rowCount() - 1; row >= 0; --row)
                {
                    QDateTime dt = model->index(row, ModelSud::ColBraudatum).data().toDateTime();
                    x[i] = dt.toSecsSinceEpoch();
                    y[i] = model->index(row, auswahl3->col).data().toDouble();
                    i++;
                }
                ui->diagram->setData3(x, y, auswahl3->label, auswahl3->unit, auswahl3->precision);
            }
        }
    }
    onTableSelectionChanged(ui->table->selectionModel()->selection());
    ui->diagram->rescale();
    ui->diagram->replot();
}

void TabBrauuebersicht::onTableSelectionChanged(const QItemSelection &selected)
{
    int index = -1;
    if (selected.indexes().count() > 0)
        index = ui->table->model()->rowCount() - 1 - selected.indexes()[0].row();
    ui->diagram->select(index);
}

void TabBrauuebersicht::onDiagramSelectionChanged(int index)
{
    ui->table->selectRow(ui->table->model()->rowCount() - 1 - index);
    ui->table->setFocus();
}

void TabBrauuebersicht::updateFilterLabel()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->table->model());
    ProxyModelSud proxy;
    proxy.setSourceModel(bh->modelSud());
    proxy.setFilterStatus(ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    ui->lblFilter->setText(QString::number(model->rowCount()) + " / " + QString::number(proxy.rowCount()));
}

void TabBrauuebersicht::on_tbFilter_textChanged(const QString &pattern)
{
    static_cast<ProxyModelSud*>(ui->table->model())->setFilterText(pattern);
}

void TabBrauuebersicht::on_cbAuswahlL1_currentIndexChanged(int)
{
    if (ui->cbAuswahlL1->hasFocus())
    {
        updateDiagram();
        ui->table->setFocus();
    }
}

void TabBrauuebersicht::on_cbAuswahlL2_currentIndexChanged(int)
{
    if (ui->cbAuswahlL2->hasFocus())
    {
        updateDiagram();
        ui->table->setFocus();
    }
}

void TabBrauuebersicht::on_cbAuswahlL3_currentIndexChanged(int)
{
    if (ui->cbAuswahlL3->hasFocus())
    {
        updateDiagram();
        ui->table->setFocus();
    }
}
