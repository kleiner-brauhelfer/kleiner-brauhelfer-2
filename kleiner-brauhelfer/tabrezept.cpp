#include "tabrezept.h"
#include "ui_tabrezept.h"
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "model/checkboxdelegate.h"
#include "widgets/wdgrast.h"
#include "widgets/wdgmalzgabe.h"
#include "widgets/wdghopfengabe.h"
#include "widgets/wdghefegabe.h"
#include "widgets/wdgweiterezutatgabe.h"
#include "widgets/wdganhang.h"
#include "dialogs/dlgrohstoffaustausch.h"
#include "dialogs/dlgeinmaischtemp.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabRezept::TabRezept(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabRezept)
{
    ui->setupUi(this);
    mGlasSvg = new QGraphicsSvgItem(":/images/bier.svg");
    ui->lblCurrency->setText(QLocale().currencySymbol() + "/" + tr("l"));

    QChart *chart = ui->diagramMalz->chart();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setAlignment(Qt::AlignRight);

    chart = ui->diagramHopfen->chart();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->setAlignment(Qt::AlignRight);

    chart = ui->diagramRasten->chart();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->legend()->hide();

    QPalette palette = ui->tbHelp->palette();
    palette.setBrush(QPalette::Base, palette.brush(QPalette::ToolTipBase));
    palette.setBrush(QPalette::Text, palette.brush(QPalette::ToolTipText));
    ui->tbHelp->setPalette(palette);

    gSettings->beginGroup("TabRezept");

    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->splitterHelp->setStretchFactor(0, 1);
    ui->splitterHelp->setStretchFactor(1, 0);
    ui->splitterHelp->setSizes({90, 10});
    mDefaultSplitterHelpState = ui->splitterHelp->saveState();
    ui->splitterHelp->restoreState(gSettings->value("splitterHelpState").toByteArray());
    ui->splitterMalzDiagram->restoreState(gSettings->value("splitterMalzDiagramState").toByteArray());
    ui->splitterHopfenDiagram->restoreState(gSettings->value("splitterHopfenDiagramState").toByteArray());
    ui->splitterRastenDiagram->restoreState(gSettings->value("splitterRastenDiagramState").toByteArray());

    gSettings->endGroup();

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(focusChanged(QWidget*,QWidget*)));

    connect(bh, SIGNAL(modified()), this, SLOT(updateValues()));
    connect(bh, SIGNAL(discarded()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(sudLoaded()));
    connect(bh->sud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
                    this, SLOT(sudDataChanged(const QModelIndex&)));

    connect(bh->sud()->modelRasten(), SIGNAL(layoutChanged()), this, SLOT(rasten_modified()));
    connect(bh->sud()->modelRasten(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(updateRastenDiagram()));

    connect(bh->sud()->modelMalzschuettung(), SIGNAL(layoutChanged()), this, SLOT(malzGaben_modified()));
    connect(bh->sud()->modelMalzschuettung(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(malzGaben_dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));

    connect(bh->sud()->modelHopfengaben(), SIGNAL(layoutChanged()), this, SLOT(hopfenGaben_modified()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(hopfenGaben_dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));

    // TODO
    //connect(bh->sud()->modelHefeGaben(), SIGNAL(layoutChanged()), this, SLOT(hefeGaben_modified()));

    connect(bh->sud()->modelWeitereZutatenGaben(), SIGNAL(layoutChanged()), this, SLOT(weitereZutatenGaben_modified()));

    connect(bh->sud()->modelAnhang(), SIGNAL(layoutChanged()), this, SLOT(anhaenge_modified()));

    int col;
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    QTableView *table = ui->tableTags;
    table->setModel(model);
    for (int col = 0; col < model->columnCount(); ++col)
        table->setColumnHidden(col, true);
    col = model->fieldIndex("Tagname");
    model->setHeaderData(col, Qt::Horizontal, tr("Tag"));
    table->setColumnHidden(col, false);
    col = model->fieldIndex("Value");
    model->setHeaderData(col, Qt::Horizontal, tr("Wert"));
    table->setColumnHidden(col, false);
    table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    col = model->fieldIndex("Global");
    model->setHeaderData(col, Qt::Horizontal, tr("Global"));
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new CheckBoxDelegate(table));
}

TabRezept::~TabRezept()
{
    delete ui;
    delete mGlasSvg;
}

void TabRezept::saveSettings()
{
    gSettings->beginGroup("TabRezept");
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->setValue("splitterHelpState", ui->splitterHelp->saveState());
    gSettings->setValue("splitterMalzDiagramState", ui->splitterMalzDiagram->saveState());
    gSettings->setValue("splitterHopfenDiagramState", ui->splitterHopfenDiagram->saveState());
    gSettings->setValue("splitterRastenDiagramState", ui->splitterRastenDiagram->saveState());
    gSettings->endGroup();
}

void TabRezept::restoreView()
{
    ui->splitter->restoreState(mDefaultSplitterState);
    ui->splitterHelp->restoreState(mDefaultSplitterHelpState);
}

void TabRezept::focusChanged(QWidget *old, QWidget *now)
{
    if (old == ui->tbKommentar)
        ui->tbKommentar->setHtml(bh->sud()->getKommentar().replace("\n", "<br>"));
    if (now == ui->tbKommentar)
        ui->tbKommentar->setPlainText(bh->sud()->getKommentar());
    if (now && now != ui->tbHelp)
        ui->tbHelp->setHtml(now->toolTip());
}

void TabRezept::sudLoaded()
{
    checkEnabled();
    ui->cbAnlage->setCurrentIndex(-1);
    ui->cbWasserProfil->setCurrentIndex(-1);
    if (bh->sud()->isLoaded())
    {
        updateAnlageModel();
        updateWasserModel();
        updateValues();
        hefeGaben_modified();// TODO: remove
        checkRohstoffe();
    }
}

void TabRezept::sudDataChanged(const QModelIndex& index)
{
    const SqlTableModel* model = static_cast<const SqlTableModel*>(index.model());
    QString fieldname = model->fieldName(index.column());
    if (fieldname == "BierWurdeGebraut" || fieldname == "BierWurdeAbgefuellt")
    {
        checkEnabled();
        updateAnlageModel();
        updateWasserModel();
        rasten_modified();
        malzGaben_modified();
        hopfenGaben_modified();
        hefeGaben_modified();
        weitereZutatenGaben_modified();
        checkRohstoffe();
    }
    else if (fieldname == "EinmaischenTemp")
    {
        updateRastenDiagram();
    }
}

void TabRezept::checkEnabled()
{
    bool gebraut = bh->sud()->getBierWurdeGebraut();
    bool abgefuellt = bh->sud()->getBierWurdeAbgefuellt();
    ui->cbAnlage->setEnabled(!gebraut);
    ui->tbMenge->setReadOnly(gebraut);
    ui->tbSW->setReadOnly(gebraut);
    ui->tbFaktorHauptguss->setReadOnly(gebraut);
    ui->tbHGF->setReadOnly(gebraut);
    ui->tbBittere->setReadOnly(gebraut);
    ui->tbKochzeit->setReadOnly(gebraut);
    ui->tbNachisomerisierungszeit->setReadOnly(gebraut);
    ui->tbCO2->setReadOnly(gebraut);
    ui->cbWasserProfil->setEnabled(!gebraut);
    ui->tbRestalkalitaet->setReadOnly(gebraut);
    ui->tbReifezeit->setReadOnly(gebraut);
    ui->tbEinmaischtemperatur->setReadOnly(gebraut);
    ui->btnEinmaischtemperatur->setVisible(!gebraut);
    ui->btnNeueRast->setVisible(!gebraut);
    ui->lineNeueRast->setVisible(!gebraut);
    ui->btnNeueMalzGabe->setVisible(!gebraut);
    ui->lineNeueMalzGabe->setVisible(!gebraut);
    ui->cbBerechnungsartHopfen->setEnabled(!gebraut);
    ui->btnNeueHopfenGabe->setVisible(!gebraut);
    ui->lineNeueHopfenGabe->setVisible(!gebraut);
    ui->btnNeueHefeGabe->setVisible(!abgefuellt);
    ui->lineNeueHefeGabe->setVisible(!gebraut);
    ui->btnNeueHopfenstopfenGabe->setVisible(!abgefuellt);
    ui->btnNeueWeitereZutat->setVisible(!abgefuellt);
    ui->lineNeueWeitereZutat->setVisible(!gebraut);
}

void TabRezept::checkRohstoffe()
{
    QWidget *wdg = ui->tabZutaten->currentWidget();
    DlgRohstoffAustausch dlg(DlgRohstoffAustausch::NichtVorhanden, QString(), this);
    dlg.setSud(bh->sud()->getSudname());

    dlg.setModel(bh->modelMalz(), bh->modelMalz()->fieldIndex("Beschreibung"));
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data("Name").toString();
        if (name.isEmpty())
            continue;
        int row = bh->modelMalz()->getRowWithValue("Beschreibung", name);
        if (row == -1)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabMalz);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QVariantMap values({{"Beschreibung", name},
                                        {"Farbe", wdg->data("Farbe")}});
                    bh->modelMalz()->append(values);
                    dlg.setModel(bh->modelMalz(), bh->modelMalz()->fieldIndex("Beschreibung"));
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData("Name", dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    dlg.setModel(bh->modelHopfen(), bh->modelHopfen()->fieldIndex("Beschreibung"));
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data("Name").toString();
        if (name.isEmpty())
            continue;
        int row = bh->modelHopfen()->getRowWithValue("Beschreibung", name);
        if (row == -1)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabHopfen);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QVariantMap values({{"Beschreibung", name},
                                        {"Alpha", wdg->data("Alpha")},
                                        {"Pellets", wdg->data("Pellets")}});
                    bh->modelHopfen()->append(values);
                    dlg.setModel(bh->modelHopfen(), bh->modelHopfen()->fieldIndex("Beschreibung"));
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData("Name", dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    dlg.setModel(bh->modelHefe(), bh->modelHefe()->fieldIndex("Beschreibung"));
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
    {
        WdgHefeGabe* wdg = static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data("Name").toString();
        if (name.isEmpty())
            continue;
        int row = bh->modelHefe()->getRowWithValue("Beschreibung", name);
        if (row == -1)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabHefe);
            dlg.setRohstoff(name);
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    QVariantMap values({{"Beschreibung", name}});
                    bh->modelHefe()->append(values);
                    dlg.setModel(bh->modelHefe(), bh->modelHefe()->fieldIndex("Beschreibung"));
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData("Name", dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
    {
        WdgWeitereZutatGabe* wdg = static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget());
        if (!wdg->isEnabled())
            continue;
        QString name = wdg->data("Name").toString();
        if (name.isEmpty())
            continue;
        int typ = wdg->data("Typ").toInt();
        int row;
        if (typ == EWZ_Typ_Hopfen)
            row = bh->modelHopfen()->getRowWithValue("Beschreibung", name);
        else
            row = bh->modelWeitereZutaten()->getRowWithValue("Beschreibung", name);
        if (row == -1)
        {
            ui->tabZutaten->setCurrentWidget(ui->tabWeitereZutaten);
            dlg.setRohstoff(name);
            if (typ == EWZ_Typ_Hopfen)
                dlg.setModel(bh->modelHopfen(), bh->modelHopfen()->fieldIndex("Beschreibung"));
            else
                dlg.setModel(bh->modelWeitereZutaten(), bh->modelWeitereZutaten()->fieldIndex("Beschreibung"));
            if (dlg.exec() == QDialog::Accepted)
            {
                if (dlg.importieren())
                {
                    if (typ == EWZ_Typ_Hopfen)
                    {
                        QVariantMap values({{"Beschreibung", name},
                                            {"Alpha", wdg->data("Alpha")},
                                            {"Pellets", wdg->data("Pellets")}});
                        bh->modelHopfen()->append(values);
                    }
                    else
                    {
                        QVariantMap values({{"Beschreibung", name},
                                            {"Einheiten", wdg->data("Einheit")},
                                            {"Typ", wdg->data("Typ")},
                                            {"Ausbeute", wdg->data("Ausbeute")},
                                            {"EBC", wdg->data("Farbe")}});
                        bh->modelWeitereZutaten()->append(values);
                    }
                    wdg->updateValues();
                }
                else
                {
                    wdg->setData("Name", dlg.rohstoff());
                }
            }
            else
            {
                bh->sud()->unload();
                return;
            }
        }
    }

    ui->tabZutaten->setCurrentWidget(wdg);
}

void TabRezept::updateValues()
{
    double f;
    if (!ui->tbSudnummer->hasFocus())
        ui->tbSudnummer->setValue(bh->sud()->getSudnummer());
    if (!ui->tbSudname->hasFocus())
        ui->tbSudname->setText(bh->sud()->getSudname());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(bh->sud()->getMenge());
    if (!ui->tbSW->hasFocus())
        ui->tbSW->setValue(bh->sud()->getSW());
    if (!ui->tbCO2->hasFocus())
        ui->tbCO2->setValue(bh->sud()->getCO2());
    if (!ui->tbBittere->hasFocus())
        ui->tbBittere->setValue(bh->sud()->getIBU());
    ui->tbFarbe->setValue((int)bh->sud()->geterg_Farbe());
    ui->tbGesamtschuettung->setValue(bh->sud()->geterg_S_Gesammt());
    ui->tbKosten->setValue(bh->sud()->geterg_Preis());
    if (!ui->tbReifezeit->hasFocus())
        ui->tbReifezeit->setValue(bh->sud()->getReifezeit());
    if (!ui->cbAnlage->hasFocus())
        ui->cbAnlage->setCurrentText(bh->sud()->getAuswahlBrauanlageName());
    ui->cbAnlage->setError(ui->cbAnlage->currentIndex() == -1);
    if (!ui->tbHGF->hasFocus())
        ui->tbHGF->setValue(bh->sud()->gethighGravityFaktor());
    if (!ui->tbFaktorHauptguss->hasFocus())
        ui->tbFaktorHauptguss->setValue(bh->sud()->getFaktorHauptguss());
    ui->tbFaktorHauptgussEmpfehlung->setValue(bh->sud()->getFaktorHauptgussEmpfehlung());
    if (!ui->tbRestalkalitaet->hasFocus())
        ui->tbRestalkalitaet->setValue(bh->sud()->getRestalkalitaetSoll());
    ui->tbRestalkalitaetWasser->setValue(bh->modelWasser()->data(0, "Restalkalitaet").toDouble());
    ui->tbRestalkalitaet->setMaximum(ui->tbRestalkalitaetWasser->value());
    ui->lblWasserprofil->setText(""); // TODO
    f = bh->sud()->getRestalkalitaetFaktor();
    ui->tbWasserGesamt->setValue(bh->sud()->geterg_W_Gesammt());
    ui->tbMilchsaeureGesamt->setValue(ui->tbWasserGesamt->value() * f);
    ui->tbHauptguss->setValue(bh->sud()->geterg_WHauptguss());
    ui->tbMilchsaeureHG->setValue(ui->tbHauptguss->value() * f);
    ui->tbNachguss->setValue(bh->sud()->geterg_WNachguss());
    ui->tbMilchsaeureNG->setValue(ui->tbNachguss->value() * f);
    ui->tbMilchsaeureGesamt->setVisible(f > 0.0);
    ui->lblMilchsaeureGesamt->setVisible(f > 0.0);
    ui->lblMilchsaeureGesamtEinheit->setVisible(f > 0.0);
    ui->tbMilchsaeureHG->setVisible(f > 0.0);
    ui->lblMilchsaeureHG->setVisible(f > 0.0);
    ui->lblMilchsaeureHGEinheit->setVisible(f > 0.0);
    ui->tbMilchsaeureNG->setVisible(f > 0.0);
    ui->lblMilchsaeureNG->setVisible(f > 0.0);
    ui->lblMilchsaeureNGEinheit->setVisible(f > 0.0);
    ui->lblAnlageName->setText(bh->sud()->getAuswahlBrauanlageName());
    ui->tbAnlageSudhausausbeute->setValue(bh->sud()->getAnlageValue("Sudhausausbeute").toDouble());
    ui->tbAnlageVerdampfung->setValue(bh->sud()->getAnlageValue("Verdampfungsziffer").toDouble());
    ui->tbAnlageVolumenMaische->setValue(bh->sud()->getAnlageValue("Maischebottich_MaxFuellvolumen").toDouble());
    ui->tbVolumenMaische->setValue(bh->sud()->geterg_WHauptguss() + BierCalc::MalzVerdraengung * bh->sud()->geterg_S_Gesammt());
    ui->tbVolumenMaische->setError(ui->tbVolumenMaische->value() > ui->tbAnlageVolumenMaische->value());
    ui->tbAnlageVolumenKochen->setValue(bh->sud()->getAnlageValue("Sudpfanne_MaxFuellvolumen").toDouble());
    ui->tbVolumenKochen->setValue(BierCalc::volumenWasser(20.0, 100.0, bh->sud()->getMengeSollKochbeginn()));
    ui->tbVolumenKochen->setError(ui->tbVolumenKochen->value() > ui->tbAnlageVolumenKochen->value());
    if (!ui->tbEinmaischtemperatur->hasFocus())
        ui->tbEinmaischtemperatur->setValue(bh->sud()->getEinmaischenTemp());
    if (!ui->tbKochzeit->hasFocus())
        ui->tbKochzeit->setValue(bh->sud()->getKochdauerNachBitterhopfung());
    if (!ui->tbNachisomerisierungszeit->hasFocus())
        ui->tbNachisomerisierungszeit->setValue(bh->sud()->getNachisomerisierungszeit());
    if (!ui->cbBerechnungsartHopfen->hasFocus())
        ui->cbBerechnungsartHopfen->setCurrentIndex(bh->sud()->getberechnungsArtHopfen());
    if (!ui->tbKommentar->hasFocus())
        ui->tbKommentar->setHtml(bh->sud()->getKommentar().replace("\n", "<br>"));

    ui->cbWasserProfil->setError(ui->cbWasserProfil->currentIndex() == -1);
    updateGlas();
}

void TabRezept::updateGlas()
{
    QGraphicsScene* scene = new QGraphicsScene();
    QPen pen(Qt::white);
    QBrush brush(BierCalc::ebcToColor(bh->sud()->geterg_Farbe()));
    scene->addRect(10, 10, 980, 980, pen, brush);
    scene->addItem(mGlasSvg);
    ui->gvGlas->setScene(scene);
    ui->gvGlas->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void TabRezept::updateAnlageModel()
{
    if (bh->sud()->getBierWurdeGebraut())
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbAnlage);
        model->setItem(0, 0, new QStandardItem(bh->sud()->getAuswahlBrauanlageName()));
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(0);
    }
    else
    {
        ProxyModel *model = new ProxyModel(ui->cbAnlage);
        model->setSourceModel(bh->modelAusruestung());
        ui->cbAnlage->setModel(model);
        ui->cbAnlage->setModelColumn(bh->modelAusruestung()->fieldIndex("Name"));
    }
    ui->cbAnlage->setCurrentIndex(-1);
}

void TabRezept::updateWasserModel()
{
    if (bh->sud()->getBierWurdeGebraut())
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbWasserProfil);
        // TODO:
        //model->setItem(0, 0, new QStandardItem(bh->sud()->getAuswahlBrauanlageName()));
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(0);
    }
    else
    {
        ProxyModel *model = new ProxyModel(ui->cbWasserProfil);
        model->setSourceModel(bh->modelWasser());
        ui->cbWasserProfil->setModel(model);
        ui->cbWasserProfil->setModelColumn(bh->modelWasser()->fieldIndex("Name"));
    }
    ui->cbWasserProfil->setCurrentIndex(-1);
}

