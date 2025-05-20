#include "dlgbrauuebersicht.h"
#include "ui_dlgbrauuebersicht.h"
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
        ProxyModelSud(parent),
        mColAuswahl1(-1),
        mColAuswahl2(-1),
        mColAuswahl3(-1)
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
    DlgAbstract(staticMetaObject.className(), parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DlgBrauUebersicht)
{
    ui->setupUi(this);
    if (gSettings->theme() == Qt::ColorScheme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QStringLiteral(":/images/dark/%1.svg").arg(name));
                button->setIcon(icon);
            }
        }
    }

    ui->splitter->setSizes({INT_MAX, INT_MAX});

    ProxyModelBrauuebersicht *model = new ProxyModelBrauuebersicht(this);
    model->setSourceModel(bh->modelSud());
    model->setFilterStatus(ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    model->sort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    ui->tableView->setModel(model);

    modulesChanged(Settings::ModuleAlle);
    connect(gSettings, &Settings::modulesChanged, this, &DlgBrauUebersicht::modulesChanged);

    connect(bh->modelSud(), &ModelSud::layoutChanged, this, &DlgBrauUebersicht::updateDiagram);
    connect(bh->modelSud(), &ModelSud::rowsInserted, this, &DlgBrauUebersicht::updateDiagram);
    connect(bh->modelSud(), &ModelSud::rowsRemoved, this, &DlgBrauUebersicht::updateDiagram);
    connect(bh->modelSud(), &ModelSud::dataChanged, this, &DlgBrauUebersicht::modelDataChanged);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,this, &DlgBrauUebersicht::table_selectionChanged);
    connect(ui->diagram, &Chart3::selectionChanged, this, &DlgBrauUebersicht::diagram_selectionChanged);

    setFilterDate();
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
    gSettings->setValue("ZeitraumVon", ui->tbDatumVon->date());
    gSettings->setValue("ZeitraumBis", ui->tbDatumBis->date());
    gSettings->setValue("ZeitraumAlle", ui->cbDatumAlle->isChecked());
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
    ui->tbDatumVon->setDate(gSettings->value("ZeitraumVon", QDate::currentDate().addYears(-1)).toDate());
    ui->tbDatumBis->setDate(gSettings->value("ZeitraumBis", QDate::currentDate()).toDate());
    ui->cbDatumAlle->setChecked(gSettings->value("ZeitraumAlle", false).toBool());
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

void DlgBrauUebersicht::showEvent(QShowEvent *event)
{
    DlgAbstract::showEvent(event);
    updateDiagram();
}

void DlgBrauUebersicht::modulesChanged(Settings::Modules modules)
{
    if (modules.testFlag(Settings::ModulePreiskalkulation))
    {
        build();
    }
}

void DlgBrauUebersicht::build()
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

    ui->tableView->clearCols();
    ui->tableView->appendCol({ModelSud::ColSudname, true, false, 200, new TextDelegate(ui->tableView)});
    ui->tableView->appendCol({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(ui->tableView)});
    ui->tableView->appendCol({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, ui->tableView)});
    ui->tableView->appendCol({ModelSud::ColBraudatum, true, false, 100, new DateDelegate(false, false, ui->tableView)});
    ui->cbAuswahlL1->clear();
    ui->cbAuswahlL2->clear();
    ui->cbAuswahlL3->clear();
    ui->cbAuswahlL2->addItem(mAuswahlListe[0].label);
    ui->cbAuswahlL3->addItem(mAuswahlListe[0].label);
    for (int i = 1; i < mAuswahlListe.count(); ++i)
    {
        ui->tableView->appendCol({mAuswahlListe[i].col, true, true, 80, new DoubleSpinBoxDelegate(mAuswahlListe[i].precision, ui->tableView)});
        ui->cbAuswahlL1->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL2->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL3->addItem(mAuswahlListe[i].label);
    }
    ui->tableView->build();
    ui->tableView->setDefaultContextMenu();
}

void DlgBrauUebersicht::modelDataChanged(const QModelIndex& index)
{
    switch (index.column())
    {
    case ModelSud::ColStatus:
        ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->tableView->model());
        model->invalidate();
        break;
    }
    updateDiagram();
}

void DlgBrauUebersicht::updateDiagram()
{
    ui->diagram->clear();
    ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->tableView->model());
    if (model->rowCount() > 1)
    {
        int index = ui->cbAuswahlL1->currentIndex() + 1;
        if (index >= 0 && index < mAuswahlListe.size())
        {
            AuswahlType *auswahl1 = &mAuswahlListe[index];
            model->mColAuswahl1 = auswahl1->col;
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
            model->mColAuswahl2 = auswahl2->col;
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
            model->mColAuswahl3 = auswahl3->col;
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
    table_selectionChanged(ui->tableView->selectionModel()->selection());
    ui->diagram->rescale();
    ui->diagram->replot();
}

void DlgBrauUebersicht::table_selectionChanged(const QItemSelection &selected)
{
    int index = -1;
    if (selected.indexes().count() > 0)
        index = ui->tableView->model()->rowCount() - 1 - selected.indexes()[0].row();
    ui->diagram->select(index);
}

void DlgBrauUebersicht::diagram_selectionChanged(int index)
{
    ui->tableView->selectRow(ui->tableView->model()->rowCount() - 1 - index);
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

void DlgBrauUebersicht::setFilterDate()
{
    bool notAll = ui->cbDatumAlle->isChecked();
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableView->model());
    if (notAll)
    {
        model->setFilterMinimumDate(QDateTime(ui->tbDatumVon->date(), QTime(0,0,0)));
        model->setFilterMaximumDate(QDateTime(ui->tbDatumBis->date(), QTime(23,59,59)));
    }
    model->setFilterDate(notAll);
    ui->tbDatumVon->setEnabled(notAll);
    ui->tbDatumBis->setEnabled(notAll);
    updateDiagram();
}

void DlgBrauUebersicht::on_tbDatumVon_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    setFilterDate();
}

void DlgBrauUebersicht::on_tbDatumBis_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    setFilterDate();
}

void DlgBrauUebersicht::on_cbDatumAlle_stateChanged(int state)
{
    Q_UNUSED(state)
    setFilterDate();
}
