#include "tabausruestung.h"
#include "ui_tabausruestung.h"
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodel.h"
#include "model/proxymodelsudcolored.h"
#include "model/textdelegate.h"
#include "model/spinboxdelegate.h"
#include "model/doublespinboxdelegate.h"
#include "model/checkboxdelegate.h"
#include "model/comboboxdelegate.h"
#include "model/datedelegate.h"
#include "dialogs/dlgverdampfung.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

QList<QPair<QString, int> > TabAusruestung::Typname = {
    {tr("Standard"), static_cast<int>(Brauhelfer::AnlageTyp::Standard)},
    {tr("Grainfather G30"), static_cast<int>(Brauhelfer::AnlageTyp::GrainfatherG30)},
    {tr("Grainfather G70"), static_cast<int>(Brauhelfer::AnlageTyp::GrainfatherG70)},
    {tr("Braumeister 10L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister10)},
    {tr("Braumeister 20L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister20)},
    {tr("Braumeister 50L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister50)},
    {tr("Braumeister 200L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister200)},
    {tr("Braumeister 500L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister500)},
    {tr("Braumeister 1000L"), static_cast<int>(Brauhelfer::AnlageTyp::Braumeister1000)},
    {tr("Brauheld Pro 30L"), static_cast<int>(Brauhelfer::AnlageTyp::BrauheldPro30)}
};

TabAusruestung::TabAusruestung(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabAusruestung)
{
    ui->setupUi(this);
    ui->lblCurrency->setText(QLocale().currencySymbol());

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabAusruestung");

    TableView *table = ui->tableViewAnlagen;
    ProxyModel *model = new ProxyModel(this);
    model->setSourceModel(bh->modelAusruestung());
    model->setHeaderData(ModelAusruestung::ColName, Qt::Horizontal, tr("Anlage"));
    model->setHeaderData(ModelAusruestung::ColTyp, Qt::Horizontal, tr("Typ"));
    model->setHeaderData(ModelAusruestung::ColVermoegen, Qt::Horizontal, tr("Vermögen [l]"));
    model->setHeaderData(ModelAusruestung::ColAnzahlSude, Qt::Horizontal, tr("Anzahl Sude"));
    table->setModel(model);
    table->cols.append({ModelAusruestung::ColName, true, false, -1, nullptr});
    table->cols.append({ModelAusruestung::ColTyp, true, true, 100, new ComboBoxDelegate(Typname, table)});
    table->cols.append({ModelAusruestung::ColVermoegen, true, true, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelAusruestung::ColAnzahlSude, true, true, 100, new SpinBoxDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateAnlagen").toByteArray());

    table = ui->tableViewGeraete;
    model = new ProxyModel(this);
    model->setSourceModel(bh->modelGeraete());
    model->setHeaderData(ModelGeraete::ColBezeichnung, Qt::Horizontal, tr("Bezeichnung"));
    model->setFilterKeyColumn(ModelGeraete::ColAusruestungAnlagenID);
    table->setModel(model);
    table->cols.append({ModelGeraete::ColBezeichnung, true, false, -1, nullptr});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateGeraete").toByteArray());

    table = ui->tableViewSude;
    model = new ProxyModelSudColored(this);
    model->setSourceModel(bh->modelSud());
    model->setFilterKeyColumn(ModelSud::ColAnlage);
    table->setModel(model);
    table->cols.append({ModelSud::ColSudname, true, false, 200, new TextDelegate(true, Qt::AlignLeft | Qt::AlignVCenter, table)});
    table->cols.append({ModelSud::ColSudnummer, true, true, 80, new SpinBoxDelegate(table)});
    table->cols.append({ModelSud::ColKategorie, true, true, 100, new TextDelegate(false, Qt::AlignCenter, table)});
    table->cols.append({ModelSud::ColBraudatum, true, false, 100, new DateDelegate(false, true, table)});
    table->cols.append({ModelSud::Colerg_EffektiveAusbeute, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelSud::ColVerdampfungsrateIst, true, false, 100, new DoubleSpinBoxDelegate(1, table)});
    table->cols.append({ModelSud::ColAusbeuteIgnorieren, true, false, 150, new CheckBoxDelegate(table)});
    table->build();
    table->setDefaultContextMenu();
    table->restoreState(gSettings->value("tableStateSude").toByteArray());

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    mDefaultSplitterLeftState = ui->splitterLeft->saveState();
    ui->splitterLeft->restoreState(gSettings->value("splitterLeftState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());

    ui->sliderAusbeuteSude->setValue(gSettings->value("AnzahlDurchschnitt").toInt());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->modelSud(), SIGNAL(modified()), this, SLOT(updateDurchschnitt()));
    connect(bh->modelAusruestung(), SIGNAL(modified()), this, SLOT(updateValues()));
    connect(ui->tableViewAnlagen->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(anlage_selectionChanged()));

    sudLoaded();
}

TabAusruestung::~TabAusruestung()
{
    delete ui;
}

void TabAusruestung::saveSettings()
{
    gSettings->beginGroup("TabAusruestung");
    gSettings->setValue("tableStateAnlagen", ui->tableViewAnlagen->horizontalHeader()->saveState());
    gSettings->setValue("tableStateGeraete", ui->tableViewGeraete->horizontalHeader()->saveState());
    gSettings->setValue("tableStateSude", ui->tableViewSude->horizontalHeader()->saveState());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterLeftState", ui->splitterLeft->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->setValue("AnzahlDurchschnitt", ui->sliderAusbeuteSude->value());
    gSettings->endGroup();
}

void TabAusruestung::restoreView(bool full)
{
    ui->tableViewAnlagen->restoreDefaultState();
    ui->tableViewGeraete->restoreDefaultState();
    ui->tableViewSude->restoreDefaultState();
    if (full)
    {
        ui->splitter->restoreState(mDefaultSplitterState);
        ui->splitterLeft->restoreState(mDefaultSplitterLeftState);
        ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
    }
}

void TabAusruestung::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (ui->tableViewAnlagen->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnAnlageLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnNeueAnlage_clicked();
            break;
        }
    }
    else if (ui->tableViewGeraete->hasFocus())
    {
        switch (event->key())
        {
        case Qt::Key::Key_Delete:
            on_btnGeraetLoeschen_clicked();
            break;
        case Qt::Key::Key_Insert:
            on_btnNeuesGeraet_clicked();
            break;
        }
    }
}

void TabAusruestung::focusChanged(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)
    if (now && now != ui->tbHelp && now != ui->splitterHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabAusruestung::sudLoaded()
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    int row = 0;
    if (bh->sud()->isLoaded())
        row = model->getRowWithValue(ModelAusruestung::ColName, bh->sud()->getAnlage());
    ui->tableViewAnlagen->setCurrentIndex(model->index(row, ModelAusruestung::ColName));
}

void TabAusruestung::anlage_selectionChanged()
{
    QRegularExpression regExpId;
    QRegularExpression regExpId2;
    QModelIndexList selectedRows = ui->tableViewAnlagen->selectionModel()->selectedRows();
    if (selectedRows.count() > 0)
    {
        mRow = selectedRows[0].row();
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
        QString anlage = model->data(model->index(mRow, ModelAusruestung::ColName)).toString();
        regExpId = QRegularExpression(QString("^%1$").arg(QRegularExpression::escape(anlage)));
        regExpId2 = QRegularExpression(QString("^%1$").arg(model->data(model->index(mRow, ModelAusruestung::ColID)).toInt()));
    }
    else
    {
        regExpId = QRegularExpression(QString("--dummy--"));
        regExpId2 = QRegularExpression(QString("--dummy--"));
    }
    static_cast<QSortFilterProxyModel*>(ui->tableViewGeraete->model())->setFilterRegularExpression(regExpId2);
    static_cast<QSortFilterProxyModel*>(ui->tableViewSude->model())->setFilterRegularExpression(regExpId);
    ui->sliderAusbeuteSude->setMaximum(9999);
    if (ui->sliderAusbeuteSude->value() == 0)
        ui->sliderAusbeuteSude->setValue(9999);
    updateValues();
    updateDurchschnitt();
}

void TabAusruestung::on_btnNeueAnlage_clicked()
{
    QMap<int, QVariant> values({{ModelAusruestung::ColName, tr("Neue Brauanlage")}});
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    int row = model->append(values);
    if (row >= 0)
    {
        const QModelIndex index = model->index(row, ModelAusruestung::ColName);
        ui->tableViewAnlagen->setCurrentIndex(index);
        ui->tableViewAnlagen->scrollTo(ui->tableViewAnlagen->currentIndex());
        ui->tableViewAnlagen->edit(ui->tableViewAnlagen->currentIndex());
    }
}

void TabAusruestung::on_btnAnlageLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableViewAnlagen->model());
    QModelIndexList indices = ui->tableViewAnlagen->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        QString name = model->data(index.row(), ModelAusruestung::ColName).toString();
        int ret = QMessageBox::question(this, tr("Brauanlage löschen?"),
                                        tr("Soll die Brauanlage \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
            int row = model->getRowWithValue(ModelAusruestung::ColName, name);
            model->removeRow(row);
        }
    }
}

void TabAusruestung::on_btnNeuesGeraet_clicked()
{
    QModelIndexList selected = ui->tableViewAnlagen->selectionModel()->selectedRows();
    if (selected.count() > 0)
    {
        QMap<int, QVariant> values({{ModelGeraete::ColAusruestungAnlagenID, data(ModelAusruestung::ColID)},
                                    {ModelGeraete::ColBezeichnung, tr("Neues Gerät")}});
        ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewGeraete->model());
        int row = model->append(values);
        if (row >= 0)
        {
            const QModelIndex index = model->index(row, ModelGeraete::ColBezeichnung);
            ui->tableViewGeraete->setCurrentIndex(index);
            ui->tableViewGeraete->scrollTo(ui->tableViewGeraete->currentIndex());
            ui->tableViewGeraete->edit(ui->tableViewGeraete->currentIndex());
        }
    }
}

void TabAusruestung::on_btnGeraetLoeschen_clicked()
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableViewGeraete->model());
    QModelIndexList indices = ui->tableViewGeraete->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
    {
        QString name = model->data(index.row(), ModelGeraete::ColBezeichnung).toString();
        int ret = QMessageBox::question(this, tr("Gerät löschen?"),
                                        tr("Soll das Gerät \"%1\" gelöscht werden?").arg(name));
        if (ret == QMessageBox::Yes)
        {
            int row = model->getRowWithValue(ModelGeraete::ColBezeichnung, name);
            model->removeRow(row);
        }
    }
}

QVariant TabAusruestung::data(int col) const
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->data(mRow, col);
}

bool TabAusruestung::setData(int col, const QVariant &value)
{
    ProxyModel *model = static_cast<ProxyModel*>(ui->tableViewAnlagen->model());
    return model->setData(mRow, col, value);
}

void TabAusruestung::updateValues()
{
    ui->lblAnlage->setText(data(ModelAusruestung::ColName).toString());
    if (!ui->tbAusbeute->hasFocus())
        ui->tbAusbeute->setValue(data(ModelAusruestung::ColSudhausausbeute).toDouble());
    if (!ui->tbVerdampfung->hasFocus())
        ui->tbVerdampfung->setValue(data(ModelAusruestung::ColVerdampfungsrate).toDouble());
    if (!ui->tbKorrekturNachguss->hasFocus())
        ui->tbKorrekturNachguss->setValue(data(ModelAusruestung::ColKorrekturWasser).toDouble());
    if (!ui->tbKorrekturFarbe->hasFocus())
        ui->tbKorrekturFarbe->setValue(data(ModelAusruestung::ColKorrekturFarbe).toInt());
    if (!ui->tbKorrekturSollmenge->hasFocus())
        ui->tbKorrekturSollmenge->setValue(data(ModelAusruestung::ColKorrekturMenge).toInt());
    if (!ui->tbKosten->hasFocus())
        ui->tbKosten->setValue(data(ModelAusruestung::ColKosten).toDouble());
    if (!ui->tbMaischebottichHoehe->hasFocus())
        ui->tbMaischebottichHoehe->setValue(data(ModelAusruestung::ColMaischebottich_Hoehe).toDouble());
    if (!ui->tbMaischebottichDurchmesser->hasFocus())
        ui->tbMaischebottichDurchmesser->setValue(data(ModelAusruestung::ColMaischebottich_Durchmesser).toDouble());
    ui->tbMaischebottichMaxFuellhoehe->setMaximum(data(ModelAusruestung::ColMaischebottich_Hoehe).toDouble());
    if (!ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        ui->tbMaischebottichMaxFuellhoehe->setValue(data(ModelAusruestung::ColMaischebottich_MaxFuellhoehe).toDouble());
    if (!ui->tbSudpfanneHoehe->hasFocus())
        ui->tbSudpfanneHoehe->setValue(data(ModelAusruestung::ColSudpfanne_Hoehe).toDouble());
    if (!ui->tbSudpfanneDurchmesser->hasFocus())
        ui->tbSudpfanneDurchmesser->setValue(data(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble());
    ui->tbSudpfanneMaxFuellhoehe->setMaximum(data(ModelAusruestung::ColSudpfanne_Hoehe).toDouble());
    if (!ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        ui->tbSudpfanneMaxFuellhoehe->setValue(data(ModelAusruestung::ColSudpfanne_MaxFuellhoehe).toDouble());
    ui->tbMaischenVolumen->setValue(data(ModelAusruestung::ColMaischebottich_Volumen).toDouble());
    ui->tbMaischenMaxNutzvolumen->setValue(data(ModelAusruestung::ColMaischebottich_MaxFuellvolumen).toDouble());
    ui->tbSudpfanneVolumen->setValue(data(ModelAusruestung::ColSudpfanne_Volumen).toDouble());
    ui->tbSudpfanneMaxNutzvolumen->setValue(data(ModelAusruestung::ColSudpfanne_MaxFuellvolumen).toDouble());
    if (!ui->tbBemerkung->hasFocus())
        ui->tbBemerkung->setText(data(ModelAusruestung::ColBemerkung).toString());
}

void TabAusruestung::updateDurchschnitt()
{
    ProxyModelSud model;
    model.setSourceModel(bh->modelSud());
    model.setFilterStatus(ProxyModelSud::Gebraut | ProxyModelSud::Abgefuellt | ProxyModelSud::Verbraucht);
    model.sort(ModelSud::ColBraudatum, Qt::DescendingOrder);
    QString anlage = data(ModelAusruestung::ColName).toString();
    int nAusbeute = 0;
    int nVerdampfung = 0;
    int N = 0;
    double ausbeute = 0.0;
    double verdampfung = 0.0;
    for (int i = 0; i < model.rowCount(); ++i)
    {
        if (model.index(i, ModelSud::ColAnlage).data().toString() == anlage)
        {
            if (!model.index(i, ModelSud::ColAusbeuteIgnorieren).data().toBool())
            {
                if (N < ui->sliderAusbeuteSude->value())
                {
                    double val = model.index(i, ModelSud::Colerg_EffektiveAusbeute).data().toDouble();
                    if (val > 0)
                    {
                        ausbeute += val;
                        nAusbeute++;
                    }
                    val = model.index(i, ModelSud::ColVerdampfungsrateIst).data().toDouble();
                    if (val > 0)
                    {
                        verdampfung += val;
                        ++nVerdampfung;
                    }
                }
                ++N;
            }
        }
    }
    if (nAusbeute > 0)
        ausbeute /= nAusbeute;
    if (nVerdampfung > 0)
        verdampfung /= nVerdampfung;
    ui->sliderAusbeuteSude->setMaximum(N);
    ui->sliderAusbeuteSude->setEnabled(N > 1);
    ui->tbAusbeuteMittel->setValue(ausbeute);
    ui->tbVerdampfungMittel->setValue(verdampfung);
    ui->tbAusbeuteSude->setValue(ui->sliderAusbeuteSude->value());
}

void TabAusruestung::on_btnVerdampfungsrate_clicked()
{
    DlgVerdampfung dlg;
    dlg.setDurchmesser(data(ModelAusruestung::ColSudpfanne_Durchmesser).toDouble());
    dlg.setHoehe(data(ModelAusruestung::ColSudpfanne_Hoehe).toDouble());
    if (bh->sud()->isLoaded())
    {
        dlg.setKochdauer(bh->sud()->getKochdauer());
        dlg.setMenge1(BierCalc::volumenWasser(20, 100, bh->sud()->getWuerzemengeKochbeginn()));
        dlg.setMenge2(BierCalc::volumenWasser(20, 100, bh->sud()->getWuerzemengeVorHopfenseihen()));
    }
    else
    {
        dlg.setKochdauer(90);
        dlg.setMenge1(20);
        dlg.setMenge2(18);
    }
    if (dlg.exec() == QDialog::Accepted)
    {
        setData(ModelAusruestung::ColVerdampfungsrate, dlg.getVerdampfungsrate());
    }
}

void TabAusruestung::on_tbAusbeute_valueChanged(double value)
{
    if (ui->tbAusbeute->hasFocus())
        setData(ModelAusruestung::ColSudhausausbeute, value);
}

void TabAusruestung::on_btnAusbeuteMittel_clicked()
{
    setData(ModelAusruestung::ColSudhausausbeute, ui->tbAusbeuteMittel->value());
}

void TabAusruestung::on_tbVerdampfung_valueChanged(double value)
{
    if (ui->tbVerdampfung->hasFocus())
        setData(ModelAusruestung::ColVerdampfungsrate, value);
}

void TabAusruestung::on_btnVerdampfungMittel_clicked()
{
    setData(ModelAusruestung::ColVerdampfungsrate, ui->tbVerdampfungMittel->value());
}

void TabAusruestung::on_sliderAusbeuteSude_sliderMoved(int)
{
    updateDurchschnitt();
}

void TabAusruestung::on_tbKorrekturNachguss_valueChanged(double value)
{
    if (ui->tbKorrekturNachguss->hasFocus())
        setData(ModelAusruestung::ColKorrekturWasser, value);
}

void TabAusruestung::on_tbKorrekturFarbe_valueChanged(int value)
{
    if (ui->tbKorrekturFarbe->hasFocus())
        setData(ModelAusruestung::ColKorrekturFarbe, value);
}

void TabAusruestung::on_tbKorrekturSollmenge_valueChanged(double value)
{
    if (ui->tbKorrekturSollmenge->hasFocus())
        setData(ModelAusruestung::ColKorrekturMenge, value);
}

void TabAusruestung::on_tbKosten_valueChanged(double value)
{
    if (ui->tbKosten->hasFocus())
        setData(ModelAusruestung::ColKosten, value);
}

void TabAusruestung::on_tbMaischebottichHoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichHoehe->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_Hoehe, value);
}

void TabAusruestung::on_tbMaischebottichDurchmesser_valueChanged(double value)
{
    if (ui->tbMaischebottichDurchmesser->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_Durchmesser, value);
}

void TabAusruestung::on_tbMaischebottichMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbMaischebottichMaxFuellhoehe->hasFocus())
        setData(ModelAusruestung::ColMaischebottich_MaxFuellhoehe, value);
}

void TabAusruestung::on_tbSudpfanneHoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneHoehe->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_Hoehe, value);
}

void TabAusruestung::on_tbSudpfanneDurchmesser_valueChanged(double value)
{
    if (ui->tbSudpfanneDurchmesser->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_Durchmesser, value);
}

void TabAusruestung::on_tbSudpfanneMaxFuellhoehe_valueChanged(double value)
{
    if (ui->tbSudpfanneMaxFuellhoehe->hasFocus())
        setData(ModelAusruestung::ColSudpfanne_MaxFuellhoehe, value);
}

void TabAusruestung::on_tbBemerkung_textChanged()
{
    if (ui->tbBemerkung->hasFocus())
        setData(ModelAusruestung::ColBemerkung, ui->tbBemerkung->toPlainText());
}