void TabRezept::rasten_modified()
{    
    const int nModel = bh->sud()->modelRasten()->rowCount();
    int nLayout = ui->layoutRasten->count();
    while (nLayout < nModel)
        ui->layoutRasten->addWidget(new WdgRast(nLayout++));
    while (ui->layoutRasten->count() != nModel)
        delete ui->layoutRasten->itemAt(ui->layoutRasten->count() - 1)->widget();
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
        static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget())->updateValues();
    updateRastenDiagram();
}

void TabRezept::updateRastenDiagram()
{
    QLineSeries *series = new QLineSeries();
    int t = 0;
    series->append(t, bh->sud()->getEinmaischenTemp());
    for (int i = 0; i < ui->layoutRasten->count(); ++i)
    {
        const WdgRast* wdg = static_cast<WdgRast*>(ui->layoutRasten->itemAt(i)->widget());
        series->append(t, wdg->temperatur());
        t += wdg->dauer();
        series->append(t, wdg->temperatur());
    }
    QChart *chart = ui->diagramRasten->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
    QValueAxis *axis =  static_cast<QValueAxis*>(chart->axes(Qt::Horizontal).back());
    axis->setRange(0, t);
    axis->setLabelFormat("%d min");
    axis =  static_cast<QValueAxis*>(chart->axes(Qt::Vertical).back());
    axis->setRange(30, 80);
    axis->setLabelFormat("%d C");
}

void TabRezept::on_btnEinmaischtemperatur_clicked()
{
    int rastTemp = bh->sud()->modelRasten()->rowCount() > 0 ? bh->sud()->modelRasten()->data(0, "RastTemp").toInt() : 57;
    DlgEinmaischTemp dlg(bh->sud()->geterg_S_Gesammt(), 18, bh->sud()->geterg_WHauptguss(), rastTemp, this);
    if (dlg.exec() == QDialog::Accepted)
        bh->sud()->setEinmaischenTemp(dlg.value());
}

void TabRezept::on_btnNeueRast_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()}, {"RastTemp", 78}});
    bh->sud()->modelRasten()->append(values);
    ui->scrollAreaRasten->verticalScrollBar()->setValue(ui->scrollAreaRasten->verticalScrollBar()->maximum());
}

void TabRezept::malzGaben_modified()
{
    const int nModel = bh->sud()->modelMalzschuettung()->rowCount();
    int nLayout = ui->layoutMalzGaben->count();
    while (nLayout < nModel)
        ui->layoutMalzGaben->addWidget(new WdgMalzGabe(nLayout++));
    while (ui->layoutMalzGaben->count() != nModel)
        delete ui->layoutMalzGaben->itemAt(ui->layoutMalzGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget())->updateValues();
    updateMalzGaben();
}

void TabRezept::malzGaben_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(roles)
    int colProzent = bh->sud()->modelMalzschuettung()->fieldIndex("Prozent");
    QModelIndex index = topLeft;
    for (int i = topLeft.column(); i <= bottomRight.column(); ++i)
    {
        if (index.column() == colProzent)
            return updateMalzGaben();
        index = index.siblingAtColumn(index.column() + 1);
    }
    updateMalzDiagram();
}

void TabRezept::updateMalzGaben()
{
    if (!bh->sud()->getBierWurdeGebraut())
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.1)
            p = 0.0;
        for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
        {
            WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
    }
    updateMalzDiagram();
}

void TabRezept::updateMalzDiagram()
{
    QPieSeries *series = new QPieSeries();
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        const WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        series->append(wdg->name(), wdg->prozent());
    }
    QChart *chart = ui->diagramMalz->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
}

void TabRezept::on_btnNeueMalzGabe_clicked()
{
    double p = 100.0;
    for (int i = 0; i < ui->layoutMalzGaben->count(); ++i)
    {
        WdgMalzGabe* wdg = static_cast<WdgMalzGabe*>(ui->layoutMalzGaben->itemAt(i)->widget());
        p -= wdg->prozent();
    }
    if (fabs(p) < 0.1)
        p = 0.0;
    QVariantMap values({{"SudID", bh->sud()->id()}});
    int row = bh->sud()->modelMalzschuettung()->append(values);
    bh->sud()->modelMalzschuettung()->setData(row, "Prozent", p);
    ui->scrollAreaMalzGaben->verticalScrollBar()->setValue(ui->scrollAreaMalzGaben->verticalScrollBar()->maximum());
}

void TabRezept::hopfenGaben_modified()
{
    const int nModel = bh->sud()->modelHopfengaben()->rowCount();
    int nLayout = ui->layoutHopfenGaben->count();
    while (nLayout < nModel)
        ui->layoutHopfenGaben->addWidget(new WdgHopfenGabe(nLayout++));
    while (ui->layoutHopfenGaben->count() != nModel)
        delete ui->layoutHopfenGaben->itemAt(ui->layoutHopfenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget())->updateValues();
    updateHopfenGaben();
}

void TabRezept::hopfenGaben_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    Q_UNUSED(roles)
    int colProzent = bh->sud()->modelHopfengaben()->fieldIndex("Prozent");
    QModelIndex index = topLeft;
    for (int i = topLeft.column(); i <= bottomRight.column(); ++i)
    {
        if (index.column() == colProzent)
            return updateHopfenGaben();
        index = index.siblingAtColumn(index.column() + 1);
    }
    updateHopfenDiagram();
}

void TabRezept::updateHopfenDiagram()
{
    QPieSeries *series = new QPieSeries();
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        const WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        series->append(wdg->name(), wdg->prozent());
    }
    QChart *chart = ui->diagramHopfen->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
}

void TabRezept::updateHopfenGaben()
{
    if (!bh->sud()->getBierWurdeGebraut())
    {
        double p = 100.0;
        for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            p -= wdg->prozent();
        }
        if (fabs(p) < 0.1)
            p = 0.0;
        for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
        {
            WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
            wdg->setFehlProzent(p);
        }
    }
    updateHopfenDiagram();
}

void TabRezept::on_btnNeueHopfenGabe_clicked()
{
    double p = 100.0;
    for (int i = 0; i < ui->layoutHopfenGaben->count(); ++i)
    {
        WdgHopfenGabe* wdg = static_cast<WdgHopfenGabe*>(ui->layoutHopfenGaben->itemAt(i)->widget());
        p -= wdg->prozent();
    }
    if (fabs(p) < 0.1)
        p = 0.0;
    QVariantMap values({{"SudID", bh->sud()->id()}, {"Zeit", bh->sud()->getKochdauerNachBitterhopfung()}});
    int row = bh->sud()->modelHopfengaben()->append(values);
    bh->sud()->modelHopfengaben()->setData(row, "Prozent", p);
    ui->scrollAreaHopfenGaben->verticalScrollBar()->setValue(ui->scrollAreaHopfenGaben->verticalScrollBar()->maximum());
}

void TabRezept::on_cbBerechnungsartHopfen_currentIndexChanged(int index)
{
    if (ui->cbBerechnungsartHopfen->hasFocus())
    {
        bh->sud()->setberechnungsArtHopfen(index);
        updateHopfenGaben();
    }
}

void TabRezept::hefeGaben_modified()
{
    const int nModel = 1;//TODO: bh->sud()->modelHefegaben()->rowCount();
    int nLayout = ui->layoutHefeGaben->count();
    while (nLayout < nModel)
        ui->layoutHefeGaben->addWidget(new WdgHefeGabe(nLayout++));
    while (ui->layoutHefeGaben->count() != nModel)
        delete ui->layoutHefeGaben->itemAt(ui->layoutHefeGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutHefeGaben->count(); ++i)
        static_cast<WdgHefeGabe*>(ui->layoutHefeGaben->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeueHefeGabe_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()}});
    //bh->sud()->modelHefegaben()->append(values); // TODO: uncomment
    ui->scrollAreaHefeGaben->verticalScrollBar()->setValue(ui->scrollAreaHefeGaben->verticalScrollBar()->maximum());
}

void TabRezept::weitereZutatenGaben_modified()
{
    const int nModel = bh->sud()->modelWeitereZutatenGaben()->rowCount();
    int nLayout = ui->layoutWeitereZutatenGaben->count();
    while (nLayout < nModel)
        ui->layoutWeitereZutatenGaben->addWidget(new WdgWeitereZutatGabe(nLayout++));
    while (ui->layoutWeitereZutatenGaben->count() != nModel)
        delete ui->layoutWeitereZutatenGaben->itemAt(ui->layoutWeitereZutatenGaben->count() - 1)->widget();
    for (int i = 0; i < ui->layoutWeitereZutatenGaben->count(); ++i)
        static_cast<WdgWeitereZutatGabe*>(ui->layoutWeitereZutatenGaben->itemAt(i)->widget())->updateValues(true);
}

void TabRezept::on_btnNeueHopfenstopfenGabe_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()}, {"Typ", EWZ_Typ_Hopfen}});
    bh->sud()->modelWeitereZutatenGaben()->append(values);
    ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
}

void TabRezept::on_btnNeueWeitereZutat_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()}});
    bh->sud()->modelWeitereZutatenGaben()->append(values);
    ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->setValue(ui->scrollAreaWeitereZutatenGaben->verticalScrollBar()->maximum());
}

void TabRezept::anhaenge_modified()
{
    const int nModel = bh->sud()->modelAnhang()->rowCount();
    int nLayout = ui->layoutAnhang->count();
    while (nLayout < nModel)
        ui->layoutAnhang->addWidget(new WdgAnhang(nLayout++));
    while (ui->layoutAnhang->count() != nModel)
        delete ui->layoutAnhang->itemAt(ui->layoutAnhang->count() - 1)->widget();
    for (int i = 0; i < ui->layoutAnhang->count(); ++i)
        static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(i)->widget())->updateValues();
}

void TabRezept::on_btnNeuerAnhang_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()}});
    int index = bh->sud()->modelAnhang()->append(values);
    static_cast<WdgAnhang*>(ui->layoutAnhang->itemAt(index)->widget())->openDialog();
    ui->scrollAreaAnhang->verticalScrollBar()->setValue(ui->scrollAreaAnhang->verticalScrollBar()->maximum());
}

void TabRezept::on_tbSudnummer_valueChanged(int value)
{
    if (ui->tbSudnummer->hasFocus())
        bh->sud()->setSudnummer(value);
}

void TabRezept::on_tbSudname_textChanged(const QString &value)
{
    if (ui->tbSudname->hasFocus())
        bh->sud()->setSudname(value);
}

void TabRezept::on_tbMenge_valueChanged(double value)
{
    if (ui->tbMenge->hasFocus())
        bh->sud()->setMenge(value);
}

void TabRezept::on_tbSW_valueChanged(double value)
{
    if (ui->tbSW->hasFocus())
        bh->sud()->setSW(value);
}

void TabRezept::on_tbCO2_valueChanged(double value)
{
    if (ui->tbCO2->hasFocus())
        bh->sud()->setCO2(value);
}

void TabRezept::on_tbBittere_valueChanged(int value)
{
    if (ui->tbBittere->hasFocus())
        bh->sud()->setIBU(value);
}

void TabRezept::on_tbReifezeit_valueChanged(int value)
{
    if (ui->tbReifezeit->hasFocus())
        bh->sud()->setReifezeit(value);
}

void TabRezept::on_cbAnlage_currentIndexChanged(const QString &value)
{
    if (ui->cbAnlage->hasFocus())
        bh->sud()->setAuswahlBrauanlageName(value);
}

void TabRezept::on_tbKommentar_textChanged()
{
    if (ui->tbKommentar->hasFocus())
    {
        QString kommentar = ui->tbKommentar->toPlainText().replace("<br>", "\n");
        if (kommentar != bh->sud()->getKommentar())
            bh->sud()->setKommentar(kommentar);
    }
}

void TabRezept::on_tbHGF_valueChanged(int value)
{
    if (ui->tbHGF->hasFocus())
        bh->sud()->sethighGravityFaktor(value);
}

void TabRezept::on_tbFaktorHauptguss_valueChanged(double value)
{
    if (ui->tbFaktorHauptguss->hasFocus())
        bh->sud()->setFaktorHauptguss(value);
}

void TabRezept::on_tbRestalkalitaet_valueChanged(double value)
{
    if (ui->tbRestalkalitaet->hasFocus())
        bh->sud()->setRestalkalitaetSoll(value);
}

void TabRezept::on_tbEinmaischtemperatur_valueChanged(int temp)
{
    if (ui->tbEinmaischtemperatur->hasFocus())
        bh->sud()->setEinmaischenTemp(temp);
}

void TabRezept::on_tbKochzeit_valueChanged(int min)
{
    if (ui->tbKochzeit->hasFocus())
        bh->sud()->setKochdauerNachBitterhopfung(min);
}

void TabRezept::on_tbNachisomerisierungszeit_valueChanged(int min)
{
    if (ui->tbNachisomerisierungszeit->hasFocus())
        bh->sud()->setNachisomerisierungszeit(min);
}

void TabRezept::on_btnTagNeu_clicked()
{
    QVariantMap values({{"SudID", bh->sud()->id()},
                        {"Tagname", tr("Neuer Tag")}});
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    int row = model->append(values);
    if (row >= 0)
    {
        QModelIndex index = model->index(row, model->fieldIndex("Tagname"));
        ui->tableTags->setCurrentIndex(index);
        ui->tableTags->scrollTo(index);
        ui->tableTags->edit(index);
    }
}

void TabRezept::on_btnTagLoeschen_clicked()
{
    ProxyModel *model = bh->sud()->modelFlaschenlabelTags();
    for (const QModelIndex &index : ui->tableTags->selectionModel()->selectedIndexes())
        model->removeRow(index.row());
}
